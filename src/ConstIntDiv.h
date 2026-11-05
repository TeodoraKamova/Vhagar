#pragma once
#include "Backend.h"

class ConstIntDiv
{
private:
    llvm::Module& m_Module;
    llvm::LLVMContext& m_Context;
    llvm::IRBuilder<>& m_Builder;

public:
    ConstIntDiv(Backend& be)
        : m_Module(be.getModule()), m_Context(be.getContext()), m_Builder(be.getBuilder())
    {
    }

    void run(void);

public:
    void handleInstruction(llvm::BinaryOperator* instr);
    void handleSDiv(llvm::BinaryOperator* instr);
    void handleSRem(llvm::BinaryOperator* instr);
    void handleSMul(llvm::BinaryOperator* instr);
};
