#pragma once

#include "Ast.h"

#include <stack>
#include <unordered_map>
#include <unordered_set>
// #include "Types.h"

// // Forward declarations
// class Type;
// class Ast;
// class SymbolTable;

using Scope = std::unordered_map<uint32_t, const Type*>;
using Env = std::stack<Scope>;
using LabScope = std::unordered_set<uint32_t>;
using LabEnv = std::stack<LabScope>;

class Context
{
public:
    Env vars;
    Env functions;
    Env functionDecls;
    Env structs;
    LabEnv labelsUsed;
    bool insideLoop = false;

    Context(Env vars, Env functions, Env functionDecls, Env structs, LabEnv labelsUsed)
        : vars(vars), functions(functions), functionDecls(functionDecls), structs(structs),
          labelsUsed(labelsUsed)
    {
    }
};

class SemanticAnalysis
{
private:
    Ast& m_Ast;
    std::unordered_map<uint32_t, uint32_t> labelsSeen;
    Locator m_Locator;

public:
    SemanticAnalysis(Ast& ast, Locator locator) : m_Ast(ast), m_Locator(locator) {}

    void validate();

private:
    void typeCheckStatement(node_t node, Env& vars, Env& functions, Env& functionDecls,
                            Env& structs, LabEnv& labelsUsed, const Type* ftype = 0,
                            bool insideLoop = false, Scope funcArg = {}, bool global = false);

    const Type* typeCheckExpression(node_t node, Context& ctx, bool func = false);

    const Type* checkBinarySub(node_t node, Context& ctx, bool func);
    const Type* checkBinaryLessGreat(node_t node, Context& ctx, bool func);
    const Type* checkBinaryEqNeq(node_t node, Context& ctx, bool func);
    const Type* checkBinaryAdd(node_t node, Context& ctx, bool func);
    const Type* checkBinaryMul(node_t node, Context& ctx, bool func);
    const Type* checkBinaryXor(node_t node, Context& ctx, bool func);
    const Type* checkBinaryAnd(node_t node, Context& ctx, bool func);
    const Type* checkConditional(node_t node, Context& ctx, bool func);
    const Type* checkArrayAccess(node_t node, Context& ctx, bool func);
    const Type* checkCallExpression(node_t node, Context& ctx);
    const Type* checkDotAccess(node_t node, Context& ctx, bool func);
    const Type* checkArrowAccess(node_t node, Context& ctx, bool func);
    const Type* checkAddrExpression(node_t node, Context& ctx, bool func);
    const Type* checkIndirExpression(node_t node, Context& ctx, bool func);
    const Type* checkUnaryNeg(node_t node, Context& ctx, bool func);
    const Type* checkUnaryComplement(node_t node, Context& ctx, bool func);
    const Type* checkUnaryNot(node_t node, Context& ctx, bool func);
    const Type* checkUnarySizeof(node_t node, Context& ctx, bool func);
    const Type* checkCastExpression(node_t node, Context& ctx, bool func);
    const Type* checkConstant(node_t node, const SymbolTable& m_Symbol_Table);
    const Type* checkStringLit(node_t node);
    const Type* checkVar(node_t node, Context& ctx, bool func);
    const Type* checkAssign(node_t node, Context& ctx, bool func);
};
