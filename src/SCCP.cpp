#include "SCCP.h"
#include "Diagnostics.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

#include <cstdint>
#include <stack>
//#define DEBUG_PRINTS
// TODO: we are currently handling undefined values like BOT, which causes problems with
// join operations. As anything combined with undef should be undef as well (it is kinda the TOP
// over TOP) so, need to make a new element in the lattice UNDEF which is greater that TOP.
// BOT -> what does this then even mean? Do we really need a new state?

void SCCP::run(void)
{
    for (llvm::Function& fun : m_Module)
        // skip declarations
        if (!fun.isDeclaration())
            // sccp each function until we
            // cannot find any more constants
            while (sccpFun(fun) == false)
                ;
}

// helper to determine whether two constants are equal
bool areConstantsEqual(const llvm::Constant* const1, const llvm::Constant* const2);

#define TOP_PTR (nullptr)
#define UNDEF_PTR (reinterpret_cast<llvm::Constant*>(static_cast<uintptr_t>(1)))

struct LattHM : llvm::DenseMap<const llvm::Value*, llvm::Constant*>
{
private:
    uint32_t m_Changes = 0;

public:
    enum class Type
    {
        TOP,   // if element exists but is topptr
        UNDEF, // if element does not exist or is undef (everything but the function argument is
               // undef by default)
        VALUE  // if it exist and is not topptr nor undef
    };

    /// @brief checks whether the pointer is neither top nor bottom
    static constexpr bool isValidPtr(const llvm::Constant* ptr)
    {
        return reinterpret_cast<uintptr_t>(ptr) > 1; // may not be 0 or 1
    }

    /// @brief calculate meet (greatest lower bound) of ptr1 and ptr2
    // static llvm::Constant* meet(llvm::Constant* ptr1, llvm::Constant* ptr2)
    // {
    //     if (ptr1 == BOT_PTR || ptr2 == BOT_PTR)
    //         return BOT_PTR;
    //
    //     if (isValidPtr(ptr1) && isValidPtr(ptr2))
    //         return BOT_PTR;
    //
    //     return TOP_PTR;
    // }

    /// @brief calculate join (leat upper bound) of ptr1 and ptr2
    static constexpr llvm::Constant* join(llvm::Constant* ptr1, llvm::Constant* ptr2)
    {
        // either is top -> top
        if (ptr1 == TOP_PTR || ptr2 == TOP_PTR)
            return TOP_PTR;

        if (ptr1 == UNDEF_PTR && ptr2 == UNDEF_PTR)
            return UNDEF_PTR;

        if (ptr1 != UNDEF_PTR && ptr2 == UNDEF_PTR)
            return ptr1;

        if (ptr1 == UNDEF_PTR && ptr2 != UNDEF_PTR)
            return ptr2;

        // bot are not bot -> bot
        if (ptr1 != UNDEF_PTR && ptr2 != UNDEF_PTR)
            return areConstantsEqual(ptr1, ptr2) ? ptr1 : TOP_PTR;

        // one is bot the other not top -> the other
        // return reinterpret_cast<llvm::Constant*>(
        //     ((reinterpret_cast<uintptr_t>(ptr1) | reinterpret_cast<uintptr_t>(ptr2)) ^
        //      ((uintptr_t)(1))));
        return TOP_PTR;
    }

    static llvm::Constant* joinIfNotUndef(llvm::Constant* ptr1, llvm::Constant* ptr2)
    {
        if (ptr1 == UNDEF_PTR || ptr2 == UNDEF_PTR)
            return UNDEF_PTR;
        return join(ptr1, ptr2);
    }

    Type getType(llvm::Value* k)
    {
        const auto* c = getConstant(k);
        if (c == TOP_PTR)
            return Type::TOP;

        if (c == UNDEF_PTR)
            return Type::UNDEF;

        return Type::VALUE;
    }

    static constexpr const char* typeToStr(const llvm::Constant* t)
    {
        switch (reinterpret_cast<uintptr_t>(t))
        {
            case 1: return "BOT";
            case 0: return "TOP";
            default: return "VALUE";
        }
    }

    llvm::Constant* getConstant(llvm::Value* k) const
    {
        // is undefined value, so its bot in the lattuce
        if (llvm::dyn_cast<llvm::UndefValue>(k))
            return UNDEF_PTR;

        // already constant, no need to invoque the HM
        if (auto* constVal = llvm::dyn_cast<llvm::Constant>(k))
            return constVal;

        const auto it = find(k);
        if (it == end())
            return UNDEF_PTR;

        else if (it->second == nullptr)
            return TOP_PTR;

        return it->second;
    }

    void mergeInto(llvm::Value* key, llvm::Constant* val)
    {
        auto it = find(key);

        // if it is undefined
        if (it == end())
        {
            operator[](key) = val;
            m_Changes++;
        }

        else if (it->second == UNDEF_PTR)
        {
            operator[](key) = val;
            if (it->second != val)
                m_Changes++;
        }

        // if values are not equal and not already top, make top
        else if (!areConstantsEqual(it->second, val) && it->second != TOP_PTR)
        {
            // insert(std::make_pair(key, TOP_PTR));
            operator[](key) = TOP_PTR;
            m_Changes++;
        }

        else if (!isValidPtr(val))
        {
            operator[](key) = val;
            if (it->second != val)
                m_Changes++;
        }

        // otherwise constants are equal and or already top
        // -> no need to update the HM
    }

    uint32_t getNumChanges(void) const { return m_Changes; }
};

bool areConstantsEqual(const llvm::Constant* const1, const llvm::Constant* const2)
{
    if (!LattHM::isValidPtr(const1) || !LattHM::isValidPtr(const2))
        return false;

    return const1 == const2;
}

std::ostream& operator<<(std::ostream& os, const LattHM& latt)
{
    os.flush();
    for (const auto& v : latt)
    {
        v.first->print(llvm::outs());
        std::cout << "\t -> ";
        if (LattHM::isValidPtr(v.second))
            std::cout << v.second->getUniqueInteger().getZExtValue() << " ("
                      << v.second->getUniqueInteger().getSExtValue() << ')' << std::endl;
        else
            std::cout << LattHM::typeToStr(v.second) << std::endl;
    }
    return os.flush();
}

/// @return true iff we pruned out something, which means we
/// may be able to find out more constants now
bool SCCP::sccpFun(llvm::Function& fun)
{
    LattHM latt;

    // make all arguments top
    for (auto& arg : fun.args())
        latt.mergeInto(&arg, TOP_PTR);

    // iterate until we find a fixpoint
    uint32_t numChangesLastIt, counter = 0;
    do
    {

        numChangesLastIt = latt.getNumChanges();

        if (counter++ >= 100'000)
#ifdef NDEBUG
            return false; // everything is fine
#else
            diagnostics::error("DEBUG INFO: unrealistically high number of changes, may not halt.");
#endif

        for (llvm::BasicBlock& bb : fun)
            for (llvm::Instruction& instr : bb)
                latt.mergeInto(&instr, propagateInstr(&instr, &latt));

    } while (numChangesLastIt != latt.getNumChanges());

#ifdef DEBUG_PRINTS
    std::cout << "Result for function " << fun.getName().data() << ':' << std::endl;
    std::cout << latt << std::endl;
    // return true;
#endif

    replaceByConstant(fun, &latt);
    const uint32_t replaced = pruneUnreachableBlocks(fun);
    // replaced += dce(fun, &latt); // untested
    const uint32_t dcerepl = dce(fun);
    // assert(dcerepl == 0 && "dce actually removed stuff, wow");

    // done, when nothing could be replaced
    // return true;
    return replaced + dcerepl == 0;
}

llvm::Constant* SCCP::propagateInstr(llvm::Instruction* inst, LattHM* latt)
{
    if (auto* binOp = llvm::dyn_cast<llvm::BinaryOperator>(inst))
        return propagateBinaryOperator(binOp, latt);
    if (llvm::dyn_cast<llvm::StoreInst>(inst))
        return propagateStoreInst();
    if (auto* icmpInst = llvm::dyn_cast<llvm::ICmpInst>(inst))
        return propagateICmpInst(icmpInst, latt);
    if (auto* phi = llvm::dyn_cast<llvm::PHINode>(inst))
        return propagatePhiNode(phi, latt);
    if (auto* ret = llvm::dyn_cast<llvm::ReturnInst>(inst))
        return propagateRet(ret, latt);
    if (auto* br = llvm::dyn_cast<llvm::BranchInst>(inst))
        return propagateBranch(br, latt);
    if (llvm::dyn_cast<llvm::CallInst>(inst))
        return propagateCall();
    if (auto* sel = llvm::dyn_cast<llvm::SelectInst>(inst))
        return propagateSelect(sel, latt);
    if (auto* cast = llvm::dyn_cast<llvm::CastInst>(inst))
        return propagateCast(cast, latt);

// TODO: in codegen.cpp search for der.Create

// skip all other
#ifdef DEBUG_PRINTS
    {
        (std::cout << "need to implement ").flush();
        inst->print(llvm::outs());
        std::cout << std::endl;
    }
#endif

    return nullptr;
}

inline bool isIntTyInRange(const llvm::Constant* c)
{
    const auto ty = c->getType();
    return ty->isIntegerTy(32) || ty->isIntegerTy(8);
}

llvm::Constant* SCCP::propagateBinaryOperator(llvm::BinaryOperator* binOp, LattHM* latt)
{
    const llvm::Value* trueValueLhs = binOp->getOperand(0);
    llvm::Constant* lhs = latt->getConstant(binOp->getOperand(0));
    const llvm::Value* trueValueRhs = binOp->getOperand(1);
    llvm::Constant* rhs = latt->getConstant(binOp->getOperand(1));

    if (LattHM::isValidPtr(lhs) && LattHM::isValidPtr(rhs))
    {
        // we need to have int32 types, because there exists
        // %16 = sdiv exact i64 %15, ptrtoint (ptr getelementptr ({ i32, i32, i32 }, ptr null, i32
        // 1) to i64)
        // and maybe even scarier stuff
        if (!isIntTyInRange(lhs) || !isIntTyInRange(rhs))
            return LattHM::joinIfNotUndef(lhs, rhs);

        const auto opcode = binOp->getOpcode();
        bool defined = true;
        uint64_t result = 0;
        switch (opcode)
        {
            case llvm::Instruction::SDiv:
            {
                const int32_t rhsv = (int32_t)rhs->getUniqueInteger().getSExtValue();
                if (rhsv == 0)
                    defined = false;
                else
                    result = (int32_t)lhs->getUniqueInteger().getSExtValue() / rhsv;
                break;
            }
            case llvm::Instruction::UDiv:
            {
                const uint32_t rhsv = (uint32_t)rhs->getUniqueInteger().getSExtValue();
                if (rhsv == 0)
                    defined = false;
                else
                    result = (uint32_t)lhs->getUniqueInteger().getSExtValue() / rhsv;
                break;
            }
            case llvm::Instruction::SRem:
            {
                const int32_t rhsv = (int32_t)rhs->getUniqueInteger().getSExtValue();
                if (rhsv == 0)
                    defined = false;
                else
                    result = (int32_t)lhs->getUniqueInteger().getSExtValue() % rhsv;
                break;
            }
            case llvm::Instruction::URem:
            {
                const uint32_t rhsv = (uint32_t)rhs->getUniqueInteger().getSExtValue();
                if (rhsv == 0)
                    defined = false;
                else
                    result = (uint32_t)lhs->getUniqueInteger().getSExtValue() % rhsv;
                break;
            }
            case llvm::Instruction::Shl:
            {
                result = lhs->getUniqueInteger().getZExtValue()
                         << rhs->getUniqueInteger().getZExtValue();
                break;
            }
            case llvm::Instruction::LShr:
            {
                result = (uint64_t)lhs->getUniqueInteger().getZExtValue() >>
                         (uint64_t)rhs->getUniqueInteger().getZExtValue();
                break;
            }
            case llvm::Instruction::AShr:
            {
                result = lhs->getUniqueInteger().getSExtValue() >>
                         rhs->getUniqueInteger().getZExtValue();
                break;
            }
            case llvm::Instruction::And:
            {
                result =
                    lhs->getUniqueInteger().getZExtValue() & rhs->getUniqueInteger().getZExtValue();
                break;
            }
            case llvm::Instruction::Or:
            {
                result =
                    lhs->getUniqueInteger().getZExtValue() | rhs->getUniqueInteger().getZExtValue();
                break;
            }

            default:
#ifdef DEBUG_PRINTS
                llvm::outs() << "trying to constexpr " << opcode << '\n';
#endif
                return llvm::ConstantExpr::get(opcode, lhs, rhs);
        }

        if (!defined)
            return UNDEF_PTR;

        return llvm::ConstantInt::get(binOp->getType(), result);
    }

    // if just one of them is undef, we imm. say its undef (as with gives use more pruning
    // possibilities , for example "if (c) ..." and c was proven to be undev->we don't have to do
    // the if)
    else if (lhs == UNDEF_PTR || rhs == UNDEF_PTR)
        return UNDEF_PTR;

    // special cases such as muiltiplication by +/-1 or 0, rem/div by +/-1
    else // if (false)
    {
        llvm::Constant* cons = LattHM::isValidPtr(lhs) ? lhs : rhs;
        const auto opcode = binOp->getOpcode();
        switch (opcode)
        {
            // +/-1 will be handled by const int div, so sonly need to care for:
            // a / a = 1
            // 0 / a = 0
            // a / 0 = undefined
            case llvm::Instruction::SDiv:
            {
                if (trueValueLhs == trueValueRhs)
                    return llvm::ConstantInt::get(binOp->getType(), 1);

                if (LattHM::isValidPtr(lhs))
                    if (isIntTyInRange(lhs))
                        if (lhs->getUniqueInteger().getZExtValue() == 0)
                            return llvm::ConstantInt::get(binOp->getType(), 0);

                if (LattHM::isValidPtr(rhs))
                    if (isIntTyInRange(rhs))
                        if (rhs->getUniqueInteger().getZExtValue() == 0)
                            return UNDEF_PTR;
                break;
            }

            // a * 0 = 0
            // 0 * a = 0
            case llvm::Instruction::Mul:
            // a & 0 = 0
            // 0 & a = 0
            case llvm::Instruction::And:
            {
                if (LattHM::isValidPtr(cons))
                    if (isIntTyInRange(cons))
                        if (cons->getUniqueInteger().getZExtValue() == 0)
                            return llvm::ConstantInt::get(binOp->getType(), 0);
                break;
            }

            // a % a = 0
            // a % 1 = 0
            case llvm::Instruction::SRem:
            {
                if (trueValueLhs == trueValueRhs)
                    return llvm::ConstantInt::get(binOp->getType(), 0);
                if (LattHM::isValidPtr(rhs))
                    if (isIntTyInRange(rhs))
                        if (rhs->getUniqueInteger().getZExtValue() == 1)
                            return llvm::ConstantInt::get(binOp->getType(), 0);
                break;
            }

            // a ^ a = 0
            case llvm::Instruction::Xor:
            // a - a = 0
            case llvm::Instruction::Sub:
            {
                if (trueValueLhs == trueValueRhs)
                    return llvm::ConstantInt::get(binOp->getType(), 0);
                break;
            }

            //  a | -1 = -1
            // -1 |  a = -1
            case llvm::Instruction::Or:
            {
                if (LattHM::isValidPtr(cons))
                    if (isIntTyInRange(cons))
                        if (cons->getUniqueInteger().getSExtValue() == -1)
                            return llvm::ConstantInt::get(binOp->getType(), -1, true);
                break;
            }

            // we can sadly not do anything :(
            default: break;
        }
    }
    return LattHM::joinIfNotUndef(lhs, rhs);
}

llvm::Constant* SCCP::propagateStoreInst(void)
{
    return TOP_PTR; // storing is out of the game. TODO: how can we do better
}

llvm::Constant* SCCP::propagateICmpInst(llvm::ICmpInst* icmpInst, LattHM* latt)
{
    llvm::Constant* lhs = latt->getConstant(icmpInst->getOperand(0));
    llvm::Constant* rhs = latt->getConstant(icmpInst->getOperand(1));

    if (LattHM::isValidPtr(lhs) && LattHM::isValidPtr(rhs))
    {
        // sicher ist sicher
        if (!isIntTyInRange(lhs) || !isIntTyInRange(rhs))
            return LattHM::joinIfNotUndef(lhs, rhs);

        // Evaluate the comparison
        bool result = false;
        const auto pred = icmpInst->getPredicate();
        switch (pred)
        {
            case llvm::ICmpInst::ICMP_EQ: result = areConstantsEqual(lhs, rhs); break;
            case llvm::ICmpInst::ICMP_NE: result = !areConstantsEqual(lhs, rhs); break;
            case llvm::ICmpInst::ICMP_SGE:
                result = lhs->getUniqueInteger().getZExtValue() >=
                         rhs->getUniqueInteger().getZExtValue();
                break;
            case llvm::ICmpInst::ICMP_SGT:
                result =
                    lhs->getUniqueInteger().getZExtValue() > rhs->getUniqueInteger().getZExtValue();
                break;
            case llvm::ICmpInst::ICMP_SLE:
                result = lhs->getUniqueInteger().getZExtValue() <=
                         rhs->getUniqueInteger().getZExtValue();
                break;
            case llvm::ICmpInst::ICMP_SLT:
                result =
                    lhs->getUniqueInteger().getZExtValue() < rhs->getUniqueInteger().getZExtValue();
                break;
            case llvm::ICmpInst::ICMP_UGE:
                result = lhs->getUniqueInteger().getSExtValue() >=
                         rhs->getUniqueInteger().getSExtValue();
                break;
            case llvm::ICmpInst::ICMP_UGT:
                result =
                    lhs->getUniqueInteger().getSExtValue() > rhs->getUniqueInteger().getSExtValue();
                break;
            case llvm::ICmpInst::ICMP_ULE:
                result = lhs->getUniqueInteger().getSExtValue() <=
                         rhs->getUniqueInteger().getSExtValue();
                break;
            case llvm::ICmpInst::ICMP_ULT:
                result =
                    lhs->getUniqueInteger().getSExtValue() < rhs->getUniqueInteger().getSExtValue();
                break;
            default:
                icmpInst->print(llvm::outs());
                diagnostics::todo(
                    "else if (auto* icmpInst = llvm::dyn_cast<llvm::ICmpInst>(inst)), case ",
                    std::to_string(pred).c_str());
                break;
        }
        return llvm::ConstantInt::get(icmpInst->getType(), result ? 1 : 0);
    }

    return LattHM::joinIfNotUndef(lhs, rhs);
}

llvm::Constant* SCCP::propagatePhiNode(llvm::PHINode* phi, LattHM* latt)
{
    const unsigned nodes = phi->getNumIncomingValues();
    llvm::Constant* join = UNDEF_PTR;
    for (unsigned i = 0; i < nodes; i++)
    {
        llvm::Value* v = phi->getIncomingValue(i);
        llvm::Constant* c = latt->getConstant(v);
        join = LattHM::join(join, c);
    }
    return join;
}

llvm::Constant* SCCP::propagateRet(llvm::ReturnInst* ret, LattHM* latt)
{
    // Check if the value being stored is a constant
    llvm::Value* rettype = ret->getReturnValue();

    // will be null if we return void
    if (!rettype)
        return TOP_PTR;

    llvm::Constant* retVal = latt->getConstant(rettype);
    if (LattHM::isValidPtr(retVal))
    {
        latt->mergeInto(ret, retVal);
        return retVal;
    }
    return retVal;
}

llvm::Constant* SCCP::propagateBranch(llvm::BranchInst* br, LattHM* latt)
{
    if (br->isConditional())
    {
        llvm::Value* condition = br->getCondition();
        llvm::Constant* constant = latt->getConstant(condition);
        if (LattHM::isValidPtr(constant))
            return constant;
    }

    return TOP_PTR;
}

llvm::Constant* SCCP::propagateCall(void)
{
    // sadly, we cannot assert anything about other functions (yet)
    // we may be in the future? At least for non-recursive calls?
    return TOP_PTR;
}

llvm::Constant* SCCP::propagateSelect(llvm::SelectInst* sel, LattHM* latt)
{
    // if the condition is constant, use this information.
    llvm::Constant* cond = latt->getConstant(sel->getCondition());
    llvm::Constant* ifTrue = latt->getConstant(sel->getTrueValue());
    llvm::Constant* ifFalse = latt->getConstant(sel->getFalseValue());
    if (LattHM::isValidPtr(cond))
    {
        if (cond->getUniqueInteger().getBoolValue())
            return ifTrue;
        return ifFalse;
    }

    // if both cases are known to be the same (or bot), there is no need to eval cond
    if (areConstantsEqual(ifTrue, ifFalse))
        return ifTrue;

    return TOP_PTR;
}

llvm::Constant* SCCP::propagateCast(llvm::CastInst* cast, LattHM* latt)
{
    llvm::Constant* e = latt->getConstant(cast->getOperand(0));
    if (!LattHM::isValidPtr(e))
        return e;

    uint64_t res;
    switch (cast->getOpcode())
    {
        case llvm::Instruction::Trunc:
        case llvm::Instruction::ZExt: res = e->getUniqueInteger().getZExtValue(); break;
        case llvm::Instruction::SExt: res = e->getUniqueInteger().getSExtValue(); break;

        case llvm::Instruction::BitCast:
        case llvm::Instruction::AddrSpaceCast:
        case llvm::Instruction::FPToUI:
        case llvm::Instruction::FPToSI:
        case llvm::Instruction::UIToFP:
        case llvm::Instruction::SIToFP:
        case llvm::Instruction::FPTrunc:
        case llvm::Instruction::FPExt:
        case llvm::Instruction::PtrToInt:
        case llvm::Instruction::IntToPtr:
        default: return TOP_PTR;
    }

    return llvm::ConstantInt::get(cast->getType(), res);
}

uint32_t SCCP::replaceByConstant(llvm::Function& fun, const LattHM* latt)
{
    std::vector<llvm::Instruction*> instToReplace;
    for (llvm::BasicBlock& bb : fun)
    {
        instToReplace.clear();
        instToReplace.reserve(bb.size());
        for (llvm::Instruction& inst : bb)
            instToReplace.push_back(&inst);

        for (llvm::Instruction* inst : instToReplace)
        {
            // skip if already constant
            if (llvm::dyn_cast<llvm::Constant>(inst))
                continue;

            // Return instructions cannot be replaced
            else if (llvm::dyn_cast<llvm::ReturnInst>(inst))
                continue;

            else if (auto* br = llvm::dyn_cast<llvm::BranchInst>(inst))
                replaceByUncodinitalBranch(br, latt);

            else
                replaceByConstantAssign(inst, latt);
        }
    }

    return 0;
}

void SCCP::replaceByUncodinitalBranch(llvm::BranchInst* br, const LattHM* latt)
{
    // we only care about conditional branches
    if (br->isUnconditional())
        return;

    llvm::Constant* cond = latt->getConstant(br->getCondition());

    // if undef, we decide where to go
    if (cond != TOP_PTR)
    {
        // for undef, we always assume "false" (as we can this way hopefully avoid if branches or
        // while loops)
        uint64_t choice = (cond != UNDEF_PTR) ? cond->getUniqueInteger().getZExtValue() : 0;
        llvm::BasicBlock* newsucc = br->getSuccessor(choice ? 0 : 1);
        llvm::BasicBlock* dead = br->getSuccessor(choice ? 1 : 0);

        // update (potential) phi
        dead->removePredecessor(br->getParent());

        llvm::IRBuilder<> builder(br);
#ifdef DEBUG_PRINTS
        llvm::BranchInst* uncondBr =
#endif
            builder.CreateBr(newsucc);
#ifdef DEBUG_PRINTS
        (std::cout << "replacing ").flush();
        br->print(llvm::outs());
        (std::cout << " by ").flush();
        uncondBr->print(llvm::outs());
        std::cout << std::endl;
#endif
        br->eraseFromParent();
    }
}

void SCCP::replaceByConstantAssign(llvm::Instruction* inst, const LattHM* latt)
{
    llvm::Constant* cons = latt->getConstant(inst);
    // if (cons == BOT_PTR)
    // const = m_Builder.

    // may not be top
    if (cons == TOP_PTR)
        return;

    // if bot, replace all occ by undef
    llvm::Constant* replacement =
        (cons == UNDEF_PTR) ? llvm::UndefValue::get(inst->getType()) : cons;

#ifdef DEBUG_PRINTS
    (std::cout << "replacing ").flush();
    inst->print(llvm::outs());
    (std::cout << " by ").flush();
    cons->print(llvm::outs());
    std::cout << std::endl;
#endif

    inst->replaceAllUsesWith(replacement);
    inst->eraseFromParent();
}

void collectReachableBlocks(llvm::Function& fun, llvm::DenseSet<llvm::BasicBlock*>& reachable)
{
    std::stack<llvm::BasicBlock*> stack;
    if (llvm::BasicBlock* entryBlock = &fun.getEntryBlock())
    {
        stack.push(entryBlock);
        reachable.insert(entryBlock);
    }

    // dfs
    while (!stack.empty())
    {
        llvm::BasicBlock* currentBlock = stack.top();
        stack.pop();

        for (llvm::BasicBlock* succ : successors(currentBlock))
        {
            // If the block was not already visited
            if (reachable.insert(succ).second)
                stack.push(succ);
        }
    }
}

bool removeUnreachableBlock(llvm::BasicBlock** btr)
{
    llvm::BasicBlock* blockToRemove = *btr;
    if (!blockToRemove)
        return false;

#ifdef DEBUG_PRINTS
    blockToRemove->print(llvm::outs() << "removing:");
#endif

    if (!blockToRemove->use_empty())
        return false;

    // update phi functions before removing
    for (auto* succ : llvm::successors(blockToRemove))
        succ->removePredecessor(blockToRemove);
    blockToRemove->eraseFromParent();
    *btr = nullptr;
    return true;
}

uint32_t SCCP::pruneUnreachableBlocks(llvm::Function& fun)
{
    // depth-first search through the ir untill all eachable blocks
    // have been visited, the remove all non-visited blocks
    llvm::DenseSet<llvm::BasicBlock*> reachable;
    collectReachableBlocks(fun, reachable);

    std::vector<llvm::BasicBlock*> blocks;
    blocks.reserve(fun.size());
    for (auto& bb : fun)
        if (!reachable.contains(&bb))
            blocks.push_back(&bb);

#ifdef DEBUG_PRINTS
    llvm::outs() << "cannot reach: \n\t";
    for (auto* bb : blocks)
    {
        bb->print(llvm::outs());
        llvm::outs() << "\n\t";
    }
#endif

    // remove all terminators, as we have already proven that the blocks are not reachable
    for (llvm::BasicBlock* bb : blocks)
    {
        if (auto* term = bb->getTerminator())
        {
            // llvm::errs() << "try to remove block " << *bb << '\n';
            //  remove us from all predecessors, trust me, segfaults are anoying to debug
            for (auto* succ : llvm::successors(bb))
                succ->removePredecessor(bb);

            term->eraseFromParent();
        }
    }

    // in theory O(n^2), but in practice O(n)
    size_t removedThisIt, removed = 0;
    do
    {
        removedThisIt = 0;
        for (auto it = blocks.rbegin(); it != blocks.rend(); it++)
            removedThisIt += removeUnreachableBlock(&*it) ? 1 : 0;
        removed += removedThisIt;
    } while (removedThisIt > 0);

    return removed;
}

/// @brief dead code elimination
/// @param fun
/// @return number of eliminated instructions
uint32_t SCCP::dce(llvm::Function& fun)
{
    uint32_t elim = 0, elimThisIteration;
    std::vector<llvm::Instruction*> toDelete;
    //  delete deads until we can no longer delete any dead instruction
    do
    {
        elimThisIteration = 0;
        for (llvm::BasicBlock& bb : fun)
        {
            toDelete.clear();
            for (llvm::Instruction& inst : bb)
                if (inst.isSafeToRemove() && !inst.mayHaveSideEffects() && inst.getNumUses() == 0)
                    toDelete.push_back(&inst);

            elimThisIteration += toDelete.size();
            for (llvm::Instruction* inst : toDelete)
                inst->eraseFromParent();
        }

        elim += elimThisIteration;
    } while (elimThisIteration != 0);
    return elim;
}
