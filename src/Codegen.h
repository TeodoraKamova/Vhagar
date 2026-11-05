#pragma once

#include "Ast.h"
#include "Backend.h"
#include "llvm/ADT/SmallVector.h"
#include <utility>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#pragma GCC diagnostic pop

class Codegen
{
    using LLVMContext = llvm::LLVMContext;
    using Builder = llvm::IRBuilder<>;
    using Value = llvm::Value;
    using Instruction = llvm::Instruction;
    using BB = llvm::BasicBlock;
    using BasicBlock = llvm::BasicBlock;
    using Function = llvm::Function;
    using GlobalVariable = llvm::GlobalVariable;
    template <typename T> using Vec = llvm::SmallVector<T>;
    template <typename K, typename V> using Map = llvm::SmallDenseMap<K, V>;

private:
    const Ast& m_Ast;
    const SymbolTable& m_SymbolTable;
    const Locator& m_Locator;

    Backend& m_Backend;
    Builder& m_Builder;

    Function* m_Function;

    // module
    Map<symbol_t, GlobalVariable*> m_Globals;
    Map<symbol_t, Function*> m_Functions;
    Map<symbol_t, llvm::StructType*> m_Structs;

    // function
    Map<symbol_t, BasicBlock*> m_Labels;
    Map<symbol_t, Value*> m_Locals;
    Vec<std::pair<BB*, BB*>> m_Loops;

public:
    Codegen(Backend& backend, const Ast& ast, const Locator& locator);

    void emitToplevel();

private:
    // utils

    llvm::Type* convertType(const ::Type* type);
    Value* implicitCast(Value* value, llvm::Type* desiredType);
    BasicBlock* createBasicBlock(const std::string& name);

    void registerFunction(std::pair<symbol_t, Function*> pair);
    Function* lookupFunction(symbol_t symbol);

    void registerGlobalVariable(std::pair<symbol_t, GlobalVariable*> pair);
    GlobalVariable* lookupGlobalVariable(symbol_t symbol);

    void registerLocalVariable(std::pair<symbol_t, Value*> pair);
    Value* lookupLocalVariable(symbol_t symbol);

    void registerLabel(std::pair<symbol_t, BasicBlock*> pair);
    BasicBlock* lookupLabel(symbol_t symbol);

    // codegen

    void emitFunction(node_t node);
    void emitExternFunctionDeclaration(node_t node);
    void emitGlobalDeclaration(node_t node);
    void emitStructDeclaration(node_t node);
    void emitExternVariableDeclaration(node_t node);

    Value* emitExprR(node_t node);
    Value* emitExprR(node_t node, llvm::Type* desiredType);
    Value* emitConstantR(node_t node);
    Value* emitVariableR(node_t node);
    Value* emitCallR(node_t node);
    Value* emitAssignR(node_t node);
    Value* emitArrayAccessR(node_t node);
    Value* emitCastR(node_t node);
    Value* emitAddrR(node_t node);
    Value* emitIndirR(node_t node);
    Value* emitBinaryOpR(node_t node, Instruction::BinaryOps binop);
    Value* emitCompareOpR(node_t node, llvm::CmpInst::Predicate binop);
    Value* emitUnaryNotR(node_t node);
    Value* emitUnaryNegR(node_t node);
    Value* emitUnaryPosR(node_t node);
    Value* emitUnaryComplementR(node_t node);
    Value* emitPointerDiffR(node_t node);
    Value* emitPointerAddOffsetR(node_t node);
    Value* emitPointerSubOffsetR(node_t node);
    Value* emitTernaryR(node_t node);
    Value* emitStringR(node_t node);
    Value* emitDotAccessR(node_t node);
    Value* emitArrowAccessR(node_t node);
    Value* emitSizeofR(node_t node);
    Value* emitLogicalR(node_t node);

    Value* emitExprL(node_t node);
    Value* emitVariableL(node_t node);
    Value* emitArrayAccessL(node_t node);
    Value* emitIndirL(node_t node);
    Value* emitDotAccessL(node_t node);
    Value* emitArrowAccessL(node_t node);
    Value* emitCallL(node_t node);

    void emitCF(node_t node, BB& bbt, BB& bbf);
    void emitExprCF(node_t node, BB& bbt, BB& bbf);
    void emitCmpCF(node_t node, BB& bbt, BB& bbf, llvm::CmpInst::Predicate op);
    void emitNotCF(node_t node, BB& bbt, BB& bbf);
    void emitIfElseCF(node_t node, BB& bbt, BB& bbf);
    void emitIfCF(node_t node, BB& bbt, BB& bbf);
    void emitWhileCF(node_t node, BB& bbt, BB& bbf);
    void emitLogicalAndCF(node_t node, BB& bbt, BB& bbf);
    void emitLogicalOrCF(node_t node, BB& bbt, BB& bbf);

    void emitStmt(node_t node);
    void emitBlock(node_t node);
    void emitReturn(node_t node);
    void emitLocalDeclaration(node_t node);
    void emitGoto(node_t node);
    void emitLabel(node_t node);
    void emitContinue(node_t node);
    void emitBreak(node_t node);
};
