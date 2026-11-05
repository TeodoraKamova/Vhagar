// heavily inspired by blog post from Sean Barrett
// please read it first before trying to understand this!
// https://nothings.org/computer/lexing.html

#include "Lexer.h"
#include "Diagnostics.h"
#include "Locator.h"
#include "SymbolTable.h"
#include "Token.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <string_view>

using U8 = uint8_t;
using U64 = uint64_t;

enum class State : uint8_t
{
    INVALID = 0,
    F_IDENTIFIER,
    F_INTEGER_LITERAL,
    F_STRING_LITERAL,
    F_CHAR_LITERAL,
    F_PUNCT,
    F_PUNCT_UNREAD,    // same as f_punct but unreads the last recently read character
    F_DOUBLE_OPERATOR, // ++ -- && || ##
    F_OPERATOR_EQUALS, // += -= *= ^= &= |= /= %= != <= >=
    F_SHIFT_SHIFT_EQ,
    F_DIV,
    F_DIV_EQ,
    F_END_OF_INPUT,
    F_DOT3,
    F_DOT2,
    F_DIGRAPH_OPEN,
    F_DIGRAPH_CLOSE,
    F_DIGRAPH_WEIRD,
    F_DIGRAPH_MODULO,
    F_ARROW,
#define LAST_FINAL_STATE ((uint8_t)State::F_ARROW)

    START,
    IDENTIFIER,
    INTEGER_LITERAL,
    STRING_LITERAL,         // anything after "
    STRING_LITERAL_ESCAPED, // anything directly after a backslash
    SLASH,
    COMMENT_SL,          // singleline comments
    COMMENT_ML,          // "/* ..."
    COMMENT_ML_ASTERISK, // "/* ... *"
    PUNCT2,
    PUNCT3,
    EQUALS,
    LESSTHAN,
    GREATERTHAN,
    HASHTAG,
    SHIFT,
    OPERATOR,
    MINUS,
    DOT1,
    DOT2,
    PERCENT,
    COLON,
    CHAR_LITERAL,
    CHAR_LITERAL_ESCAPED,
    CHAR_LITERAL_CLOSE,
    PERCENT_COLON,
    PERCENT_COLON_PERCENT,
    // ...

    __NUM_STATES // bit of a hack to automatically count states
};

constexpr auto __num_states = ((int)State::__NUM_STATES);

/// Basically 'a' and 'b' have exactly the same transitions
/// so we don't use the ascii alphabet as input language
/// but instead these equivalence classes, where
/// all the characters whithin one class behave equivalent.
enum class EquivClass : uint16_t
{
    UNKNOWN = 0,

    ALPHA = 1 * __num_states,  // 'a'..'z', 'A'..'Z', also includes underscore
    DIGIT = 2 * __num_states,  // '0'..'9'
    PUNCT1 = 3 * __num_states, // "()[]{};,~"
    PUNCT2 = 4 * __num_states, // + & |
    PUNCT3 = 5 * __num_states, // ! ^
    MINUS = 6 * __num_states,
    DOT = 7 * __num_states,
    COLON = 8 * __num_states,
    HAT = 9 * __num_states,
    LESSTHAN = 10 * __num_states,
    GREATERTHAN = 11 * __num_states,
    EQUALS = 12 * __num_states,
    BACKSLASH = 13 * __num_states,
    SLASH = 14 * __num_states,
    BANG = 15 * __num_states,
    DOUBLEQUOTE = 16 * __num_states,
    SINGLEQUOTE = 17 * __num_states,
    PERCENT = 18 * __num_states,
    ASTERISK = 19 * __num_states,
    HASHTAG = 20 * __num_states,
    WHITESPACE = 21 * __num_states, // ' ', '\t'
    NEWLINE = 22 * __num_states,    // '\n'
    END_OF_INPUT = 23 * __num_states,

    // #define __num_equiv_classes ((size_t)23)
};
constexpr auto __num_equiv_classes = ((size_t)24);

// static_assert(__num_equiv_classes * __num_states <= 256);

/// maps each character to its equivalence class
/// using macro magic and constexpr 🤝
/// https://stackoverflow.com/questions/56207065/creating-a-lookup-table-at-compile-time
constexpr static const auto EQUIV_CLASSES = []
{
    std::array<EquivClass, 256> equiv_classes = {};

#define EQUIV_CLASS_ACTION(ch, eq) equiv_classes[ch] = eq;
#include "def/EquivClassTable.def"
#undef EQUIV_CLASS_ACTION

    return equiv_classes;
}();

/// a transition table for a state-machine driven lexer
/// 1st dimension is current state
/// 2nd dimension is incoming character class
/// output represents next state
constexpr static const auto TRANSITION_TABLE = []
{
    std::array<uint8_t, __num_states* __num_equiv_classes> transition_table = {};

#define REGISTER_TRANSITION(current_state, equiv_class, next_state)                                \
    transition_table[U8(current_state) + U64(equiv_class)] = U8(next_state);
#include "def/TransitionTable.def"
#undef REGISTER_TRANSITION

    return transition_table;
}();

// this lookup table stores a one for every state that should track the length of the read input
// and a zero for all other states.
// It is used to measure the length of e.g. identifiers, literals etc.
constexpr static const auto IN_TOKEN = []
{
    std::array<uint8_t, __num_states> in_token = {};

    in_token[U8(State::IDENTIFIER)] = 1;
    in_token[U8(State::INTEGER_LITERAL)] = 1;
    in_token[U8(State::STRING_LITERAL)] = 1;
    in_token[U8(State::STRING_LITERAL_ESCAPED)] = 1;
    in_token[U8(State::CHAR_LITERAL)] = 1;
    in_token[U8(State::CHAR_LITERAL_ESCAPED)] = 1;
    in_token[U8(State::CHAR_LITERAL_CLOSE)] = 1;
    in_token[U8(State::F_PUNCT)] = 1;
    in_token[U8(State::PUNCT2)] = 1;
    in_token[U8(State::PUNCT3)] = 1;
    in_token[U8(State::LESSTHAN)] = 1;
    in_token[U8(State::GREATERTHAN)] = 1;
    in_token[U8(State::SHIFT)] = 1;
    in_token[U8(State::F_SHIFT_SHIFT_EQ)] = 1;
    in_token[U8(State::EQUALS)] = 1;
    in_token[U8(State::MINUS)] = 1;
    in_token[U8(State::OPERATOR)] = 1;
    in_token[U8(State::F_OPERATOR_EQUALS)] = 1;
    in_token[U8(State::F_DOUBLE_OPERATOR)] = 1;
    in_token[U8(State::COLON)] = 1;
    in_token[U8(State::PERCENT)] = 1;
    in_token[U8(State::DOT1)] = 1;
    in_token[U8(State::DOT2)] = 1;
    in_token[U8(State::F_DOT3)] = 1;
    in_token[U8(State::PUNCT2)] = 1;
    in_token[U8(State::F_DIGRAPH_OPEN)] = 1;
    in_token[U8(State::F_DIGRAPH_CLOSE)] = 1;
    in_token[U8(State::F_DIGRAPH_WEIRD)] = 1;
    in_token[U8(State::F_DIGRAPH_MODULO)] = 1;
    in_token[U8(State::PERCENT_COLON)] = 1;
    in_token[U8(State::PERCENT_COLON_PERCENT)] = 1;
    in_token[U8(State::HASHTAG)] = 1;
    in_token[U8(State::F_ARROW)] = 1;

    return in_token;
}();

static inline bool isFinalState(uint8_t state)
{
    // this check assumes that all non-final states come _after_ in the enum.
    return state <= LAST_FINAL_STATE;
}

/**
 * this function runs over the given string and validates
 * that after each \ there comes one of:
 * ' " ? \ a b f n r t v
 * which are all the valid escape sequences.
 *
 * It works by checking if the (1 << ch)'s bit is set
 * in the precomputed bitmasks after having seen a \ before.
 */
static bool validate_escape_character(std::string_view str)
{
    static const uint64_t arr[] = {
        0x8000008400000000, // \' \" \?
        0x0054404610000000, // \\ a b f n r t v
    };

    bool check = false;
    for (char ch : str)
    {
        if (check)
        {
            const uint64_t mask = arr[ch >= 64];

            if (ch >= 64)
                ch -= 64;

            if ((mask & (1ull << ch)) == 0)
                return false;
        }

        check = !check & (ch == '\\');
    }

    return true;
}

Lexer::Lexer(const std::string& code, const Locator& locator, bool needTokensLater)
    : m_SymbolTable(), m_Tokens(needTokensLater, locator, m_SymbolTable), m_Code(code),
      m_Locator(locator), m_Offset(0), m_Keywords()
{
    // reserving some space to avoid unnecessary re-allocations
    // we are inserting 44 keywords anyways.
    m_SymbolTable.reserve(std::max<size_t>(code.size() / 16, 64));

    // empirical measurements have shown that in actual C programs
    // there is roughly a bytes/tokens ratio of 5.4
    // so we overallocate by using 4 as our ratio
    if (needTokensLater)
        m_Tokens.reserve(code.size() / 4);

    // this will fill up the keyword map so that the lexer can lookup
    // the token type of identifiers in order to seperate keywords from identifiers.

    uint32_t symbol;
#define KEYWORD_ACTION(kw, str)                                                                    \
    symbol = m_SymbolTable.insert(std::string_view(str));                                          \
    m_Keywords.insert({symbol, kw});
#include "def/Keywords.def"
#undef KEYWORD_ACTION
}

bool Lexer::nextToken()
{
    size_t length = 0;
    uint8_t state = (uint8_t)State::START;
    do
    {
        uint8_t ch = m_Code[m_Offset++];
        EquivClass equiv_class = EQUIV_CLASSES[ch];
        state = TRANSITION_TABLE[state + (size_t)equiv_class];

        // states marked as in_token measure the length of the matched
        // string. we cannot just do offset - start because this would
        // include whitespace characters  or comments.
        length += IN_TOKEN[state];
    } while (!isFinalState(state));

    uint32_t start = (uint32_t)(m_Offset - length);

    // we have fully traversed the state-machine and are now in a final
    // state. its time to allocate the token and maybe do something specific
    // e.g. parsing the integer or putting the string into the symbol table.
    switch ((State)state)
    {
        case State::F_IDENTIFIER:
        {
            --m_Offset; // unread the char that came after ident in e.g. func(
            uint32_t symbol =
                m_SymbolTable.insert(std::string_view(m_Code.data() + start - 1, length));

            auto it = m_Keywords.find(symbol);
            auto tt = it != m_Keywords.end() ? it->second : TokenType::IDENTIFIER;

            m_Tokens.push_back(Token{tt, start - 1, symbol});
            break;
        }
        case State::F_INTEGER_LITERAL:
        {
            --m_Offset;
            auto symbol = m_SymbolTable.insert(std::string_view(m_Code.data() + start - 1, length));
            m_Tokens.push_back(Token{TokenType::CONSTANT, start - 1, symbol});
            break;
        }
        case State::F_STRING_LITERAL:
        {
            // We want to exlude the quotes (") from the interned symbol.
            // Also note that consecutive string literals will NOT be merged.
            auto str = std::string_view(m_Code.data() + start, length - 1);

            if (!validate_escape_character(str))
                goto ERROR;

            auto symbol = m_SymbolTable.insert(str);
            m_Tokens.push_back(Token{TokenType::STRING_LITERAL, start - 1, symbol});
            break;
        }
        case State::F_CHAR_LITERAL:
        {
            auto str = std::string_view(m_Code.data() + start - 1, length + 1);

            if (!validate_escape_character(str))
                goto ERROR;

            auto symbol = m_SymbolTable.insert(str);
            m_Tokens.push_back(Token{TokenType::CONSTANT, start - 1, symbol});
            break;
        }
        case State::F_PUNCT:
        {
            auto tt = (TokenType)m_Code[start];
            m_Tokens.push_back(Token{tt, start, 0});
            break;
        }
        case State::F_PUNCT_UNREAD:
        {
            --m_Offset;
            auto tt = (TokenType)m_Code[start - 1];
            m_Tokens.push_back(Token{tt, (uint32_t)start - 1, 0});
            break;
        }
        case State::F_DIV:
        {
            --m_Offset;
            m_Tokens.push_back(Token{TokenType::SLASH, (uint32_t)start - 2, 0});
            break;
        }
        case State::F_DIV_EQ:
        {
            m_Tokens.push_back(Token{TokenType::DIV_EQ, (uint32_t)start - 2, 0});
            break;
        }
        case State::F_DOT3:
        {
            m_Tokens.push_back(Token{TokenType::DOT_DOT_DOT, start, 0});
            break;
        }
        case State::F_DOT2:
        {
            m_Tokens.push_back(Token{TokenType::DOT, start - 1, 0});
            m_Tokens.push_back(Token{TokenType::DOT, start, 0});
            m_Offset--;
            break;
        }
        case State::F_ARROW:
        {
            m_Tokens.push_back(Token{TokenType::ARROW, start, 0});
            break;
        }
        case State::F_OPERATOR_EQUALS:
        {
            TokenType tt = (TokenType)(m_Code[start] ^ XOR_SEED_OP_EQ);
            m_Tokens.push_back(Token{tt, start, 0});
            break;
        }
        case State::F_SHIFT_SHIFT_EQ:
        {
            TokenType tt = m_Code[start] == '<' ? TokenType::SHL_EQ : TokenType::SHR_EQ;
            m_Tokens.push_back(Token{tt, start, 0});
            break;
        }
        case State::F_DOUBLE_OPERATOR:
        {
            TokenType tt;
            if (m_Code[start] == m_Code[start + 1])
            {
                tt = (TokenType)(m_Code[start] ^ XOR_SEED_OP_OP);

                m_Offset -= m_Code[start] == '<';
                m_Offset -= m_Code[start] == '>';
            }
            else
            {
                // we re-scan the second one but its fine.... this is invalid C anyways
                m_Offset--;
                tt = (TokenType)m_Code[start];
            }

            m_Tokens.push_back(Token{tt, start, 0});
            break;
        }
        case State::F_DIGRAPH_OPEN:
        {
            TokenType tt = m_Code[start + 1] == ':' ? TokenType::LBRACKET : TokenType::LBRACE;
            auto symbol = m_SymbolTable.insert(std::string_view(m_Code.data() + start, length));
            m_Tokens.push_back(Token{tt, start, symbol});
            break;
        }
        case State::F_DIGRAPH_CLOSE:
        {
            TokenType tt = m_Code[start] == ':' ? TokenType::RBRACKET : TokenType::RBRACE;
            auto symbol = m_SymbolTable.insert(std::string_view(m_Code.data() + start, length - 1));
            m_Tokens.push_back(Token{tt, start, symbol});
            break;
        }
        case State::F_DIGRAPH_WEIRD:
        {
            TokenType tt = length == 3 ? TokenType::HASHTAG : TokenType::HASHTAG_HASHTAG;
            auto symbol = m_SymbolTable.insert(std::string_view(m_Code.data() + start, length - 1));
            m_Tokens.push_back(Token{tt, start, symbol});

            if (length == 3)
                --m_Offset;

            break;
        }
        case State::F_DIGRAPH_MODULO:
        {
            // perfectly reasonable implementation of the edge case %:%
            // not hacky what so ever. works 10/10. didnt take weeks to find. trust me (:
            auto symbol = m_SymbolTable.insert(std::string_view(m_Code.data() + start, length - 1));

            m_Tokens.push_back(Token{TokenType::HASHTAG, start, symbol});

            m_Offset -= 2;
            break;
        }
        case State::F_END_OF_INPUT:
        {
            auto tt = TokenType::END_OF_INPUT;
            m_Tokens.push_back(Token{tt, static_cast<uint32_t>(this->m_Offset - 1), 0});
            return false;
        }
        default:
        {
        ERROR:
            auto location = m_Locator.getLocationByOffset((uint32_t)m_Offset - 1);

            if (m_Offset >= 3)
            {
                diagnostics::errorloc(location, "unexpected character: '", m_Code[m_Offset - 1],
                                      "' appearing after: ", m_Code[m_Offset - 3],
                                      m_Code[m_Offset - 2], m_Code[m_Offset - 1]);
            }
            else
            {
                diagnostics::errorloc(location, "unexpected character: '", m_Code[m_Offset - 1],
                                      "'");
            }
        }
    }

    return true;
}

void Lexer::lex(void)
{
    while (nextToken()) {}
}
