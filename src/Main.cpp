#include "ArgParse.h"
#include "Ast.h"
#include "Backend.h"
#include "Lexer.h"
#include "Locator.h"
#include "Parser.h"
#include "SemanticAnalysis.h"
#include "SymbolTable.h"
#include "Token.h"
#include "Types.h"

#include <iostream>
#include <vector>

// #define ENNABLE_UNIT_TESTS

inline std::ostream& printTokens(std::ostream& ostr, const std::vector<Token>& tokens,
                                 const Locator& locator, const SymbolTable& symbolTable)
{
    // we only go to token.size() - 1 because we don't want
    // to print the END_OF_FILE token
    for (size_t i = 0; i < tokens.size() - 1; i++)
    {
        print(ostr, tokens[i], locator, symbolTable);

        if (i != tokens.size() - 1)
            ostr << '\n';
    }

    return ostr;
}

static void start(int argc, const char** argv)
{
    /// ------------------- PARSE ARGS -------------------

    auto arguments = parseArguments(argc, argv);

    /// ------------------- LOAD FILE -------------------

    std::string code;
    readFile(arguments.filename, code);
    const Locator locator(arguments.filename, code);

    /// ------------------- LEXING -------------------

    bool needTokenBuffer = arguments.flags != Flags::TOKENIZE;

    Lexer lexer(code, locator, needTokenBuffer);
    lexer.lex();

    // only print if there are more flags than the Flags::TOKENIZE, since otherwise
    // it would have been printed before in the lexer already
    if (arguments.flags & Flags::TOKENIZE && needTokenBuffer)
        printTokens(std::cout, lexer.getTokens(), locator, lexer.getSymbolTable());

    /// ------------------- PARSING -------------------

    if (arguments.flags < Flags::PARSE)
        return;

    Parser parser(lexer.getTokens(), locator, lexer.getSymbolTable());
    Ast& ast = parser.parse();

    if (arguments.flags & Flags::PRINT_AST)
        std::cout << ast << std::endl;

    /// ------------------- SEMANTICS -------------------

    SemanticAnalysis sema(ast, locator);
    sema.validate();

    /// ------------------- CODEGEN ---------------------

    if (arguments.flags < Flags::COMPILE)
        return;

    Backend backend(arguments.filename);
    backend.codegen(ast, locator);

    /// ------------------- OPTIMIZE ---------------------

    if (arguments.flags & Flags::OPTIMIZE)
    {
        backend.mem2reg();
        backend.sccp();
        backend.simplifyCFG();

        backend.unwrapper();
        // backend.inliner();
        backend.mem2reg();

        backend.sccp();
        backend.licm();
        backend.simplifyCFG();

        backend.constDiv();
        backend.tco();
        backend.dedupGlobalStringData();

        // and many more...
    }

    /// ---------------- WRITE OUTPUT ---------------------

    if (!(arguments.flags & Flags::OPTIMIZE_COMPILE_TIME))
        backend.verifyModule();

    const auto& outfile = getOutputFilename(arguments.filename);
    backend.emitModule(outfile);

    return;
}

int main(int argc, const char** argv)
{
#ifndef ENNABLE_UNIT_TESTS

    start(argc, argv);

#else

    // unit tests may go here
    std::string code = R"V0G0N(
int(*f())();
int(*g());
)V0G0N";
    const Locator locator("unit-test", code);

    Lexer lexer(code, locator);
    lexer.lex();

    // printTokens(std::cout, lexer.getTokens(), locator, lexer.getSymbolTable());

    Parser parser(lexer.getTokens(), locator, lexer.getSymbolTable());
    parser.parse();

    std::cout << parser.getAst() << std::endl;
#endif

    return EXIT_SUCCESS;
}
