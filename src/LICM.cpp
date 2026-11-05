// loop invariant code motion
// 1. detects loops using dfs
// 2. profit

#include "Backend.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

#include <cstdint>

using namespace llvm;

using BB = BasicBlock;
using Edge = std::pair<BB*, BB*>;

struct Loop
{
    BasicBlock* preheader;
    BasicBlock* header;

    // if there is a multi-latch loop we treat each of them
    // as a seperate loop. it doesnt change much i think
    BasicBlock* latch;

    // we store all blocks between header and latch so that
    // we can identify instructions outside of the loop faster.
    // does not include the preheader, obviously
    SmallPtrSet<BB*, 2> blocks;

    SetVector<Instruction*> invariant;

    // when creating a loop we first have to make sure
    // the given edge is an back edge i.e. header dominates latch.
    // we test for this by doing dfs from the latch upwards
    // if we reach the root, we know its not dominating.
    static Loop* create(BB* preheader, BB* header, BB* latch)
    {
        auto loop = new Loop;

        SmallVector<BB*> queue;
        auto& visited = loop->blocks;

        queue.push_back(latch);

        visited.insert(latch);
        visited.insert(header);

        while (!queue.empty())
        {
            auto bb = queue.pop_back_val();

            if (bb == header)
                continue;

            if (bb->isEntryBlock())
                return nullptr;

            for (auto pred : predecessors(bb))
                if (!visited.contains(pred))
                {
                    visited.insert(pred);
                    queue.push_back(pred);
                }
        }

        loop->preheader = preheader;
        loop->header = header;
        loop->latch = latch;

        loop->header->setName("licm-header");
        loop->latch->setName("licm-latch");

        return loop;
    }

    bool contains(Instruction* inst) { return blocks.contains(inst->getParent()); }

    // does not recurse on instruction operands
    bool isInvariantShallow(Value* value)
    {
        if (isa<Constant>(value))
            return true;

        if (isa<Argument>(value))
            return true;

        if (auto inst = dyn_cast<Instruction>(value))
        {
            if (invariant.contains(inst))
                return true;

            // the instruction lies outside of the loop
            if (!this->contains(inst))
                return true;
        }

        return false;
    }

    // TODO: what about globals?
    bool isInvariantDeep(Value* value)
    {
        if (isInvariantShallow(value))
            return true;

        if (!isa<Instruction>(value))
            return false;

        auto inst = static_cast<Instruction*>(value);

        for (auto& op : inst->operands())
            if (!isInvariantShallow(op.get()))
                return false;

        // for load and stores we would need alias analysis
        // to verify a load doesnt depend on a store of a previous iteration
        if (isa<LoadInst>(inst))
            return false;

        if (isa<StoreInst>(inst))
            return false;

        if (isa<CallInst>(inst))
            return false;

        if (isa<PHINode>(inst))
            return false;

        return true;
    }

    // fixpoint iteration which marks instructions as invariant
    // it fills up the invariant set with all invariant instructions
    void findInvariantInstructions()
    {
        bool hasChanged;
        int n = 10000; // I really dont wanna f up our tests on the last day

        do
        {
            hasChanged = false;

            for (auto bb : blocks)
                for (auto& inst : *bb)
                {
                    if (invariant.contains(&inst))
                        continue;

                    if (isInvariantDeep(&inst))
                    {
                        invariant.insert(&inst);
                        hasChanged = true;
                    }
                }

        } while (hasChanged && n-- > 0);
    }

    void moveInvariantsToPreheader()
    {
        auto location = preheader->getTerminator();

        for (auto inst : invariant)
            inst->moveBefore(location);
    }
};

static bool isRetreatEdge(BB* src, BB* dst, SmallDenseMap<BB*, uint32_t>& pretime)
{
    if (!pretime.contains(src) || !pretime.contains(dst))
        return false;

    return pretime[dst] < pretime[src];
}

struct LoopFinder
{
    int time = 0;
    SmallPtrSet<BasicBlock*, 8> visited;
    SmallDenseMap<BasicBlock*, uint32_t> pretime;

    SmallVector<Edge> retreatEdges;

    void clear()
    {
        time = 0;
        visited.clear();
        pretime.clear();
        retreatEdges.clear();
    }

    SmallVector<std::pair<BasicBlock*, BasicBlock*>> findRetreatEdges(BasicBlock* bb)
    {
        visited.insert(bb);
        pretime[bb] = time++;

        for (auto succ : successors(bb))
        {
            if (!visited.contains(succ))
                findRetreatEdges(succ);

            else if (isRetreatEdge(bb, succ, pretime))
                retreatEdges.push_back({succ, bb});
        }

        return retreatEdges;
    }
};

// does bb1 dominate bb2 ?
// instead of computing the full dominator tree we just do bfs from bb2 upwards
// if there is a path to root without bb on it, it is not dominating!
// this might be more expensive than computing the entire tree
// but since many functions only contain one loop, this is fine :0
static bool dominates(BasicBlock* bb1, BasicBlock* bb2)
{
    // not a queue, so its actually not bfs but same same
    SmallVector<BasicBlock*> queue;
    SmallPtrSet<BasicBlock*, 8> visited;

    queue.push_back(bb2);

    while (!queue.empty())
    {
        auto bb = queue.pop_back_val();

        // this path contains bb1
        // and thus can be short-circuited
        if (bb == bb1)
            continue;

        // this can only be the function entry block
        // thus there IS a path from bb2 to root without bb1
        // so bb1 is not dominating bb2
        if (bb->hasNPredecessors(0))
            return false;

        for (auto pred : predecessors(bb))
            if (visited.insert(pred).second)
                queue.push_back(pred);
    }

    return true;
}

static BB* createLoopPreHeader(BB* header, SmallDenseMap<BB*, uint32_t>& pretime)
{
    SmallVector<BB*> nonLoopPredecessors;

    for (auto pred : predecessors(header))
        if (!isRetreatEdge(pred, header, pretime))
            nonLoopPredecessors.push_back(pred);

    auto& context = header->getContext();
    auto function = header->getParent();
    auto preheader = BasicBlock::Create(context, "licm-preheader", function);
    BranchInst::Create(header, preheader);

    for (auto pred : nonLoopPredecessors)
    {
        auto br = static_cast<BranchInst*>(pred->getTerminator());
        br->replaceSuccessorWith(header, preheader);
    }

    function->getEntryBlock().setName("entry");

    // okay, if you could follow along so far then this is the end.
    // we have to update the phi nodes, but only some of them.
    // some incoming phi values come from within the loop, we dont care bout them.
    // but the others now need to come from the preheader.
    // Well, so the preheader needs phi's to deal with its predecessors
    // and pass the value onto the phi of the header.
    //      Maybe this is best explained with a Picture.

    // header:
    //   %x = phi i32 [ %a, %non_loop_pred_1 ], [ %b, %non_loop_pred_2 ], [ %c, %loop_pred ]

    // preheader:
    //      %ab = phi i32 [ %a, %non_loop_pred_1 ], [ %b, %non_loop_pred_2 ]
    //      br label header
    //
    // header:
    //      %x = phi i32 [ %ab, %preheader ], [ %c, %loop_pred ]

    // shoutout to claude for helping me understand this.
    //      (all code is still handwritten and my intellectual creation tho)

    InsertPosition insertBeforeBranch = preheader->getTerminator();
    size_t size = nonLoopPredecessors.size();

    for (auto& headerPhi : header->phis())
    {
        auto type = headerPhi.getType();
        auto preheaderPhi = PHINode::Create(type, size, "", insertBeforeBranch);

        for (auto pred : nonLoopPredecessors)
            preheaderPhi->addIncoming(headerPhi.removeIncomingValue(pred), pred);

        headerPhi.addIncoming(preheaderPhi, preheader);
    }

    // finally since we were sloppy earlier we clean up degenerate phis
    SmallVector<PHINode*> buf;
    for (auto& phi : preheader->phis())
    {
        if (phi.getNumOperands() == 1)
        {
            auto val = phi.getIncomingValue(0);
            phi.replaceAllUsesWith(val);
            buf.push_back(&phi);
        }
    }

    for (auto phi : buf)
        phi->eraseFromParent();

    return preheader;
}

void Backend::licm(void)
{
    LoopFinder loopFinder;

    for (auto& function : m_Module)
    {
        if (function.isDeclaration())
            continue;

        // 1. find retreat edges
        loopFinder.findRetreatEdges(&function.getEntryBlock());

        for (auto [header, latch] : loopFinder.retreatEdges)
        {

            // 2. check if its a back edge
            if (!dominates(header, latch))
                continue;

            // 3. create a preheader
            auto preheader = createLoopPreHeader(header, loopFinder.pretime);

            auto loop = Loop::create(preheader, header, latch);

            loop->findInvariantInstructions();
            loop->moveInvariantsToPreheader();
        }

        loopFinder.clear();
    }
}
