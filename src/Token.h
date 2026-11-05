#pragma once
#include "Locator.h"
#include "SymbolTable.h"
#include <stdint.h>

/// we use these two seeds and the xor operation to quickly
/// find the token type based on just the operators char.
/// e.g. the token type of + is the int value of '+'
///      the token type of += is the int value of '+' ^ XOR_SEED_OP_EQ
///      the token type of ++ is the int value of '+' ^ XOR_SEED_OP_OP
/// this is very hacky and fragil but its also fast!
constexpr uint8_t XOR_SEED_OP_EQ = 0x92;
constexpr uint8_t XOR_SEED_OP_OP = 0x7a;

enum class TokenType : uint8_t
{
    INVALID = 0,
    IDENTIFIER = 1,
    CONSTANT = 2,
    STRING_LITERAL = 3,

    // single punctuators
    LPAREN = '(',
    RPAREN = ')',
    LBRACKET = '[',
    RBRACKET = ']',
    LBRACE = '{',
    RBRACE = '}',
    DOT = '.',
    AND = '&',
    ASTERISK = '*',
    PLUS = '+',
    MINUS = '-',
    TILDE = '~',
    BANG = '!',
    SLASH = '/',
    PERCENT = '%',
    EQ = '=',
    LT = '<',
    GT = '>',
    HAT = '^',
    OR = '|',
    COLON = ':',
    SEMICOLON = ';',
    COMMA = ',',
    QUESTION_MARK = '?',
    HASHTAG = '#',

    // composed punctuators
    LT_EQ = '<' ^ XOR_SEED_OP_EQ,
    GT_EQ = '>' ^ XOR_SEED_OP_EQ,
    EQ_EQ = '=' ^ XOR_SEED_OP_EQ,
    BANG_EQ = '!' ^ XOR_SEED_OP_EQ,
    PLUS_EQ = '+' ^ XOR_SEED_OP_EQ,
    MINUS_EQ = '-' ^ XOR_SEED_OP_EQ,
    TIMES_EQ = '*' ^ XOR_SEED_OP_EQ,
    DIV_EQ = '/' ^ XOR_SEED_OP_EQ,
    MOD_EQ = '%' ^ XOR_SEED_OP_EQ,
    XOR_EQ = '^' ^ XOR_SEED_OP_EQ,
    AND_EQ = '&' ^ XOR_SEED_OP_EQ,
    OR_EQ = '|' ^ XOR_SEED_OP_EQ,

    AND_AND = '&' ^ XOR_SEED_OP_OP,
    OR_OR = '|' ^ XOR_SEED_OP_OP,
    LT_LT = '<' ^ XOR_SEED_OP_OP,
    GT_GT = '>' ^ XOR_SEED_OP_OP,
    PLUS_PLUS = '+' ^ XOR_SEED_OP_OP,
    MINUS_MINUS = '-' ^ XOR_SEED_OP_OP,
    HASHTAG_HASHTAG = '#' ^ XOR_SEED_OP_OP,

    ARROW = 4,
    DOT_DOT_DOT = 5,

    SHL_EQ = 100,
    SHR_EQ = 101,

    // keywords
    KW_AUTO = 8,
    KW_BREAK = 9,
    KW_CASE = 10,
    KW_CHAR = 11,
    KW_CONST = 12,
    KW_CONTINUE = 13,
    KW_DEFAULT = 14,
    KW_DO = 15,
    KW_DOUBLE = 16,
    KW_ELSE = 17,
    KW_ENUM = 18,
    KW_EXTERN = 19,
    KW_FLOAT = 20,
    KW_FOR = 21,
    KW_GOTO = 22,
    KW_IF = 23,
    KW_INLINE = 24,
    KW_INT = 25,
    KW_LONG = 26,
    KW_REGISTER = 27,
    KW_RESTRICT = 28,
    KW_RETURN = 29,
    KW_SHORT = 30,
    KW_SIGNED = 31,
    KW_SIZEOF = 32,
    KW_STATIC = 34,
    KW_STRUCT = 7,
    KW_SWITCH = 36,
    KW_TYPEDEF = 39,
    KW_UNION = 48,
    KW_UNSIGNED = 49,
    KW_VOID = 50,
    KW_VOLATILE = 51,
    KW_WHILE = 52,
    KW__ALIGNAS = 53,
    KW__ALIGNOF = 54,
    KW__ATOMIC = 55,
    KW__BOOL = 56,
    KW__COMPLEX = 57,
    KW__GENERIC = 64,
    KW__IMAGINARY = 65,
    KW__NORETURN = 66,
    KW__STATIC_ASSERT = 67,
    KW__THREAD_LOCAL = 69,

    END_OF_INPUT = (uint8_t)-1,
};

static_assert((int)TokenType::KW__THREAD_LOCAL <= 127);

/// <summary>
/// Tokens, hehe
/// </summary>
enum class TokenKind : uint8_t
{
    CONSTANT,
    KEYWORD,
    STRING_LITERAL,
    IDENTIFIER,
    PUNCTUATOR,
    INVALID, // should not be printed!
};

constexpr bool isType(TokenType tk)
{
    switch (tk)
    {
        case TokenType::KW_STRUCT:
        case TokenType::KW_CONST:
        case TokenType::KW_INT:
        case TokenType::KW_UNSIGNED:
        case TokenType::KW_CHAR:
        case TokenType::KW_VOID: return true;
        default: return false;
    }
}

/// <summary>
/// TODO
/// </summary>
class Token
{
private:
    // We use 8 bits to save the token kind,
    // and 56 bits to save the position in the file
    struct
    {
        const uint32_t m_TokenType : 8;
        const uint32_t m_Offset : 24;
    };

    const uint32_t m_ContentKey;

public:
    constexpr Token(const TokenType type, uint32_t offset, uint32_t contentKey)
        : m_TokenType((uint8_t)type), m_Offset(offset), m_ContentKey(contentKey)
    {
    }

    constexpr TokenType getType(void) const { return (TokenType)(m_TokenType); }

    constexpr uint32_t getOffset(void) const { return m_Offset; }

    constexpr uint32_t getContentKey(void) const { return m_ContentKey; }
};

/// <summary>
/// Precedence rules of nodes
/// inspired by the prog2 compiler
/// </summary>
enum class Precedence
{
    NONE,
    EXPRESSION,
    ASSIGNMENT,
    CONDITIONAL,
    LOGICAL_OR,
    LOGICAL_AND,
    OR,
    XOR,
    AND,
    EQUALITY,
    RELATIONAL,
    SHIFT,
    ADDITIVE,
    MULTIPLICATIVE,
    CAST,
    UNARY,
    POSTFIX,
    PRIMARY,
    MEMBER_ACCESS,
};

constexpr bool operator<(Precedence lhs, Precedence rhs) { return (int)lhs < (int)rhs; }

constexpr Precedence getLPrec(TokenType tty)
{
    switch (tty)
    {
        case TokenType::LPAREN: return Precedence::POSTFIX;
        case TokenType::LBRACKET: return Precedence::POSTFIX;

        case TokenType::MINUS_MINUS: return Precedence::POSTFIX;
        case TokenType::PLUS_PLUS: return Precedence::POSTFIX;
        case TokenType::PERCENT: return Precedence::MULTIPLICATIVE;
        case TokenType::SLASH: return Precedence::MULTIPLICATIVE;
        case TokenType::ASTERISK: return Precedence::MULTIPLICATIVE;
        case TokenType::MINUS: return Precedence::ADDITIVE;
        case TokenType::PLUS: return Precedence::ADDITIVE;
        case TokenType::LT_LT: return Precedence::SHIFT;
        case TokenType::GT_GT: return Precedence::SHIFT;
        case TokenType::LT: return Precedence::RELATIONAL;
        case TokenType::GT: return Precedence::RELATIONAL;
        case TokenType::GT_EQ: return Precedence::RELATIONAL;
        case TokenType::LT_EQ: return Precedence::RELATIONAL;
        case TokenType::EQ_EQ: return Precedence::EQUALITY;
        case TokenType::BANG_EQ: return Precedence::EQUALITY;
        case TokenType::AND: return Precedence::AND;
        case TokenType::HAT: return Precedence::XOR;
        case TokenType::OR: return Precedence::OR;
        case TokenType::AND_AND: return Precedence::LOGICAL_AND;
        case TokenType::OR_OR: return Precedence::LOGICAL_OR;
        case TokenType::QUESTION_MARK: return Precedence::CONDITIONAL;
        case TokenType::EQ:
        case TokenType::PLUS_EQ:
        case TokenType::MINUS_EQ:
        case TokenType::TIMES_EQ:
        case TokenType::DIV_EQ:
        case TokenType::SHL_EQ:
        case TokenType::SHR_EQ:
        case TokenType::XOR_EQ:
        case TokenType::AND_EQ:
        case TokenType::OR_EQ:
        case TokenType::MOD_EQ: return Precedence::ASSIGNMENT;
        case TokenType::DOT: return Precedence::MEMBER_ACCESS;
        case TokenType::ARROW:
            return Precedence::MEMBER_ACCESS;
            // case TokenType::COMMA: return Precedence::EXPRESSION;

        default: return Precedence::NONE;
    }
}

constexpr Precedence getRPrec(TokenType tty)
{
    switch (tty)
    {
        case TokenType::MINUS_MINUS: return Precedence::POSTFIX;
        case TokenType::PLUS_PLUS: return Precedence::POSTFIX;
        case TokenType::PERCENT: return Precedence::CAST;
        case TokenType::SLASH: return Precedence::CAST;
        case TokenType::ASTERISK: return Precedence::CAST;
        case TokenType::MINUS: return Precedence::MULTIPLICATIVE;
        case TokenType::PLUS: return Precedence::MULTIPLICATIVE;
        case TokenType::LT_LT: return Precedence::ADDITIVE;
        case TokenType::GT_GT: return Precedence::ADDITIVE;
        case TokenType::LT: return Precedence::SHIFT;
        case TokenType::GT: return Precedence::SHIFT;
        case TokenType::GT_EQ: return Precedence::SHIFT;
        case TokenType::LT_EQ: return Precedence::SHIFT;
        case TokenType::EQ_EQ: return Precedence::RELATIONAL;
        case TokenType::BANG_EQ: return Precedence::RELATIONAL;
        case TokenType::AND: return Precedence::EQUALITY;
        case TokenType::HAT: return Precedence::AND;
        case TokenType::OR: return Precedence::XOR;
        case TokenType::AND_AND: return Precedence::OR;
        case TokenType::OR_OR: return Precedence::LOGICAL_AND;
        case TokenType::QUESTION_MARK: return Precedence::CONDITIONAL;
        case TokenType::EQ:
        case TokenType::PLUS_EQ:
        case TokenType::MINUS_EQ:
        case TokenType::TIMES_EQ:
        case TokenType::DIV_EQ:
        case TokenType::SHL_EQ:
        case TokenType::SHR_EQ:
        case TokenType::XOR_EQ:
        case TokenType::AND_EQ:
        case TokenType::OR_EQ:
        case TokenType::MOD_EQ: return Precedence::ASSIGNMENT;
        // case TokenType::COMMA: return Precedence::ASSIGNMENT;
        case TokenType::COMMA: return Precedence ::NONE;

        default: return Precedence::NONE;
    }
}

/// <summary>
/// Converts a token kind to string in the format
/// </summary>
/// <param name="token"></param>
/// <returns></returns>
constexpr const char* tokenToString(const TokenType tty)
{
    switch (tty)
    {
        // handle invalid stuff after switch
        case TokenType::INVALID: return "invalid";

        case TokenType::IDENTIFIER: return "identifier ";
        case TokenType::CONSTANT: return "constant ";
        case TokenType::STRING_LITERAL: return "string-literal";

        // these need special handling because of digraphs
        case TokenType::LBRACKET:
        case TokenType::RBRACKET:
        case TokenType::LBRACE:
        case TokenType::RBRACE:
        case TokenType::HASHTAG:
        case TokenType::HASHTAG_HASHTAG: return "punctuator ";

        case TokenType::LPAREN: return "punctuator (";
        case TokenType::RPAREN: return "punctuator )";
        case TokenType::DOT: return "punctuator .";
        case TokenType::DOT_DOT_DOT: return "punctuator ...";
        case TokenType::COMMA: return "punctuator ,";
        case TokenType::COLON: return "punctuator :";
        case TokenType::SEMICOLON: return "punctuator ;";
        case TokenType::ARROW: return "punctuator ->";
        case TokenType::PLUS_PLUS: return "punctuator ++";
        case TokenType::MINUS_MINUS: return "punctuator --";
        case TokenType::AND: return "punctuator &";
        case TokenType::ASTERISK: return "punctuator *";
        case TokenType::QUESTION_MARK: return "punctuator ?";
        case TokenType::PLUS: return "punctuator +";
        case TokenType::MINUS: return "punctuator -";
        case TokenType::TILDE: return "punctuator ~";
        case TokenType::BANG: return "punctuator !";
        case TokenType::SLASH: return "punctuator /";
        case TokenType::PERCENT: return "punctuator %";
        case TokenType::LT_LT: return "punctuator <<";
        case TokenType::GT_GT: return "punctuator >>";
        case TokenType::EQ: return "punctuator =";
        case TokenType::LT: return "punctuator <";
        case TokenType::GT: return "punctuator >";
        case TokenType::LT_EQ: return "punctuator <=";
        case TokenType::GT_EQ: return "punctuator >=";
        case TokenType::EQ_EQ: return "punctuator ==";
        case TokenType::BANG_EQ: return "punctuator !=";
        case TokenType::HAT: return "punctuator ^";
        case TokenType::OR: return "punctuator |";
        case TokenType::AND_AND: return "punctuator &&";
        case TokenType::OR_OR: return "punctuator ||";
        case TokenType::PLUS_EQ: return "punctuator +=";
        case TokenType::MINUS_EQ: return "punctuator -=";
        case TokenType::TIMES_EQ: return "punctuator *=";
        case TokenType::DIV_EQ: return "punctuator /=";
        case TokenType::MOD_EQ: return "punctuator %=";
        case TokenType::XOR_EQ: return "punctuator ^=";
        case TokenType::SHL_EQ: return "punctuator <<=";
        case TokenType::SHR_EQ: return "punctuator >>=";
        case TokenType::AND_EQ: return "punctuator &=";
        case TokenType::OR_EQ: return "punctuator |=";

        case TokenType::KW_AUTO: return "keyword auto";
        case TokenType::KW_BREAK: return "keyword break";
        case TokenType::KW_CASE: return "keyword case";
        case TokenType::KW_CHAR: return "keyword char";
        case TokenType::KW_CONST: return "keyword const";
        case TokenType::KW_CONTINUE: return "keyword continue";
        case TokenType::KW_DEFAULT: return "keyword default";
        case TokenType::KW_DO: return "keyword do";
        case TokenType::KW_DOUBLE: return "keyword double";
        case TokenType::KW_ELSE: return "keyword else";
        case TokenType::KW_ENUM: return "keyword enum";
        case TokenType::KW_EXTERN: return "keyword extern";
        case TokenType::KW_FLOAT: return "keyword float";
        case TokenType::KW_FOR: return "keyword for";
        case TokenType::KW_GOTO: return "keyword goto";
        case TokenType::KW_IF: return "keyword if";
        case TokenType::KW_INLINE: return "keyword inline";
        case TokenType::KW_INT: return "keyword int";
        case TokenType::KW_LONG: return "keyword long";
        case TokenType::KW_REGISTER: return "keyword register";
        case TokenType::KW_RESTRICT: return "keyword restrict";
        case TokenType::KW_RETURN: return "keyword return";
        case TokenType::KW_SHORT: return "keyword short";
        case TokenType::KW_SIGNED: return "keyword signed";
        case TokenType::KW_SIZEOF: return "keyword sizeof";
        case TokenType::KW_STATIC: return "keyword static";
        case TokenType::KW_STRUCT: return "keyword struct";
        case TokenType::KW_SWITCH: return "keyword switch";
        case TokenType::KW_TYPEDEF: return "keyword typedef";
        case TokenType::KW_UNION: return "keyword union";
        case TokenType::KW_UNSIGNED: return "keyword unsigned";
        case TokenType::KW_VOID: return "keyword void";
        case TokenType::KW_VOLATILE: return "keyword volatile";
        case TokenType::KW_WHILE: return "keyword while";
        case TokenType::KW__ALIGNAS: return "keyword _Alignas";
        case TokenType::KW__ALIGNOF: return "keyword _Alignof";
        case TokenType::KW__ATOMIC: return "keyword _Atomic";
        case TokenType::KW__BOOL: return "keyword _Bool";
        case TokenType::KW__COMPLEX: return "keyword _Complex";
        case TokenType::KW__GENERIC: return "keyword _Generic";
        case TokenType::KW__IMAGINARY: return "keyword _Imaginary";
        case TokenType::KW__NORETURN: return "keyword _Noreturn";
        case TokenType::KW__STATIC_ASSERT: return "keyword _Static_assert";
        case TokenType::KW__THREAD_LOCAL: return "keyword _Thread_local";
        case TokenType::END_OF_INPUT: return "<<<end of input>>>";

        default: return "<<<invalid token type provided>>>";
    };
}

constexpr const char* tokenToString(const Token& token) { return tokenToString(token.getType()); }

std::string tokenToFormattedString(const Token& token, const SymbolTable& symbolTable);
std::ostream& print(std::ostream& os, const Token& token, const SymbolTable& symbolTable);
std::ostream& print(std::ostream& os, const Token& token, const Locator& locator,
                    const SymbolTable& symbolTable);
