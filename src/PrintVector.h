#pragma once

#include "Locator.h"
#include "SymbolTable.h"
#include "Token.h"

#include <iostream>
#include <vector>

class PrintVector : public std::vector<Token>
{
    const bool m_SaveToVec;

    const Locator& m_Locator;
    const SymbolTable& m_SymbolTable;

public:
    PrintVector(bool save, const Locator& locator, const SymbolTable& symbolTable)
        : m_SaveToVec(save), m_Locator(locator), m_SymbolTable(symbolTable)
    {
    }

    template <typename... Args> void emplace_back(Args&&... args)
    {
        constexpr std::size_t numArgs = sizeof...(args);
        static_assert(numArgs == 0,
                      "pls do not use the emplace_back, as this is too diff to implement.");
    }

    void push_back(const Token& token)
    {
        if (m_SaveToVec)
            std::vector<Token>::push_back(token);
        else if (token.getType() != TokenType::END_OF_INPUT)
            print(std::cout, token, m_Locator, m_SymbolTable) << '\n';
    }
};
