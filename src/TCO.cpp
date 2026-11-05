#include "TCO.h"

void TCO::run(void)
{
    for (llvm::Function& fun : m_Module)
        // skip declarations
        if (!fun.isDeclaration())
            handleFun(fun);
}

void TCO::handleFun(llvm::Function& fun)
{
    for (llvm::BasicBlock& block : fun)
        if (llvm::Instruction* terminator = block.getTerminator())
            if (llvm::ReturnInst* ret = llvm::dyn_cast<llvm::ReturnInst>(terminator))
                handleTerminator(ret);
}

// inline bool canMustTailCall(llvm::Module& mod, llvm::CallInst* call)
// {
//
//     llvm::Function* calledFunction = call->getCalledFunction();
//     if (!calledFunction)
//         return false;
//
//     const auto exists = mod.getFunction(calledFunction->getName());
//     const auto wasDefinedHere = exists && !exists->isDeclaration();
//     if (!wasDefinedHere)
//         return false;
//     return true;
//     we also need to verify that the number of arguments don't mismatch
//
//         auto* bb = call->getParent();
//     if (!bb)
//         return false;
//     auto* fun = bb->getParent();
//     if (!fun)
//         return false;
//
//     const auto numUs = typeUs->getPointsTo()->getFunctionNumParams();
//
//     auto* typeThem = calledFunction->getType();
//     if (!typeThem)
//         return false;
//     const auto numThem = typeThem->getFunctionNumParams();
//
//     return numUs == numThem;
// }

void TCO::handleTerminator(llvm::ReturnInst* inst)
{
    // if re return a call, then set call to tailcall. Otherwise if we
    // return the result of a call that just happend exactly one line before,
    // set this call to tailcall.

    const auto numOperands = inst->getNumOperands();
    const bool mustRetVoid = numOperands == 0;

    if (numOperands > 1)
        return;

    if (mustRetVoid)
    {
        auto* prevInst = inst->getPrevNode();
        if (!prevInst)
            return;

        if (auto* call = llvm::dyn_cast<llvm::CallInst>(prevInst))
        {

            const auto mustTailCall = false; // canMustTailCall(m_Module, call);

            // no matter what we get as a result, since we return void anyway, we don't care
            if (call->getType()->isVoidTy()) // but better to be save
                call->setTailCallKind(mustTailCall ? llvm::CallInst::TailCallKind::TCK_MustTail
                                                   : llvm::CallInst::TailCallKind::TCK_Tail);
        }
        return;
    }

    llvm::Value* termOp = inst->getOperand(0);
    if (auto* call = llvm::dyn_cast<llvm::CallInst>(termOp))
    {
        const auto mustTailCall = false; // canMustTailCall(m_Module, call);
        call->setTailCallKind(mustTailCall ? llvm::CallInst::TailCallKind::TCK_MustTail
                                           : llvm::CallInst::TailCallKind::TCK_Tail);
    }
    else
    {
        // try to get predecessors:
        auto* prevInst = inst->getPrevNode();
        if (!prevInst)
            return;

        if (auto* call = llvm::dyn_cast<llvm::CallInst>(prevInst))
        {
            // us our result used?
            // if the function was defined in the same module, we can fore a tailcall

            const auto mustTailCall = false; // canMustTailCall(m_Module, call);

            if (termOp == call)
                call->setTailCallKind(mustTailCall ? llvm::CallInst::TailCallKind::TCK_MustTail
                                                   : llvm::CallInst::TailCallKind::TCK_Tail);
        }
    }
}
