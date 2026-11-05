#pragma once
#include "Ast.h"
#include "Token.h"

class Parser
{
private:
    const std::vector<Token>& m_Tokens;
    const Locator& m_Locator;
    const SymbolTable& m_SymbolTable;

    Ast m_Ast;
    size_t m_Index;

public:
    Parser(const std::vector<Token>& tokens, const Locator& locator, SymbolTable& symbolTable)
        : m_Tokens(tokens), m_Locator(locator), m_SymbolTable(symbolTable),
          m_Ast(locator, symbolTable), m_Index(0)
    {
    }

    Ast& parse(void);
    const Ast& getAst(void) const { return m_Ast; }

private:
    TokenType peekNextTokenType(size_t depth) const;
    void expectToken(size_t depth, TokenType expectedType) const;

    const Token& nextToken(void) { return m_Tokens[++m_Index]; }
    const Token& currentToken(void) const { return m_Tokens[m_Index]; }

    node_t parseExternalDeclaration(void);
    node_t parseFunctionDeclaration(const Type* type, const bool isExtern);
    identifier_info parseIdentifier(const Type*& type, const bool mayBeAnonymous = false);
    void parseAndMakeToFunction(const Type*& type);
    void parseFunctionArguments(nary_members_t& arguments);
    node_t parseBlock(void);
    node_t parseBreak(void);
    node_t parseGoto(void);
    node_t parseEmptyStatement(void);
    node_t parseContinue(void);
    node_t parseDeclaration(bool allowObjDecl);
    node_t parseLabel(void);
    node_t parseOperand(void);
    node_t parseExpression(const Precedence precedence = Precedence::EXPRESSION);
    node_t parseArrayAccess(node_t expr, uint32_t offs);
    node_t parseFunctionCall(node_t expr, uint32_t offs);
    node_t parseMemberAccess(node_t expr, uint32_t offs, TokenType tty);
    node_t parseIf(void);
    node_t parseReturn(void);
    node_t parseWhile(void);
    node_t parseDoWhile(void);
    node_t parseFor(void);
    node_t parseStatement(bool isInner = false);
    node_t parseTernaryExpression(node_t expr, uint32_t offs);
    node_t parseExpressionStatement(void);

    // helper
    node_t parseSimple(TokenType tty, NodeKind nk);

    const Type* parseType(void);
    const Type* parseStructType(uint32_t offs);
};
