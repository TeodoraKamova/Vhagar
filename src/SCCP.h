#pragma once
#include "Backend.h"

struct LattHM;

class SCCP
{
private:
    llvm::Module& m_Module;
    llvm::LLVMContext& m_Context;
    llvm::IRBuilder<>& m_Builder;

public:
    SCCP(Backend& be)
        : m_Module(be.getModule()), m_Context(be.getContext()), m_Builder(be.getBuilder())
    {
    }

    void run(void);

private:
    bool sccpFun(llvm::Function& fun);
    uint32_t replaceByConstant(llvm::Function& fun, const LattHM* latt);
    uint32_t pruneUnreachableBlocks(llvm::Function& fun);
    uint32_t dce(llvm::Function& fun);

    llvm::Constant* propagateConstant(llvm::Instruction* inst, LattHM* latt);

    /// ------------------------ CONSTANT PROPAGATION ------------------------ ///

    /// @brief Tries to constantly evaluate a function based on procided HM
    /// @param inst to be evaluated
    /// @param latt to take the constant values off from
    /// @return a llvm::Constant* if the const value if `inst` could be evaluated, null if TOP, ? if
    /// bot???
    llvm::Constant* propagateInstr(llvm::Instruction* inst, LattHM* latt);

    static llvm::Constant* propagateBinaryOperator(llvm::BinaryOperator* binOp, LattHM* latt);

    static llvm::Constant* propagateStoreInst(void);

    llvm::Constant* propagateICmpInst(llvm::ICmpInst* icmpInst, LattHM* latt);

    llvm::Constant* propagatePhiNode(llvm::PHINode* phi, LattHM* latt);

    llvm::Constant* propagateRet(llvm::ReturnInst* ret, LattHM* latt);

    llvm::Constant* propagateBranch(llvm::BranchInst* br, LattHM* latt);

    llvm::Constant* propagateCall(void);

    llvm::Constant* propagateSelect(llvm::SelectInst* sel, LattHM* latt);

    llvm::Constant* propagateCast(llvm::CastInst* cast, LattHM* latt);

    ///

    void replaceByUncodinitalBranch(llvm::BranchInst* br, const LattHM* latt);

    void replaceByConstantAssign(llvm::Instruction* inst, const LattHM* latt);

    /// ------------------------ LIVENESS ANALYIS ------------------------ ///
};
