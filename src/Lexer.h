#pragma once

#include "Locator.h"
#include "PrintVector.h"
#include "Token.h"

#include <unordered_map>
#include <vector>

class Lexer
{
private:
    SymbolTable m_SymbolTable;
    PrintVector m_Tokens;
    const std::string& m_Code;
    const Locator& m_Locator;

    /// Will always be pointing at the next char to read.
    /// This offset is also used to reference the location of each token.
    size_t m_Offset;

    std::unordered_map<uint32_t, TokenType> m_Keywords;

public:
    Lexer(const std::string& code, const Locator& locator, bool needTokensLater);

    void lex(void);

    SymbolTable& getSymbolTable(void) { return m_SymbolTable; }
    const SymbolTable& getSymbolTable(void) const { return m_SymbolTable; }
    const std::vector<Token>& getTokens(void) const { return m_Tokens; }

private:
    bool nextToken(void);
};
