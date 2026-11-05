#pragma once
#include "Children.h"
#include "Types.h"

#include <iostream>
#include <stdint.h>

constexpr node_t nullnode = 0;

enum class NodeKind : uint8_t
{
    UNDEFINED = 0,

    // Statements

    ASSIGN,             // Assignment: l = e;
    BLOCK,              // Block: { k1 x1; . . . km xm; p}
    IF,                 // If: if (e) s1
    IF_ELSE,            // If: if (e) s1 else s2
    WHILE,              // Loop: while (e) s
    DECLARATION,        // int type; int t = e;
    EXTERN_DECLARATION, // extern type; extern int/fun t = e;
    CONDITIONAL,        // cond ? e1 : e2;

    // Expressions

    VAR,            // Variable: x
    INDIR,          // Indirection: *e
    CONSTANT,       // Constant: c
    STRING_LITERAL, //
    ADDR,           // Address-Of: &l

    // BINARY, Binary Expr.: e1 o e2
    BINARY_ADD,
    BINARY_SUB,
    BINARY_MUL,
    BINARY_DIV,
    BINARY_REM,
    BINARY_AND,
    BINARY_OR,
    BINARY_XOR,
    BINARY_SHL,
    BINARY_SHR,
    BINARY_LOGICAL_AND,
    BINARY_LOGICAL_OR,
    BINARY_LOGICAL_EQ,
    BINARY_LOGICAL_NEQ,
    BINARY_LOGICAL_LT,
    BINARY_LOGICAL_LE,
    BINARY_LOGICAL_GT,
    BINARY_LOGICAL_GE,

    // only instantiated during sema
    POINTER_DIFF,
    POINTER_ADD_OFFSET,
    POINTER_SUB_OFFSET,

    // UNARY,  Unary Expr.: oe1
    UNARY_NOT,
    // UNARY_INC,
    // UNARY_DEC,
    UNARY_POS, // + in front of something
    UNARY_NEG,
    UNARY_BINARY_COMPLEMENT, // ~
    UNARY_SIZEOF,

    // other

    // FUNCTION_DECLARATION,
    FUNCTION,
    CALL_EXPRESSION,
    EXPR_STATEMENT,
    RETURN,
    CONTINUE,
    BREAK,
    ARRAY_ACCESS,
    GOTO,
    LABEL,
    CAST,
    STRUCT_DECL,
    EMPTY, // empty statement

    DOT_ACCESS,
    ARROW_ACCESS,

    TYPE_HOLDER,
};

constexpr bool isBinary(NodeKind nk)
{
    return (int)nk >= (int)NodeKind::BINARY_ADD && (int)nk <= (int)NodeKind::BINARY_XOR;
}

constexpr bool isUnary(NodeKind nk)
{
    return (unsigned)nk >= (unsigned)NodeKind::UNARY_NOT &&
           (unsigned)nk <= (unsigned)NodeKind::UNARY_SIZEOF;
}
