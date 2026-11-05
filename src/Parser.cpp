
// TODO: extern keyword

#include "Parser.h"
#include "Diagnostics.h"
#include "Types.h"
#include <cassert>
#include <cstdio>

#define PARSER_ERROR "parser error: "

Ast& Parser::parse(void)
{
    // here, we parse the while file.
    // So we are only interested in global variables and
    // function declaration/definitions

    if (currentToken().getType() == TokenType::END_OF_INPUT)
        diagnostics::errorloc(m_Locator.getLocationByOffset(currentToken().getOffset()),
                              PARSER_ERROR "we don't support empty files!");

    const node_t origin = m_Ast.getOrigin();
    while (m_Index < m_Tokens.size())
    {
        if (currentToken().getType() == TokenType::END_OF_INPUT)
            break;

        const node_t decl = parseExternalDeclaration();
        std::get<nary_children_t>(m_Ast.getChildren(origin)).push_back(decl);
    }

    return m_Ast;
}

/// <param name="depth">depth of the lookahead, default is 1</param>
/// <returns>the token type of the next token. If there is no next token returns
/// END_OF_INPUT</returns>
TokenType Parser::peekNextTokenType(size_t depth) const
{
    if (m_Index + 1 < m_Tokens.size())
        return m_Tokens[m_Index + depth].getType();
    return TokenType::END_OF_INPUT;
}

/// asserts that the token at index
void Parser::expectToken(size_t depth, TokenType expectedType) const
{
    const Token& token = m_Tokens[m_Index + depth];
    const TokenType tty = token.getType();
    if (tty != expectedType)
    {

        if (tty == TokenType::END_OF_INPUT)
            diagnostics::errorloc(m_Locator.getLocationByOffset(m_Tokens.back().getOffset()),
                                  PARSER_ERROR "expected",
                                  tokenToFormattedString(Token(expectedType, 0, 0), m_SymbolTable),
                                  "but no token found.");

        const Token t = m_Tokens[m_Index + depth];
        diagnostics::errorloc(m_Locator.getLocationByOffset(t.getOffset()),
                              PARSER_ERROR "unexpected",
                              tokenToFormattedString(token, m_SymbolTable), ", expected ",
                              tokenToFormattedString(Token(expectedType, 0, 0), m_SymbolTable));
    }
}

node_t Parser::parseExternalDeclaration(void)
{
    const bool isExtern = currentToken().getType() == TokenType::KW_EXTERN;
    if (isExtern)
        nextToken();

    const bool isStruct = currentToken().getType() == TokenType::KW_STRUCT;
    uint32_t offset = currentToken().getOffset();

    const Type* type = parseType();

    if (isStruct && peekNextTokenType(0) == TokenType::SEMICOLON)
    {
        if (isExtern)
            diagnostics::errorloc(m_Locator.getLocationByOffset(offset),
                                  PARSER_ERROR "cannot have extern struct declarations");
        nextToken();
        return m_Ast.addStructDeclaration(type, offset); // TODO: correct offset?
    }

    return parseFunctionDeclaration(type, isExtern);
}

node_t Parser::parseFunctionDeclaration(const Type* type, const bool isExtern)
{
    const identifier_info varId = parseIdentifier(type);
    const Token& token = currentToken();
    const TokenType tty = token.getType();
    node_t init = nullnode;

    if (typeid(*type) == typeid(const FunctionType))
    {
        // const Token& token = nextToken();
        // const TokenType ctty = token.getType();
        switch (tty)
        {
            case TokenType::LBRACE:
            {
                const node_t body = parseBlock();
                return m_Ast.addFunctionDefinition(type, varId, body);
            }
            case TokenType::SEMICOLON:
            {
                nextToken();
                break;
            }
            default:
            {
                diagnostics::errorloc(m_Locator.getLocationByOffset(token.getOffset()),
                                      PARSER_ERROR "expected '{' or ';', but got",
                                      tokenToFormattedString(token, m_SymbolTable));
            }
        }
    }
    else
    {
        switch (tty)
        {

            case TokenType::EQ:
            {
                // expectClosingParens();
                nextToken();
                init = parseExpression();
            }
                [[fallthrough]];
            case TokenType::SEMICOLON:
            {
                // expectClosingParens();
                expectToken(0, TokenType::SEMICOLON);
                nextToken();
                break;
            }
            default:
            {
                diagnostics::errorloc(m_Locator.getLocationByOffset(token.getOffset()),
                                      PARSER_ERROR "expected LPAREN, EQ, or SEMICOLON!");
            }
        }
    }
    return m_Ast.addExternalDeclaration(varId, type, init, isExtern);
}

void Parser::parseFunctionArguments(nary_members_t& arguments)
{
    nextToken();

    // no arguments
    const TokenType nextTty = peekNextTokenType(0);
    if (nextTty == TokenType::KW_VOID && peekNextTokenType(1) == TokenType::RPAREN)
        nextToken();
    else if (nextTty != TokenType::RPAREN)
    {
        while (true)
        {
            const Type* pType = parseType();

            const Token ct = currentToken();
            const TokenType ctty = ct.getType();
            const identifier_info identifier =
                (ctty != TokenType::COMMA && ctty != TokenType::RPAREN)
                    ? parseIdentifier(pType, true)
                    : identifier_info{nullnode, false, ct.getOffset()};

            arguments.push_back(MemberField(pType, identifier));

            if (currentToken().getType() != TokenType::COMMA)
                break;

            nextToken();
        }
    }
    expectToken(0, TokenType::RPAREN);
    // type = m_Ast.createFunctionType(type, std::move(arguments));
}

void Parser::parseAndMakeToFunction(const Type*& type)
{
    const Type* base = type;
    size_t numPointers = 0;
    for (; typeid(*base) == typeid(const PointerType); numPointers++)
        base = static_cast<const PointerType&>(*base).getPointsTo();

    nary_members_t members;
    parseFunctionArguments(members);
    type = m_Ast.createFunctionType(base, std::move(members));

    for (; numPointers != 0; numPointers--)
        type = m_Ast.createPointerType(type);
    nextToken();
}

// helper function that when given a FunctionType returns the
// return type, and otherwise returns its inout
const Type* getChildTypeOrLast(std::vector<Type*>& types)
{
    Type* type = types.back();
    if (typeid(*type) == typeid(FunctionType))
        return static_cast<const FunctionType*>(type)->getReturnType();
    else if (types.size() > 1 && typeid(*type) == typeid(PointerType))
        return static_cast<const PointerType*>(type)->getPointsTo();
    return type;
}

// helper function to set the new return type to the target iff the target is of FunctionType,
// otherwise it sets target_ref to newReturnType
void setReturnTypeOrNone(std::vector<Type*>& types, PointerType* newReturnType)
{
    Type* target = types.back();
    if (typeid(*target) == typeid(FunctionType))
        static_cast<FunctionType*>(target)->setNewReturnType(newReturnType);
    else if (types.size() > 1 && typeid(*target) == typeid(PointerType))
        static_cast<PointerType*>(target)->setPointsTo(newReturnType);
    else
        types.push_back(newReturnType);
}

identifier_info Parser::parseIdentifier(const Type*& type, const bool mayBeAnonymous)
{
    // the stack will hold the types center->border
    // first element is any type, then its guarenteed to be FunctionType
    std::vector<Type*> stack(1, const_cast<Type*>(type));

    // parse into the center, then build the type when going outside again
    std::function<identifier_info(void)> parseIdentifierHelper =
        [this, &stack, &parseIdentifierHelper, mayBeAnonymous](void) -> identifier_info
    {
        identifier_info identifier = nullident;
        const TokenType tty = currentToken().getType();
        switch (tty)
        {
            case TokenType::LPAREN:
            {
                unsigned parenCounter = 1;
                while (nextToken().getType() == TokenType::LPAREN)
                    parenCounter++;

                const Token& nt = currentToken();
                const TokenType tty = nt.getType();
                const uint32_t offs = nt.getOffset();
                if (isType(tty) && mayBeAnonymous)
                {
                    // parse function arguments and make function
                    m_Index--; // make sure we also parse the current token as argument
                    nary_members_t arguments;
                    parseFunctionArguments(arguments);

                    FunctionType* ftype =
                        m_Ast.createFunctionType(getChildTypeOrLast(stack), std::move(arguments));
                    PointerType* ptrFty = m_Ast.createPointerType(ftype);
                    setReturnTypeOrNone(stack, ptrFty);

                    stack.push_back(ptrFty);
                    stack.push_back(ftype);

                    for (unsigned i = 0; i < parenCounter; i++, nextToken())
                        expectToken(0, TokenType::RPAREN);

                    return identifier_info{nullnode, true, offs};
                }

                // we are now elligable to fiddle with fun pointers #D
                // case 1: (*   ...)(args...) -> fptr eligable
                else if (tty == TokenType::ASTERISK)
                {
                    // now we parse the inner expression
                    const uint32_t offs = nextToken().getOffset();
                    identifier_info identifier = parseIdentifierHelper();
                    identifier.implicitAsterix = false;

                    // expectToken(0, TokenType::RPAREN);

                    unsigned alreadyHandledRparens = 0;
                    for (; alreadyHandledRparens < parenCounter &&
                           currentToken().getType() == TokenType::RPAREN;
                         alreadyHandledRparens++, nextToken())
                        expectToken(0, TokenType::RPAREN);

                    const TokenType tty = currentToken().getType();
                    // case (*...)(args....)
                    if (tty == TokenType::LPAREN)
                    {
                        parenCounter++;
                        if (!mayBeAnonymous && (identifier.identifier == nullnode))
                            diagnostics::errorloc(
                                m_Locator.getLocationByOffset(offs),
                                PARSER_ERROR "this here may not be anonumous, expected identifier");

                        // parse function arguments and make function
                        nary_members_t arguments;
                        parseFunctionArguments(arguments);

                        const Type* lastReturnType = getChildTypeOrLast(stack);
                        FunctionType* ftype =
                            m_Ast.createFunctionType(lastReturnType, std::move(arguments));
                        PointerType* ptrFty = m_Ast.createPointerType(ftype);
                        setReturnTypeOrNone(stack, ptrFty);

                        stack.push_back(ftype);
                    }
                    // otherwise (*...)
                    else
                    {
                        PointerType* ptrFty = m_Ast.createPointerType(stack[0]);
                        setReturnTypeOrNone(stack, ptrFty);
                    }

                    // account for the rest of the rparens...
                    for (; alreadyHandledRparens < parenCounter;
                         alreadyHandledRparens++, nextToken())
                        expectToken(0, TokenType::RPAREN);

                    return identifier;
                }
                // case 2: (...)
                else
                {
                    identifier = parseIdentifierHelper();
                    for (unsigned i = 0; i < parenCounter; i++, nextToken())
                        expectToken(0, TokenType::RPAREN);

                    if (currentToken().getType() == TokenType::LPAREN)
                    {
                        m_Index--; // magic
                        goto magic;
                    }
                    return identifier;
                }
            }
            case TokenType::IDENTIFIER:
            {
                // by default it is implicit now, we can set it to non-implicit whenever
                // we parsed * before though
                identifier = identifier_info{currentToken().getContentKey(), true,
                                             currentToken().getOffset()};

            magic:
                const TokenType tty = nextToken().getType();
                if (tty == TokenType::LPAREN)
                {
                    // parse function arguments and make function
                    nary_members_t arguments;
                    parseFunctionArguments(arguments);
                    FunctionType* ftype =
                        m_Ast.createFunctionType(stack.back(), std::move(arguments));
                    // stack.push_back(ftype);

                    if (mayBeAnonymous)
                    {
                        PointerType* ptrFty = m_Ast.createPointerType(ftype);
                        setReturnTypeOrNone(stack, ptrFty);
                        stack.push_back(ptrFty);
                    }
                    stack.push_back(ftype);

                    expectToken(0, TokenType::RPAREN);
                    nextToken();
                }

                return identifier;
            }
            case TokenType::ASTERISK:
            {
                nextToken();

                const identifier_info indentifier = parseIdentifierHelper();
                const Type* lastReturnType = getChildTypeOrLast(stack);
                PointerType* fptr = m_Ast.createPointerType(lastReturnType);

                // TODO: this will make it a pointer if not function, is that wanted?
                setReturnTypeOrNone(stack, fptr);

                // just for the algorithm to remember the previous return type
                // (if needed for more function pointers)
                // stack.push_back(m_Ast.createFunctionType(lastReturnType, nary_members_t{}));
                stack.push_back(fptr);

                return indentifier;
                // diagnostics::todo("parseIdentifierHelper -> case ASTERISK");
            }
            case TokenType::RPAREN: // TODO does this solve (*) ?
                return identifier_info{nullnode, false, currentToken().getOffset()};
            default:
                diagnostics::errorloc(m_Locator.getLocationByOffset(currentToken().getOffset()),
                                      PARSER_ERROR "parseIdentifier: need to implement token",
                                      tokenToFormattedString(Token(tty, 0, 0), m_SymbolTable),
                                      " or its an error. Only God knows by now...");
        }
    };

    const identifier_info identifier = parseIdentifierHelper();

    type = (stack.size() > 1) ? stack[1] : stack[0];
    return identifier;
}

node_t Parser::parseBlock(void)
{
    nextToken();

    nary_children_t statements;

    // parse all statements
    while (true)
    {
        const Token& token = currentToken();
        uint32_t offset = token.getOffset();
        TokenType tty = token.getType();

        switch (tty)
        {
            case TokenType::AND:
            case TokenType::ASTERISK:
            case TokenType::BANG:
            case TokenType::IDENTIFIER:
            case TokenType::KW_BREAK:
            case TokenType::KW_CHAR:
            // case TokenType::CHARACTER:
            case TokenType::KW_CONTINUE:
            case TokenType::KW_IF:
            case TokenType::KW_INT:
            case TokenType::KW_RETURN:
            case TokenType::KW_SIZEOF:
            case TokenType::KW_VOID:
            case TokenType::KW_WHILE:
            case TokenType::KW_DO:
            case TokenType::KW_FOR:
            case TokenType::LBRACE:
            case TokenType::LPAREN:
            case TokenType::MINUS:
            case TokenType::MINUS_MINUS:
            case TokenType::CONSTANT:
            case TokenType::PLUS:
            case TokenType::PLUS_PLUS:
            case TokenType::STRING_LITERAL:
            case TokenType::TILDE:
            case TokenType::KW_GOTO:
            case TokenType::KW_STRUCT:
            case TokenType::SEMICOLON:
            {
                const node_t stmt = parseStatement();
                if (stmt != nullnode)
                    statements.push_back(stmt);
                continue;
            }

            case TokenType::RBRACE:
            {
                nextToken();
                return m_Ast.addBlock(std::move(statements), offset, token.getContentKey());
            }

            default:
            {
                diagnostics::errorloc(m_Locator.getLocationByOffset(currentToken().getOffset()),
                                      PARSER_ERROR "expected '}' or statement, but got",
                                      tokenToFormattedString(currentToken(), m_SymbolTable));
            }
        }
    }
}

node_t Parser::parseBreak(void) { return parseSimple(TokenType::KW_BREAK, NodeKind::BREAK); }

node_t Parser::parseGoto(void)
{
    const uint32_t offs = currentToken().getOffset();
    expectToken(0, TokenType::KW_GOTO);
    nextToken();

    expectToken(0, TokenType::IDENTIFIER);
    const uint32_t identifier = currentToken().getContentKey();
    nextToken();

    expectToken(0, TokenType::SEMICOLON);
    nextToken();

    return m_Ast.addGotoStatement(identifier, offs);
}

node_t Parser::parseEmptyStatement(void)
{
    const uint32_t offs = currentToken().getOffset();
    nextToken();
    return m_Ast.addEmptyStatement(offs);
}

node_t Parser::parseContinue(void)
{
    return parseSimple(TokenType::KW_CONTINUE, NodeKind::CONTINUE);
}

node_t Parser::parseDeclaration(bool allowObjDecl)
{
    const uint32_t offset = currentToken().getOffset();
    const Type* type = parseType();

    if (allowObjDecl && currentToken().getType() == TokenType::SEMICOLON)
    {
        nextToken();
        return m_Ast.addStructDeclaration(type, offset);
    }

    const identifier_info name = parseIdentifier(type);
    node_t init = nullnode;
    const TokenType tty = peekNextTokenType(0);
    if (tty == TokenType::EQ)
    {
        nextToken();
        init = parseExpression();
    }

    expectToken(0, TokenType::SEMICOLON);
    nextToken();
    return m_Ast.addDeclatationStatement(type, name, init);
}

node_t Parser::parseLabel(void)
{
    expectToken(0, TokenType::IDENTIFIER);
    expectToken(1, TokenType::COLON);
    const Token& t = currentToken();
    const uint32_t offs = t.getOffset();
    const uint32_t label = t.getContentKey();
    nextToken();
    nextToken();

    // the next token may not be a declaration (iff its type)
    if (isType(currentToken().getType()))
        diagnostics::errorloc(m_Locator.getLocationByOffset(currentToken().getOffset()),
                              PARSER_ERROR "unexpected type",
                              tokenToFormattedString(currentToken(), m_SymbolTable));

    // apparently we cannot have labels as last statement
    const node_t expr = parseStatement();
    return m_Ast.addLabel(label, expr, offs);
}

node_t Parser::parseOperand(void)
{
    const Token& token = currentToken();
    const TokenType tty = token.getType();
    const uint32_t offs = token.getOffset();
    switch (tty)
    {
        // unary operations
        case TokenType::AND:
        case TokenType::ASTERISK:
        case TokenType::BANG:
        case TokenType::MINUS:
        case TokenType::MINUS_MINUS:
        case TokenType::PLUS:
        case TokenType::PLUS_PLUS:
        case TokenType::TILDE:
        {
            nextToken();
            const node_t child = parseExpression(Precedence::UNARY);
            return m_Ast.addUnaryExpression(token, false, child);
        }

        // special needs token
        case TokenType::KW_SIZEOF:
        {
            nextToken();
            // test if we have a type we want to get the size of
            if (currentToken().getType() == TokenType::LPAREN && isType(peekNextTokenType(1)))
            {
                nextToken();
                const Type* type = parseType();
                assert(type != nullptr && "type was nullptr: Parser.cpp around line 567");

                if (peekNextTokenType(0) != TokenType::RPAREN)
                {
                    // it was not a rparen, so we try to parse identifier to see if that helps
                    // for example sizeof(void (*)())
                    const auto idinf = parseIdentifier(type, true);
                    if (idinf.identifier != nullnode)
                        diagnostics::errorloc(m_Locator.getLocationByOffset(idinf.offset),
                                              PARSER_ERROR "unexpected identifier in sizeof-type");
                }

                expectToken(0, TokenType::RPAREN);
                nextToken();
                return m_Ast.addUnaryExpression(token, false, type);
            }

            const node_t child = parseExpression(Precedence::UNARY);
            return m_Ast.addUnaryExpression(token, false, child);
        }

        // primary expressions
        case TokenType::IDENTIFIER:
        case TokenType::STRING_LITERAL:
        case TokenType::CONSTANT:
        {
            nextToken();
            return m_Ast.addPrimaryExpression(token);
        }

        // parentheses-expression
        case TokenType::LPAREN:
        {
            const Token& token = nextToken();
            if (isType(token.getType()))
            {
                const Type* type = parseType();

                // may be a more complex type
                if (peekNextTokenType(0) != TokenType::RPAREN)
                    (void)parseIdentifier(type, true);

                expectToken(0, TokenType::RPAREN);
                nextToken();

                node_t operand = nullnode;
                if (peekNextTokenType(0) != TokenType::SEMICOLON)
                    operand = parseExpression(Precedence::CAST);

                return m_Ast.addCastExpression(type, operand, offs);
            }
            else
            {
                const node_t child = parseExpression();
                expectToken(0, TokenType::RPAREN);
                nextToken();
                return child;
            }
        }

        default:
        {
            std::stringstream ss;
            (void)print(ss, token, m_SymbolTable);
            diagnostics::errorloc(m_Locator.getLocationByOffset(token.getOffset()),
                                  PARSER_ERROR "expected an expression, but got", ss.str());
        }
    }
}

node_t Parser::parseExpression(const Precedence precedence)
{
    // algorithm from the lecture

    node_t expr = parseOperand();

    while (true)
    {
        const Token& token = currentToken();
        const TokenType tty = token.getType();
        const uint32_t offs = token.getOffset();

        // reduce
        if (getLPrec(tty) < precedence)
            return expr;

        // else shift
        nextToken();
        switch (tty)
        {
            case TokenType::LBRACKET: expr = parseArrayAccess(expr, offs); break;
            case TokenType::LPAREN: expr = parseFunctionCall(expr, offs); break;
            case TokenType::DOT:
            case TokenType::ARROW: expr = parseMemberAccess(expr, offs, tty); break;
            case TokenType::QUESTION_MARK: expr = parseTernaryExpression(expr, offs); break;
            default:
            {
                const Precedence prec = getRPrec(tty);
                if (prec == Precedence::NONE)
                    expr = m_Ast.addUnaryExpression(token, true, expr);
                else
                {
                    const node_t right = parseExpression(prec);
                    expr = m_Ast.addBinaryExpression(tty, expr, right, offs);
                }
            }
        }
    }
    return expr;
}

node_t Parser::parseArrayAccess(node_t expr, uint32_t offs)
{
    const node_t index = parseExpression();
    expectToken(0, TokenType::RBRACKET);
    nextToken();
    return m_Ast.addBinaryExpression(TokenType::LBRACKET, expr, index, offs);
}

node_t Parser::parseFunctionCall(node_t expr, uint32_t offs)
{
    nary_children_t arguments;
    if (peekNextTokenType(0) != TokenType::RPAREN)
    {
        while (true)
        {
            arguments.push_back(parseExpression(Precedence::ASSIGNMENT));

            if (currentToken().getType() != TokenType::COMMA)
                break;

            nextToken();
        }
    }

    expectToken(0, TokenType::RPAREN);
    nextToken();
    return m_Ast.addCallExpression(expr, std::move(arguments), offs);
}

node_t Parser::parseMemberAccess(node_t expr, uint32_t offs, TokenType tty)
{
    expectToken(0, TokenType::IDENTIFIER);
    // const node_t field = parseExpression(Precedence::ASSIGNMENT);
    const node_t field = currentToken().getContentKey();
    nextToken();
    return m_Ast.addMemberAccess(expr, field, offs, tty);
}

node_t Parser::parseIf(void)
{
    const uint32_t offs = currentToken().getOffset();
    expectToken(0, TokenType::KW_IF);
    expectToken(1, TokenType::LPAREN);
    nextToken();
    nextToken();
    const node_t cond = parseExpression();
    expectToken(0, TokenType::RPAREN);
    nextToken();

    const node_t ifTrue = parseStatement(true);

    if (peekNextTokenType(0) == TokenType::KW_ELSE)
    {
        nextToken();
        const node_t ifFalse = parseStatement(true);
        return m_Ast.addIfElseStatement(cond, ifTrue, ifFalse, offs);
    }

    return m_Ast.addIfStatement(cond, ifTrue, offs);
}

node_t Parser::parseReturn(void)
{
    const uint32_t loc = currentToken().getOffset();
    nextToken();

    node_t expression = nullnode;
    if (peekNextTokenType(0) != TokenType::SEMICOLON)
    {
        expression = parseExpression();
        // nextToken();
    }
    expectToken(0, TokenType::SEMICOLON);
    nextToken();

    return m_Ast.addReturnStatement(loc, expression);
}

node_t Parser::parseWhile(void)
{
    const uint32_t offs = currentToken().getOffset();
    expectToken(0, TokenType::KW_WHILE);
    expectToken(1, TokenType::LPAREN);
    nextToken();
    nextToken();
    const node_t cond = parseExpression();
    expectToken(0, TokenType::RPAREN);
    nextToken();

    const node_t body = parseStatement(true);
    return m_Ast.addWhileStatement(cond, body, offs);
}

/// Syntactic sugar
node_t Parser::parseDoWhile(void)
{
    const uint32_t offs = currentToken().getOffset();
    expectToken(0, TokenType::KW_DO);
    nextToken();
    const node_t body = parseStatement(true);
    expectToken(0, TokenType::KW_WHILE);
    expectToken(1, TokenType::LPAREN);
    nextToken();
    nextToken();
    const node_t cond = parseExpression();
    expectToken(0, TokenType::RPAREN);
    expectToken(1, TokenType::SEMICOLON);
    nextToken();
    nextToken();

    /*
    do { block } while (expr);
    interpreted as --->
    { { block }; while (expr) { block } }
    */

    return m_Ast.addDoWhile(cond, body, offs);
}

node_t Parser::parseFor(void)
{
    const uint32_t offs = currentToken().getOffset();
    expectToken(0, TokenType::KW_FOR);
    expectToken(1, TokenType::LPAREN);
    nextToken();
    nextToken();
    const node_t init = parseStatement();
    const node_t cond = parseExpression();
    expectToken(0, TokenType::SEMICOLON);
    nextToken();
    node_t post;

    if (peekNextTokenType(0) != TokenType::RPAREN)
    {
        const node_t temp = parseExpression();
        post = m_Ast.addExpressionStatement(temp, m_Ast.getOffset(temp));
    }
    else
        post = m_Ast.addEmptyStatement(0);

    expectToken(0, TokenType::RPAREN);
    nextToken();

    const node_t body = parseStatement(true);
    return m_Ast.addForStatement(init, cond, post, body, offs);
}

node_t Parser::parseStatement(bool isInner)
{
    const Token& token = currentToken();
    const TokenType tty = token.getType();
    switch (tty)
    {
        case TokenType::KW_BREAK: return parseBreak();
        case TokenType::KW_CONTINUE: return parseContinue();
        case TokenType::KW_IF: return parseIf();
        case TokenType::LBRACE: return parseBlock();
        case TokenType::KW_RETURN: return parseReturn();
        case TokenType::KW_WHILE: return parseWhile();
        case TokenType::KW_DO: return parseDoWhile();
        case TokenType::KW_FOR: return parseFor();
        case TokenType::KW_INT:
        case TokenType::KW_CHAR:
        case TokenType::KW_VOID:
        case TokenType::KW_UNSIGNED:
        case TokenType::KW_STRUCT:
        {
            if (isInner)
                diagnostics::errorloc(m_Locator.getLocationByOffset(token.getOffset()), PARSER_ERROR
                                      "do we support declarations in inner statements?");
            return parseDeclaration(tty == TokenType::KW_STRUCT);
        }

        case TokenType::IDENTIFIER:
        {
            if (peekNextTokenType(1) == TokenType::COLON)
                return parseLabel();
        }
            [[fallthrough]];
        case TokenType::AND:
        case TokenType::ASTERISK:
        case TokenType::BANG:
        // case TokenType::CHARACTER:
        case TokenType::CONSTANT:
        case TokenType::KW_SIZEOF:
        case TokenType::LPAREN:
        case TokenType::MINUS:
        case TokenType::MINUS_MINUS:
        case TokenType::PLUS:
        case TokenType::PLUS_PLUS:
        case TokenType::STRING_LITERAL:
        case TokenType::TILDE: return parseExpressionStatement();
        case TokenType::KW_GOTO: return parseGoto();
        case TokenType::SEMICOLON: return parseEmptyStatement();
        default:
        {
            diagnostics::errorloc(m_Locator.getLocationByOffset(token.getOffset()),
                                  PARSER_ERROR "expected statement, but got",
                                  tokenToFormattedString(token, m_SymbolTable));
        }
    }
}

node_t Parser::parseTernaryExpression(node_t expr, uint32_t offs)
{
    const node_t ifTrue = parseExpression();
    expectToken(0, TokenType::COLON);
    nextToken();
    const node_t ifFalse = parseExpression(Precedence::CONDITIONAL);
    return m_Ast.addConditionalExpression(expr, ifTrue, ifFalse, offs);
}

node_t Parser::parseExpressionStatement(void)
{
    const uint32_t offs = currentToken().getOffset();
    const node_t expr = parseExpression();
    expectToken(0, TokenType::SEMICOLON);
    nextToken();
    return m_Ast.addExpressionStatement(expr, offs);
}

node_t Parser::parseSimple(TokenType tty, NodeKind nk)
{
    const uint32_t offs = currentToken().getOffset();
    expectToken(0, tty);
    expectToken(1, TokenType::SEMICOLON);
    nextToken();
    nextToken();

    return m_Ast.addLocatable(offs, nk);
}

const Type* Parser::parseType(void)
{
    TypeMap mask = TypeMap::UNDEFINED;
    if (currentToken().getType() == TokenType::KW_UNSIGNED)
    {
        mask = TypeMap::UNSIGNED_TYPE;
        nextToken();
    }

    const Type* ty;
    const Token& token = currentToken();
    switch (token.getType())
    {
        case TokenType::KW_CHAR:
            ty = (mask == TypeMap::UNSIGNED_TYPE) ? m_Ast.getUCharType() : m_Ast.getCharType();
            break;
        case TokenType::KW_INT:
            ty = (mask == TypeMap::UNSIGNED_TYPE) ? m_Ast.getUIntType() : m_Ast.getIntType();
            break;
        case TokenType::KW_VOID:
            if (mask == TypeMap::UNSIGNED_TYPE)
                diagnostics::errorloc(m_Locator.getLocationByOffset(token.getOffset()),
                                      PARSER_ERROR "unexpected token 'void' after 'unsigned'.");
            ty = m_Ast.getVoidType();
            break;
        case TokenType::KW_STRUCT:
            if (mask == TypeMap::UNSIGNED_TYPE)
                diagnostics::errorloc(m_Locator.getLocationByOffset(token.getOffset()),
                                      PARSER_ERROR "unexpected token 'struct' after 'unsigned'.");

            nextToken();
            ty = parseStructType(token.getOffset());
            goto parsePtrs;

        default:
        {
            if (mask == TypeMap::UNSIGNED_TYPE)
            {
                ty = m_Ast.getUIntType();
                break;
            }

            diagnostics::errorloc(m_Locator.getLocationByOffset(token.getOffset()),
                                  PARSER_ERROR "expected 'char', 'int', or 'void', but got",
                                  tokenToFormattedString(token, m_SymbolTable));
        }
    }
    nextToken();

parsePtrs:
    while (currentToken().getType() == TokenType::ASTERISK)
    {
        ty = m_Ast.createPointerType(ty);
        nextToken();
    }

    return ty;
}

const Type* Parser::parseStructType(uint32_t offs)
{
    (void)offs;
    uint32_t name = nullnode;
    // expectToken(0, TokenType::IDENTIFIER);

    // either identifier, otherwise we require a lbrace!
    if (peekNextTokenType(0) == TokenType::IDENTIFIER)
    {
        name = currentToken().getContentKey();
        nextToken();
    }
    else
        expectToken(0, TokenType::LBRACE);

    if (peekNextTokenType(0) != TokenType::LBRACE)
        return m_Ast.createNamedType(name);

    expectToken(0, TokenType::LBRACE);
    nextToken();

    // parse members
    nary_members_t members;
    while (peekNextTokenType(0) != TokenType::RBRACE)
    {
        const Type* mty = parseType();
        const identifier_info name = parseIdentifier(mty);
        members.push_back(MemberField(mty, name));
        expectToken(0, TokenType::SEMICOLON);
        nextToken();
    }
    if (members.size() == 0)
        diagnostics::errorloc(m_Locator.getLocationByOffset(currentToken().getOffset()),
                              PARSER_ERROR "empty structs not allowed!");

    nextToken();
    return m_Ast.createStructType(name, std::move(members));
}
