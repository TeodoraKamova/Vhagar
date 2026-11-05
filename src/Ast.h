#pragma once

#include "Children.h"
#include "Diagnostics.h"
#include "Locator.h"
#include "NodeUtils.h"
#include "SymbolTable.h"
#include "Token.h"
#include "Types.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

class Parser;

class Ast
{
private:
    friend class Parser;

    // garbage collector, also useful to 'recycle' often used types
    std::vector<const ::Type*> m_Att;

    std::vector<NodeKind> m_NodeKind;
    std::vector<children_t> m_Children;
    std::vector<const ::Type*> m_Types;
    std::vector<uint32_t> m_Offset;
    std::vector<uint32_t> m_ContentId;

    const Locator& m_Locator;
    const SymbolTable& m_SymbolTable;

    // node to that refers to a CONST[1]
    const node_t c_Const1Node;
    // const uint32_t m_const1Id;

    static constexpr node_t c_Origin = 0;
    uint32_t m_Uid;

public:
    Ast(const Locator& locator, SymbolTable& symbolTable);
    ~Ast(void);

    NodeKind getNodeKind(const node_t node) const { return m_NodeKind[node]; }
    void setNodeKind(const node_t node, NodeKind nk) { m_NodeKind[node] = nk; }
    const ::Type* getType(const node_t node) const { return m_Types[node]; }
    void setType(const node_t node, const ::Type* t) { m_Types[node] = t; }
    uint32_t getOffset(const node_t node) const { return m_Offset[node]; }
    symbol_t getContentId(const node_t node) const { return m_ContentId[node]; }

    const children_t& getChildren(const node_t node) const { return m_Children[node]; }
    children_t& getChildren(const node_t node) { return m_Children[node]; }

    /// <returns>the origin node the AST</returns>
    static constexpr node_t getOrigin(void) { return c_Origin; }
    // void setOrigin(node_t origin) { m_Origin = origin; }

    /// <returns>the number of nodes in the AST</returns>
    size_t size(void) const { return m_NodeKind.size(); }

    const Locator& getLocator(void) const { return m_Locator; }
    const SymbolTable& getSymbolTable(void) const { return m_SymbolTable; }

protected:
    // template <typename _Ty>
    inline node_t emplace_back(NodeKind nodeKind, children_t&& children, uint32_t offset,
                               const ::Type* type = nullptr, uint32_t contentId = 0)
    {
        m_NodeKind.emplace_back(nodeKind);
        m_Children.emplace_back(std::move(children));
        m_Types.emplace_back(type);
        m_Offset.emplace_back(offset);
        m_ContentId.emplace_back(contentId);

        return (node_t)(m_NodeKind.size() - 1);
    }

    //
    /// ------------------------- AST FACTORY -------------------------
    //

protected:
    inline node_t addExternalDeclaration(identifier_info varId, const ::Type* ty, node_t init,
                                         const bool isExtern)
    {
        const NodeKind nk = isExtern ? NodeKind::EXTERN_DECLARATION : NodeKind::DECLARATION;
        if (init == nullnode)
            return emplace_back(nk, none_child_t{}, varId.offset, ty, varId.identifier);
        return emplace_back(nk, unary_children_t{init}, varId.offset, ty, varId.identifier);
    }

    inline node_t addVar(uint32_t varId, uint32_t offset)
    {
        return emplace_back(NodeKind::VAR, none_child_t{}, offset, nullptr, varId);
    }

    inline node_t addAssignment(node_t lhs, node_t rhs, uint32_t offset)
    {
        return emplace_back(NodeKind::ASSIGN, binary_children_t{lhs, rhs}, offset);
    };

    inline node_t addBlock(nary_children_t&& block, uint32_t offset, uint32_t braceType)
    {
        return emplace_back(NodeKind::BLOCK, std::move(block), offset, nullptr, braceType);
    }

    inline node_t addBlock(children_t&& block, uint32_t offset, uint32_t braceType)
    {
        return emplace_back(NodeKind::BLOCK, std::move(block), offset, nullptr, braceType);
    }

    inline node_t addUnaryExpression(const Token& t, bool postfix, node_t child)
    {
        (void)postfix; // TODO: do we need this? And if so for what?
        NodeKind nk;
        switch (t.getType())
        {
            case TokenType::AND: nk = NodeKind::ADDR; break;
            case TokenType::ASTERISK: nk = NodeKind::INDIR; break;
            case TokenType::BANG: nk = NodeKind::UNARY_NOT; break;
            case TokenType::KW_SIZEOF: nk = NodeKind::UNARY_SIZEOF; break;
            case TokenType::MINUS: nk = NodeKind::UNARY_NEG; break;
            case TokenType::MINUS_MINUS:
                nk = NodeKind::BINARY_SUB;
                goto addUnaryExpression_HANDLE_SYNTACTIC_SUGAR;
            case TokenType::PLUS_PLUS:
                nk = NodeKind::BINARY_ADD;
                // goto addUnaryExpression_HANDLE_SYNTACTIC_SUGAR;
                {
                addUnaryExpression_HANDLE_SYNTACTIC_SUGAR:
                    const node_t combined = emplace_back(nk, binary_children_t{child, c_Const1Node},
                                                         t.getOffset(), getIntType());
                    return addAssignment(child, combined, t.getOffset());
                }
            case TokenType::PLUS: nk = NodeKind::UNARY_POS; break;
            case TokenType::TILDE: nk = NodeKind::UNARY_BINARY_COMPLEMENT; break;
            default:
                diagnostics::error("error, called addUnaryExpression with incompatible operand ",
                                   tokenToFormattedString(t, m_SymbolTable));
        }

        return emplace_back(nk, unary_children_t{child}, t.getOffset());
    }

    inline node_t addUnaryExpression(const Token& t, bool postfix, const ::Type* type)
    {
        (void)postfix;

        NodeKind nk;
        switch (t.getType())
        {
            case TokenType::KW_SIZEOF: nk = NodeKind::UNARY_SIZEOF; break;
            default:
                diagnostics::error("error, called addUnaryExpression(const Token&, bool, const "
                                   "::Type*) with incompatible operand ",
                                   tokenToFormattedString(t, m_SymbolTable));
        }

        return emplace_back(nk, none_child_t{}, t.getOffset(), type);
    }

    inline node_t addBinaryExpression(TokenType tty, node_t lhs, node_t rhs, uint32_t offset)
    {
        NodeKind nk;
        switch (tty)
        {
            case TokenType::AND: nk = NodeKind::BINARY_AND; break;
            case TokenType::AND_AND: nk = NodeKind::BINARY_LOGICAL_AND; break;
            case TokenType::ASTERISK: nk = NodeKind::BINARY_MUL; break;
            case TokenType::BANG_EQ: nk = NodeKind::BINARY_LOGICAL_NEQ; break;
            case TokenType::EQ: nk = NodeKind::ASSIGN; break;
            case TokenType::EQ_EQ: nk = NodeKind::BINARY_LOGICAL_EQ; break;
            case TokenType::GT: nk = NodeKind::BINARY_LOGICAL_GT; break;
            case TokenType::GT_EQ: nk = NodeKind::BINARY_LOGICAL_GE; break;
            case TokenType::LT: nk = NodeKind::BINARY_LOGICAL_LT; break;
            case TokenType::LT_EQ: nk = NodeKind::BINARY_LOGICAL_LE; break;
            case TokenType::MINUS: nk = NodeKind::BINARY_SUB; break;
            case TokenType::OR: nk = NodeKind::BINARY_OR; break;
            case TokenType::OR_OR: nk = NodeKind::BINARY_LOGICAL_OR; break;
            case TokenType::PLUS: nk = NodeKind::BINARY_ADD; break;
            case TokenType::SLASH: nk = NodeKind::BINARY_DIV; break;
            case TokenType::PERCENT: nk = NodeKind::BINARY_REM; break;
            case TokenType::LBRACKET: nk = NodeKind::ARRAY_ACCESS; break;
            case TokenType::HAT: nk = NodeKind::BINARY_XOR; break;
            case TokenType::LT_LT: nk = NodeKind::BINARY_SHL; break;
            case TokenType::GT_GT: nk = NodeKind::BINARY_SHR; break;
            case TokenType::AND_EQ:
                nk = NodeKind::BINARY_AND;
                goto addBinaryExpression_HANDLE_SYNTACTIC_SUGAR;
            case TokenType::OR_EQ:
                nk = NodeKind::BINARY_OR;
                goto addBinaryExpression_HANDLE_SYNTACTIC_SUGAR;
            case TokenType::XOR_EQ:
                nk = NodeKind::BINARY_XOR;
                goto addBinaryExpression_HANDLE_SYNTACTIC_SUGAR;
            case TokenType::PLUS_EQ:
                nk = NodeKind::BINARY_ADD;
                goto addBinaryExpression_HANDLE_SYNTACTIC_SUGAR;
            case TokenType::MINUS_EQ:
                nk = NodeKind::BINARY_SUB;
                goto addBinaryExpression_HANDLE_SYNTACTIC_SUGAR;
            case TokenType::TIMES_EQ:
                nk = NodeKind::BINARY_MUL;
                goto addBinaryExpression_HANDLE_SYNTACTIC_SUGAR;
            case TokenType::SHL_EQ:
                nk = NodeKind::BINARY_SHL;
                goto addBinaryExpression_HANDLE_SYNTACTIC_SUGAR;
            case TokenType::SHR_EQ:
                nk = NodeKind::BINARY_SHR;
                goto addBinaryExpression_HANDLE_SYNTACTIC_SUGAR;
            case TokenType::MOD_EQ:
                nk = NodeKind::BINARY_REM;
                goto addBinaryExpression_HANDLE_SYNTACTIC_SUGAR;
            case TokenType::DIV_EQ:
                nk = NodeKind::BINARY_DIV;
                {
                addBinaryExpression_HANDLE_SYNTACTIC_SUGAR:
                    // update rhs to match syntactic sugar for += *= etc.
                    rhs = emplace_back(nk, binary_children_t{lhs, rhs}, offset);
                    nk = NodeKind::ASSIGN;
                    break;
                }
            default:
                diagnostics::error("error, called addBinaryExpression with incompatible operand ",
                                   tokenToFormattedString(Token(tty, 0, 0), m_SymbolTable));
        }

        return emplace_back(nk, binary_children_t{lhs, rhs}, offset);
    }

    inline node_t addPrimaryExpression(const Token& token)
    {
        NodeKind nk;
        switch (token.getType())
        {
            // TODO: CHARACTER?
            case TokenType::IDENTIFIER: nk = NodeKind::VAR; break;
            case TokenType::STRING_LITERAL: nk = NodeKind::STRING_LITERAL; break;
            case TokenType::CONSTANT:
            {
                const std::string_view view = *m_SymbolTable[token.getContentKey()];
                // is it a char-constant
                if (view[0] == '\'' && view.back() == '\'')
                    ; // TODO: do we need to check anything here?
                // not a char constant -> need to check if it is a valid int
                else
                {
                    size_t pos;
                    try
                    {
                        (void)std::stoi(std::string(view), &pos, 0);
                    }
                    catch (const std::exception& exc)
                    {
                        diagnostics::errorloc(m_Locator.getLocationByOffset(token.getOffset()),
                                              "parser error: bad constant: ", exc.what(),
                                              " in token",
                                              tokenToFormattedString(token, m_SymbolTable));
                    }

                    if (pos != view.length())
                    {
                        diagnostics::errorloc(m_Locator.getLocationByOffset(token.getOffset()),
                                              "parser error: bad constant"
                                              " in token",
                                              tokenToFormattedString(token, m_SymbolTable));
                    }
                }

                nk = NodeKind::CONSTANT;
                break;
            }

            default:
                diagnostics::error("error, called addPrimaryExpression with incompatible operand ",
                                   tokenToFormattedString(token, m_SymbolTable));
        }

        // TODO: when i parse, should i already convert integer constants to integers?
        // In that case, we would intemperate the content key as the constant, and not the lookup
        // key.
        return emplace_back(nk, none_child_t{}, token.getOffset(), nullptr, token.getContentKey());
    }

    inline node_t addFunctionDefinition(const ::Type* type, identifier_info name, node_t body)
    {
        return emplace_back(NodeKind::FUNCTION, unary_children_t{body}, name.offset, type,
                            name.identifier);
    }

    inline node_t addDeclatationStatement(const ::Type* type, identifier_info name, node_t init)
    {
        const NodeKind nk = NodeKind::DECLARATION;
        if (init == nullnode)
            return emplace_back(nk, none_child_t{}, name.offset, type, name.identifier);
        return emplace_back(nk, unary_children_t{init}, name.offset, type, name.identifier);
    }

    inline node_t addCallExpression(node_t callee, nary_children_t&& args, uint32_t offset)
    {
        args.push_back(callee);
        return emplace_back(NodeKind::CALL_EXPRESSION, std::move(args), offset);
    }

    inline node_t addReturnStatement(uint32_t offset, node_t expr)
    {
        if (expr != nullnode)
            return emplace_back(NodeKind::RETURN, unary_children_t{expr}, offset);
        return emplace_back(NodeKind::RETURN, none_child_t{}, offset);
    }

    inline node_t addLocatable(uint32_t offset, NodeKind nk)
    {
        return emplace_back(nk, none_child_t{}, offset);
    }

    inline node_t addIfStatement(node_t cond, node_t ifTrue, uint32_t offs)
    {
        return emplace_back(NodeKind::IF, binary_children_t{cond, ifTrue}, offs);
    }

    inline node_t addIfElseStatement(node_t cond, node_t ifTrue, node_t ifFalse, uint32_t offs)
    {
        return emplace_back(NodeKind::IF_ELSE, ternary_children_t{cond, ifTrue, ifFalse}, offs);
    }

    inline node_t addConditionalExpression(node_t expr, node_t ifTrue, node_t ifFalse,
                                           uint32_t offs)
    {
        return emplace_back(NodeKind::CONDITIONAL, ternary_children_t{expr, ifTrue, ifFalse}, offs);
    }

    inline node_t addExpressionStatement(node_t expr, uint32_t offs)
    {
        return emplace_back(NodeKind::EXPR_STATEMENT, unary_children_t{expr}, offs);
    }

    inline node_t addWhileStatement(node_t cond, node_t body, uint32_t offs)
    {
        return emplace_back(NodeKind::WHILE, binary_children_t{cond, body}, offs);
    }

    inline node_t addForStatement(node_t init, node_t cond, node_t post, node_t body, uint32_t offs)
    {
        nary_children_t pseudoBlock;
        pseudoBlock.push_back(init);

        // if children are not already a block, we need to make it
        if (getNodeKind(body) != NodeKind::BLOCK)
        {
            nary_children_t children;

            children.push_back(body);
            children.push_back(post);

            const node_t newBlock =
                addBlock(std::move(children), getOffset(body), getContentId(body));

            body = newBlock;
        }
        else
        {
            children_t& whileChildren = getChildren(body);

            whileChildren.push_back(post);
        }

        pseudoBlock.push_back(addWhileStatement(cond, body, offs));
        return addBlock(std::move(pseudoBlock), offs, 0);
    }

    inline node_t addGotoStatement(node_t identifier, uint32_t offs)
    {
        return emplace_back(NodeKind::GOTO, none_child_t{}, offs, nullptr, identifier);
    }

    inline node_t addEmptyStatement(uint32_t offs)
    {
        return emplace_back(NodeKind::EMPTY, none_child_t{}, offs);
    }

    inline node_t addLabel(uint32_t label, node_t expr, uint32_t offs)
    {
        return emplace_back(NodeKind::LABEL, unary_children_t{expr}, offs, nullptr, label);
    }

    inline node_t addCastExpression(const ::Type* type, node_t operand, uint32_t offs)
    {
        return emplace_back(NodeKind::CAST, unary_children_t{operand}, offs, type);
    }

    inline node_t addStructDeclaration(const ::Type* type, uint32_t offs)
    {
        return emplace_back(NodeKind::STRUCT_DECL, none_child_t{}, offs, type);
    }

    inline node_t addDoWhile(node_t cond, node_t body, uint32_t offs)
    {
        // interpreted as
        /*
        goto doWhileBody;
        while(cond)
        {
        doWhileBody: ;
            body;
        }
        */

        uint32_t identifier = m_Uid++;
        const node_t gotoStmt = addGotoStatement(identifier, 0);
        const node_t label = addLabel(identifier, addEmptyStatement(0), 0);
        const node_t newBody = addBlock(binary_children_t{label, body}, offs, 0);
        const node_t whileStmt = addWhileStatement(cond, newBody, offs);

        return addBlock(binary_children_t{gotoStmt, whileStmt}, offs, 0);
    }

    inline node_t addMemberAccess(node_t expr, node_t field, uint32_t offs, TokenType tty)
    {
        switch (tty)
        {
            case TokenType::DOT:
                return emplace_back(NodeKind::DOT_ACCESS, unary_children_t{expr}, offs, nullptr,
                                    field);
            case TokenType::ARROW:
                return emplace_back(NodeKind::ARROW_ACCESS, unary_children_t{expr}, offs, nullptr,
                                    field);
            default:
                diagnostics::error("called addMemberAccess with incorrect token type ",
                                   tokenToFormattedString(Token(tty, 0, 0), m_SymbolTable));
        }
    }

    //
    /// ------------------------- ATT FACTORY -------------------------
    //
public:
    const ::Type* getVoidType(void) const { return m_Att[0]; }
    const ::Type* getIntType(void) const { return m_Att[1]; }
    const ::Type* getUIntType(void) const { return m_Att[2]; }
    const ::Type* getCharType(void) const { return m_Att[3]; }
    const ::Type* getUCharType(void) const { return m_Att[4]; }

    /// does not hand over ownership. Object will be destroyed when AST dies
    inline ::PointerType* createPointerType(const ::Type* pointsTo)
    {
        ::PointerType* ty = new ::PointerType(pointsTo);
        m_Att.emplace_back(ty);
        return ty;
    }

    /// does not hand over ownership. Object will be destroyed when AST dies
    inline ::FunctionType* createFunctionType(const ::Type* returnType, nary_members_t&& params)
    {
        ::FunctionType* ty = new ::FunctionType(returnType, std::move(params));
        m_Att.emplace_back(ty);
        return ty;
    }

    /// does not hand over ownership. Object will be destroyed when AST dies
    inline ::NamedType* createNamedType(uint32_t name)
    {
        ::NamedType* ty = new ::NamedType(name);
        m_Att.emplace_back(ty);
        return ty;
    }

    inline ::StructType* createStructType(uint32_t name, nary_members_t&& members)
    {
        ::StructType* ty = new ::StructType(name, std::move(members));
        m_Att.emplace_back(ty);
        return ty;
    }
};

std::ostream& operator<<(std::ostream& os, const Ast& ast);
