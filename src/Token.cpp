#include "Token.h"
#include <iostream>
#include <ostream>
#include <sstream>

std::string tokenToFormattedString(const Token& token, const SymbolTable& symbolTable)
{
    std::stringstream ss;
    print(ss, token, symbolTable);
    return ss.str();
}

std::ostream& print(std::ostream& os, const Token& token, const Locator& locator,
                    const SymbolTable& symbolTable)
{
    os << locator.getLocationByOffset(token.getOffset()) << ": ";
    return print(os, token, symbolTable);
}

std::ostream& print(std::ostream& os, const Token& token, const SymbolTable& symbolTable)
{
    os << tokenToString(token);

    // special cases that need to be printed extra:
    switch (token.getType())
    {
        case TokenType::CONSTANT:
        case TokenType::IDENTIFIER:
        {
            const std::string_view* content = symbolTable[token.getContentKey()];
            if (content != nullptr)
                os << *content;
            else
                os << "<nullptr>";
            break;
        }
        case TokenType::STRING_LITERAL:
        {
            const std::string_view* content = symbolTable[token.getContentKey()];
            os << " \"";

            if (content != nullptr)
                os << *content;
            else
                os << "<nullptr>";

            os << '"';
            break;
        }
        case TokenType::LBRACE:
        {
            os << (token.getContentKey() ? "<%" : "{");
            break;
        }
        case TokenType::RBRACE:
        {
            os << (token.getContentKey() ? "%>" : "}");
            break;
        }
        case TokenType::LBRACKET:
        {
            os << (token.getContentKey() ? "<:" : "[");
            break;
        }
        case TokenType::RBRACKET:
        {
            os << (token.getContentKey() ? ":>" : "]");
            break;
        }
        case TokenType::HASHTAG:
        {
            os << (token.getContentKey() ? "%:" : "#");
            break;
        }
        case TokenType::HASHTAG_HASHTAG:
        {
            os << (token.getContentKey() ? "%:%:" : "##");
            break;
        }
        default: break; // all other cases are not interesting
    }
    return os;
}