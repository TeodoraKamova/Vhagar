#include "Backend.h"
#include "Codegen.h"
#include "ConstIntDiv.h"
#include "SCCP.h"
#include "TCO.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Pass.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Transforms/Utils.h"

#include <cassert>

using namespace llvm;

void Backend::codegen(const Ast& ast, const Locator& locator)
{
    Codegen codegen(*this, ast, locator);

    codegen.emitToplevel();
}

void Backend::sccp(void) { SCCP(*this).run(); }

void Backend::verifyModule()
{
    llvm::verifyModule(m_Module, &outs());
    return;
}

void Backend::mem2reg()
{
    legacy::FunctionPassManager pm(&m_Module);
    pm.add(createPromoteMemoryToRegisterPass());

    pm.doInitialization();
    for (Function& function : m_Module)
        pm.run(function);

    pm.doFinalization();
}

void Backend::emitModule(const std::string& outfile) const
{
    std::error_code error;
    raw_fd_ostream os(outfile, error, sys::fs::OpenFlags::OF_Text);

    m_Module.print(os, nullptr);
}

// late night cfg optimizations

static bool removeUnreachableBlocks(BasicBlock& bb, Backend::Buffer& buffer)
{
    if (bb.hasNPredecessorsOrMore(1))
        return false;

    if (bb.isEntryBlock())
        return false;

    // queue up to be removed
    buffer.push_back(&bb);

    return true;
}

// merges two basic blocks when legal
//
// put updated labels into successor phi nodes
// copy instructions into predecessor block
// and delete bb by appending it to the buffer
//
//  bb1:
//      %a = add i32 %x, 1
//      br label %bb2
//
//  bb2:
//      %b = mul i32 %a, 2
//      ret i32 %b
//
// ...this cfg would become...
//
//  bb1:
//      %a = add i32 %x, 1
//      %b = mul i32 %a, 2
//      ret i32 %b
//
// can this introduce a critical edge? no, I believe
static bool mergeStraightLineBlocks(BasicBlock& bb, Backend::Buffer& buffer)
{
    auto pred = bb.getSinglePredecessor();

    if (!pred || pred->getSingleSuccessor() != &bb)
        return false;

    bb.replaceSuccessorsPhiUsesWith(pred);

    auto term = pred->getTerminator();
    while (!bb.empty())
        bb.front().moveBefore(term);
    term->eraseFromParent();

    buffer.push_back(&bb);
    return true;
}

// replaces useless conditional with unconditional branches
//
//  br i1 true, label %BB1, label %BB2
//  br label %BB1
//
//  br i1 %cond, label %BB, label %BB
//  br label %BB
//
static bool simplifyBranchInstr(BasicBlock& bb, [[maybe_unused]] Backend::Buffer& buffer)
{
    auto term = bb.getTerminator();

    if (!isa<BranchInst>(*term))
        return false;

    auto br = static_cast<BranchInst*>(term);

    if (br->isUnconditional())
        return false;

    auto succ = bb.getUniqueSuccessor();

    if (succ == nullptr)
        return false;

    IRBuilder<> builder(br);
    builder.CreateBr(succ);

    br->eraseFromParent();
    return true;
}

using OptPass = bool (*)(BasicBlock&, Backend::Buffer&);

static void fixpoint(Function& function, Backend::Buffer& buffer, OptPass opt)
{
    bool hasChanged;

    do
    {
        hasChanged = false;

        for (BasicBlock& bb : function)
            hasChanged |= opt(bb, buffer);

        for (auto val : buffer)
        {
            auto bb = static_cast<BasicBlock*>(val);

            for (auto succ : successors(bb))
                succ->removePredecessor(bb);

            bb->eraseFromParent();
        }

        buffer.clear();
    } while (hasChanged);
}

// 1 incoming edge
// 1 outgoing edge
// no instructions except br
static bool isUncondBranchOnlyBlock(BasicBlock& bb)
{
    return (bb.size() == 1) && bb.getSingleSuccessor() && bb.hasNPredecessors(1);
}

static PHINode* getSinglePhiNode(BasicBlock& bb)
{
    auto iter = bb.phis();

    if (iter.empty())
        return nullptr;

    PHINode* phi = nullptr;
    for (auto& node : iter)
    {
        if (phi)
            return nullptr;

        phi = &node;
    }

    return phi;
}

//          top
//         |   |
//       lhs   rhs
//         |   |
//          bot
//
// this is a diamond, but it only becomes a phiamond, if
//  1. the bot bb has a single phi node
//  2. both lhs, rhs have only an unconditional branch to bot
//
// I don't think this is the most general pattern yet.
// TODO: generalize to any number of independent phis
static bool phiamond(BasicBlock& top, Backend::Buffer& buffer)
{
    auto term = top.getTerminator();

    if (!isa<BranchInst>(*term))
        return false;

    auto br = static_cast<BranchInst*>(term);

    if (br->getNumSuccessors() != 2)
        return false;

    auto lhs = br->getSuccessor(0);
    auto rhs = br->getSuccessor(1);

    if (!isUncondBranchOnlyBlock(*lhs) || !isUncondBranchOnlyBlock(*rhs))
        return false;

    if (lhs->getSingleSuccessor() != rhs->getSingleSuccessor())
        return false;

    auto bot = lhs->getSingleSuccessor();

    if (!bot->hasNPredecessors(2))
        return false;

    PHINode* phi = getSinglePhiNode(*bot);

    if (phi == nullptr)
        return false;

    // now we have all we need for our phiamond
    // time to attach bot to top, and replace the phi with a select

    IRBuilder<> builder(br); // TODO: backend has a builder, use it!

    auto cond = br->getCondition();
    auto val0 = phi->getIncomingValue(0);
    auto val1 = phi->getIncomingValue(1);

    auto cmov = builder.CreateSelect(cond, val0, val1);
    builder.CreateBr(bot);

    phi->replaceAllUsesWith(cmov);

    br->eraseFromParent();
    phi->eraseFromParent();

    buffer.push_back(lhs);
    buffer.push_back(rhs);

    return true;
}

void Backend::simplifyCFG()
{
    m_Buffer.clear();

    for (Function& function : m_Module)
    {
        fixpoint(function, m_Buffer, ::removeUnreachableBlocks);
        fixpoint(function, m_Buffer, ::mergeStraightLineBlocks);
        fixpoint(function, m_Buffer, ::simplifyBranchInstr);
        fixpoint(function, m_Buffer, ::phiamond);
        fixpoint(function, m_Buffer, ::mergeStraightLineBlocks);
    }
}

void Backend::removeUnreachableBlocks(Function& function)
{
    fixpoint(function, m_Buffer, ::removeUnreachableBlocks);
}

void Backend::constDiv(void) { return ConstIntDiv(*this).run(); }

void Backend::tco(void) { return TCO(*this).run(); }

void Backend::dedupGlobalStringData(void)
{
    StringMap<GlobalValue*> map;

    m_Buffer.clear();
    for (auto& global : m_Module.globals())
    {
        if (global.hasNUses(0))
        {
            m_Buffer.push_back(&global);
            continue;
        }

        if (!global.hasInitializer())
            continue;

        auto data = dyn_cast<ConstantDataArray>(global.getInitializer());

        if (!data || !data->isString())
            continue;

        if (auto value = map.lookup(data->getAsString()))
        {
            global.replaceAllUsesWith(value);
            m_Buffer.push_back(&global);
        }
        else
        {
            map.insert({data->getAsString(), &global});
        }
    }

    for (auto global : m_Buffer)
        static_cast<GlobalVariable*>(global)->eraseFromParent();

    m_Buffer.clear();
}
