#include "Ast.h"

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

Ast::Ast(const Locator& locator, SymbolTable& symbolTable)
    : m_Att(), m_NodeKind(), m_Children(), m_Types(), m_Offset(), m_ContentId(), m_Locator(locator),
      m_SymbolTable(symbolTable), c_Const1Node(1)
{
    m_Att.reserve(16);
    m_Att.emplace_back(new BaseType(BasicTypes::VOID));
    m_Att.emplace_back(new BaseType(BasicTypes::INT));
    m_Att.emplace_back(new BaseType(BasicTypes::UINT));
    m_Att.emplace_back(new BaseType(BasicTypes::CHAR));
    m_Att.emplace_back(new BaseType(BasicTypes::UCHAR));

    // add origin node
    addBlock(nary_children_t(16), 0, 0);

    // add const 1 node
    emplace_back(NodeKind::CONSTANT, none_child_t{}, 0 /* offset*/, getIntType(),
                 symbolTable.insert("1"));
}

Ast::~Ast(void)
{
    // cleanup
    for (const Type* ty : m_Att)
        delete ty;
}

void printNode(node_t node, int indentLevel, std::ostream& os, const Ast& ast, bool initBlock,
               bool braces, bool special, bool comingFromBlock);
void printFuncType(const FunctionType* fty, node_t node, std::ostream& os, const Ast& ast,
                   const SymbolTable& m_SymbolTable);
void printStruct(const StructType* fty, std::ostream& os, const SymbolTable& m_SymbolTable,
                 std::string indent, const Ast& ast);
void printParams(const FunctionType* ftype, std::ostream& os, const Ast& ast,
                 const SymbolTable& m_SymbolTable);

// We are now using the operator<< on std::ostream, which is defined in Types.h currently
// void printType(const Type* t, std::ostream& os, const Ast& ast)
//{
//     while ((ast.getAtt(t)).getKind() == BaseType::POINTER_TYPE)
//     {
//         os << "*";
//         t = ast.getAtt(t).getChildren()[0];
//     }
//     os << ast.getAtt(t);
// }
void printBinary(const std::string& op, std::ostream& os, const Ast& ast, node_t node)
{
    os << "(";
    printNode(ast.getChildren(node)[0], 0, os, ast, false, true, false, false);
    os << " " << op << " ";
    printNode(ast.getChildren(node)[1], 0, os, ast, false, true, false, false);
    os << ")";
}

void printUnaryFront(const std::string& op, std::ostream& os, const Ast& ast, node_t node)
{
    os << "(";
    os << op;
    printNode(ast.getChildren(node)[0], 0, os, ast, false, true, false, false);
    os << ")";
}

void printUnaryBack(const std::string& op, std::ostream& os, const Ast& ast, node_t node)
{
    os << "(";
    printNode(ast.getChildren(node)[0], 0, os, ast, false, true, false, false);
    os << " " << op << " ";
    os << ")";
}

void printStructName(const NamedType* fty, std::ostream& os, const SymbolTable& m_SymbolTable)
{
    os << "struct ";
    if (fty->getName() != 0)
        os << *m_SymbolTable[fty->getName()];
}
int printFuncPointer(const Type* fty3, std::ostream& os, const SymbolTable& m_SymbolTable,
                     const Ast& ast)
{
    int pointerDepth = 1;
    while (typeid(*fty3) == typeid(PointerType))
    {
        const PointerType* fty4 = static_cast<const PointerType*>(fty3);
        fty3 = fty4->getPointsTo();
        ++pointerDepth;
    }
    if (typeid(*fty3) == typeid(NamedType))
    {
        const NamedType* nty = static_cast<const NamedType*>(fty3);
        printStructName(nty, os, m_SymbolTable);
        os << " ";
    }
    else if (typeid(*fty3) == typeid(StructType))
    {
        const StructType* sty = static_cast<const StructType*>(fty3);
        printStruct(sty, os, m_SymbolTable, "", ast);
        os << " ";
    }
    else if (typeid(*fty3) == typeid(FunctionType))
    {
        int depthCounter = 1;
        const FunctionType* fty = static_cast<const FunctionType*>(fty3);
        const Type* rt = fty->getReturnType();
        if (typeid(*rt) == typeid(NamedType))
        {
            const NamedType* nt = static_cast<const NamedType*>(rt);
            printStructName(nt, os, m_SymbolTable);
            os << " ";
        }
        else if (typeid(*rt) == typeid(StructType))
        {
            const StructType* pt = static_cast<const StructType*>(rt);
            printStruct(pt, os, m_SymbolTable, "", ast);
            os << " ";
        }
        else if (typeid(*rt) != typeid(PointerType))
            os << rt << " ";
        else
        {
            const PointerType* pt = static_cast<const PointerType*>(rt);
            const Type* fty3 = pt->getPointsTo();
            while (typeid(*fty3) == typeid(PointerType))
            {
                const PointerType* pt2 = static_cast<const PointerType*>(fty3);
                fty3 = pt2->getPointsTo();
                depthCounter++;
            }
            if (typeid(*fty3) == typeid(NamedType))
            {
                const NamedType* nt = static_cast<const NamedType*>(fty3);
                printStructName(nt, os, m_SymbolTable);
            }
            else if (typeid(*fty3) == typeid(StructType))
            {
                const StructType* pt = static_cast<const StructType*>(fty3);
                printStruct(pt, os, m_SymbolTable, "", ast);
            }
            else if (typeid(*fty3) != typeid(PointerType))
                os << fty3;
            os << " ";
            while (depthCounter > 0)
            {
                os << "(*";
                depthCounter--;
            }
        }
    }
    else
        os << fty3 << " ";
    int pd = pointerDepth;
    while (pd > 0)
    {
        os << "(";
        os << "*";
        --pd;
    }
    return pointerDepth;
}

void printPointer(const PointerType* fty, const Type* ftyy, node_t n, std::ostream& os,
                  const Ast& ast, const SymbolTable& m_SymbolTable, bool asterix = false)
{
    std::ostringstream oss;
    const Type* fty3 = fty->getPointsTo();
    int pointerDepth = 1;
    const FunctionType* ft2 = 0;
    int funcRetPointerDepth = 0;
    while (typeid(*fty3) == typeid(PointerType))
    {
        const PointerType* fty4 = static_cast<const PointerType*>(fty3);
        fty3 = fty4->getPointsTo();
        ++pointerDepth;
    }
    if (typeid(*fty3) == typeid(StructType))
    {
        const StructType* fty4 = static_cast<const StructType*>(fty3);
        printStruct(fty4, oss, m_SymbolTable, "", ast);
    }
    else if (typeid(*fty3) == typeid(NamedType))
    {
        const NamedType* fty4 = static_cast<const NamedType*>(fty3);
        printStructName(fty4, oss, m_SymbolTable);
    }
    else if (typeid(*fty3) == typeid(FunctionType))
    {
        const FunctionType* fty4 = static_cast<const FunctionType*>(fty3);
        auto retType = fty4->getReturnType();

        int pointerDepth1 = 0;
        if (typeid(*retType) == typeid(NamedType))
        {
            const NamedType* nt = static_cast<const NamedType*>(retType);
            printStructName(nt, os, m_SymbolTable);
        }
        else if (typeid(*retType) == typeid(StructType))
        {
            const StructType* pt = static_cast<const StructType*>(retType);
            printStruct(pt, os, m_SymbolTable, "", ast);
        }
        else if (typeid(*retType) != typeid(PointerType))
            os << retType << ' ';
        else
        {
            const PointerType* pt = static_cast<const PointerType*>(retType);
            const Type* fty3 = pt->getPointsTo();
            pointerDepth1 = printFuncPointer(fty3, os, m_SymbolTable, ast);
            if (typeid(*fty3) == typeid(FunctionType))
            {
                ft2 = static_cast<const FunctionType*>(fty3);
                const Type* helper = ft2->getReturnType();
                while (typeid(*helper) == typeid(PointerType))
                {
                    const PointerType* pt = static_cast<const PointerType*>(helper);
                    helper = pt->getPointsTo();
                    funcRetPointerDepth++;
                }
            }
        }
        if (!asterix)
        {
            os << "((";
            int copyDepth = pointerDepth - 1;
            while (pointerDepth > 0)
            {
                --pointerDepth;
                os << "*";
                if (pointerDepth > 0 && pointerDepth != copyDepth + 1)
                    os << "(";
            }
            if (n != 0)
                os << *m_SymbolTable[n] << ")";
            else
                os << ")";
            while (copyDepth > 0)
            {
                os << ")";
                --copyDepth;
            }
        }
        else
        {
            os << '(';
            if (n != 0)
                os << *m_SymbolTable[n];
        }
        os << "(";
        const auto& params = fty4->getParameters();
        for (uint32_t i = 0; i < params.size(); i++)
        {
            const Type* const type = params[i].type;
            const node_t identifier = params[i].contentId;
            if (typeid(*type) == typeid(PointerType))
            {
                const PointerType* fty2 = static_cast<const PointerType*>(type);
                if (params[i].implicitAsterix == 1)
                    printPointer(fty2, type, identifier, os, ast, m_SymbolTable, true);
                else
                    printPointer(fty2, type, identifier, os, ast, m_SymbolTable);
            }
            else if (typeid(*type) == typeid(NamedType))
            {
                const NamedType* fty = static_cast<const NamedType*>(type);
                printStructName(fty, os, m_SymbolTable);
                if (identifier != 0)
                    os << " " << *m_SymbolTable[identifier];
            }
            else if (typeid(*type) == typeid(StructType))
            {
                const StructType* fty = static_cast<const StructType*>(type);
                printStruct(fty, os, m_SymbolTable, "", ast);
                if (identifier != 0)
                    os << " " << *m_SymbolTable[identifier];
            }
            else
            {
                os << type;
                if (identifier != 0)
                    os << " " << *m_SymbolTable[identifier];
            }
            if (i + 1 < params.size())
                os << ", ";
        }
        os << "))";
        while (pointerDepth1 > 0)
        {
            os << ')';
            pointerDepth1--;
        }
        if (ft2 != 0)
            printParams(ft2, os, ast, m_SymbolTable);
        while (funcRetPointerDepth > 0)
        {
            os << ")";
            funcRetPointerDepth--;
        }
        return;
    }
    if (ftyy != 0)
        oss << ftyy;
    std::string result = oss.str();
    size_t pos = result.find("(*");
    if (pos != std::string::npos)
    {
        std::string before = result.substr(0, pos);
        std::string after = result.substr(pos);
        if (n != 0)
        {
            os << before << " ";
            os << after;
            os << *m_SymbolTable[n];
        }
        else
        {
            os << before << " " << after;
        }
        while (pointerDepth > 0)
        {
            os << ")";
            pointerDepth--;
        }
    }
}
void printParams(const FunctionType* ftype, std::ostream& os, const Ast& ast,
                 const SymbolTable& m_SymbolTable)
{
    os << '(';

    const auto& params = ftype->getParameters();
    // overflow = (ast.getChildren(node).size() - 2 > ast.getChildren(node).size())
    //                ? 0
    //                : ast.getChildren(node).size() - 2;
    if (params.size() == 0)
        os << "void";

    for (uint32_t i = 0; i < params.size(); i++)
    {
        const Type* const type = params[i].type;
        const node_t identifier = params[i].contentId;
        if (typeid(*type) == typeid(PointerType))
        {
            const PointerType* fty2 = static_cast<const PointerType*>(type);
            if (params[i].implicitAsterix == 1)
                printPointer(fty2, type, identifier, os, ast, m_SymbolTable, true);
            else
                printPointer(fty2, type, identifier, os, ast, m_SymbolTable);
        }
        else if (typeid(*type) == typeid(NamedType))
        {
            const NamedType* fty = static_cast<const NamedType*>(type);
            printStructName(fty, os, m_SymbolTable);
            if (identifier != 0)
                os << " " << *m_SymbolTable[identifier];
        }
        else if (typeid(*type) == typeid(StructType))
        {
            const StructType* fty = static_cast<const StructType*>(type);
            printStruct(fty, os, m_SymbolTable, "", ast);
            if (identifier != 0)
                os << " " << *m_SymbolTable[identifier];
        }
        else
        {
            os << type;
            if (identifier != 0)
                os << " " << *m_SymbolTable[identifier];
        }
        if (i + 1 < params.size())
            os << ", ";
    }

    os << ")";
}
void printFuncType(const FunctionType* fty, node_t node, std::ostream& os, const Ast& ast,
                   const SymbolTable& m_SymbolTable)
{
    const FunctionType* ftype = dynamic_cast<const FunctionType*>(fty);
    auto retType = ftype->getReturnType();
    int funcRetPointerDepth = 0;
    const FunctionType* ft2 = 0;
    int pointerDepth = 0;
    if (typeid(*retType) == typeid(NamedType))
    {
        const NamedType* pt = static_cast<const NamedType*>(retType);
        printStructName(pt, os, m_SymbolTable);
    }
    else if (typeid(*retType) == typeid(StructType))
    {
        const StructType* pt = static_cast<const StructType*>(retType);
        printStruct(pt, os, m_SymbolTable, "", ast);
    }
    else if (typeid(*retType) != typeid(PointerType))
        os << retType << ' ';
    else
    {
        const PointerType* pt = static_cast<const PointerType*>(retType);
        const Type* fty3 = pt->getPointsTo();
        pointerDepth = printFuncPointer(fty3, os, m_SymbolTable, ast);
        if (typeid(*fty3) == typeid(FunctionType))
        {
            ft2 = static_cast<const FunctionType*>(fty3);
            const Type* helper = ft2->getReturnType();
            while (typeid(*helper) == typeid(PointerType))
            {
                const PointerType* pt = static_cast<const PointerType*>(helper);
                helper = pt->getPointsTo();
                funcRetPointerDepth++;
            }
        }
    }
    if (ast.getContentId(node) != 0)
        os << '(' << *m_SymbolTable[ast.getContentId(node)]; // maybe remove space - yess

    printParams(ftype, os, ast, m_SymbolTable);
    os << ')';
    while (pointerDepth > 0)
    {
        os << ')';
        pointerDepth--;
    }
    if (ft2 != 0)
        printParams(ft2, os, ast, m_SymbolTable);
    while (funcRetPointerDepth > 0)
    {
        os << ")";
        funcRetPointerDepth--;
    }
}

void printStruct(const StructType* fty, std::ostream& os, const SymbolTable& m_SymbolTable,
                 std::string indent, const Ast& ast)
{
    os << "struct ";
    if (fty->getName() != 0)
        os << *m_SymbolTable[fty->getName()];
    os << "\n";
    os << indent << "{\n";
    for (unsigned long i = 0; i < fty->getMembers().size(); i++)
    {
        int flag = 1;
        os << indent << "\t";
        if (typeid(*((fty->getMembers())[i]).type) == typeid(StructType))
        {
            const StructType* fty2 = static_cast<const StructType*>(((fty->getMembers())[i]).type);
            printStruct(fty2, os, m_SymbolTable, indent + "\t", ast);
        }
        else if (typeid(*((fty->getMembers())[i]).type) == typeid(NamedType))
        {
            const NamedType* fty2 = static_cast<const NamedType*>(((fty->getMembers())[i]).type);
            printStructName(fty2, os, m_SymbolTable);
        }
        else
        {
            if (typeid(*((fty->getMembers())[i]).type) == typeid(PointerType))
            {
                const PointerType* fty2 =
                    static_cast<const PointerType*>(((fty->getMembers())[i]).type);

                printPointer(fty2, (fty->getMembers())[i].type, ((fty->getMembers())[i]).contentId,
                             os, ast, m_SymbolTable);
                flag = 0;
            }
            else
                os << ((fty->getMembers())[i]).type;
        }
        if (flag)
            os << " " << *m_SymbolTable[((fty->getMembers())[i]).contentId];
        os << ";\n";
    }
    os << indent << "}";
}

void printBlock(int i, node_t node, const Ast& ast, std::ostream& os, std::string indent,
                int indentLevel)
{
    os << "{\n";
    printNode(ast.getChildren(node)[i], indentLevel, os, ast, false, false, false, true);
    // case empty block
    if (ast.getNodeKind(ast.getChildren(node)[i]) == NodeKind::BLOCK &&
        ast.getChildren(ast.getChildren(node)[i]).size() == 0)
        os << "\r";
    else
        os << "\n";
    os << indent << "}";
}

void printNode(node_t node, int indentLevel, std::ostream& os, const Ast& ast,
               bool initBlock = false, bool braces = true, bool special = false,
               bool comingFromBlock = false)
{
    const SymbolTable& m_SymbolTable = ast.getSymbolTable();
    std::string indent(indentLevel, '\t');
    if (!special && ast.getNodeKind(node) != NodeKind::LABEL && !comingFromBlock)
        os << indent;

    switch (ast.getNodeKind(node))
    {
        case NodeKind::BINARY_ADD: printBinary("+", os, ast, node); break;
        case NodeKind::BINARY_SUB: printBinary("-", os, ast, node); break;
        case NodeKind::BINARY_MUL: printBinary("*", os, ast, node); break;
        case NodeKind::BINARY_DIV: printBinary("/", os, ast, node); break;
        case NodeKind::BINARY_REM: printBinary("%", os, ast, node); break;
        case NodeKind::BINARY_XOR: printBinary("^", os, ast, node); break;
        case NodeKind::BINARY_OR: printBinary("|", os, ast, node); break;
        case NodeKind::BINARY_AND: printBinary("&", os, ast, node); break;
        case NodeKind::BINARY_LOGICAL_LT: printBinary("<", os, ast, node); break;
        case NodeKind::BINARY_LOGICAL_LE: printBinary("<=", os, ast, node); break;
        case NodeKind::BINARY_LOGICAL_GT: printBinary(">", os, ast, node); break;
        case NodeKind::BINARY_LOGICAL_GE: printBinary(">=", os, ast, node); break;
        case NodeKind::BINARY_LOGICAL_EQ: printBinary("==", os, ast, node); break;
        case NodeKind::BINARY_LOGICAL_NEQ: printBinary("!=", os, ast, node); break;
        case NodeKind::BINARY_LOGICAL_AND: printBinary("&&", os, ast, node); break;
        case NodeKind::BINARY_LOGICAL_OR: printBinary("||", os, ast, node); break;
        case NodeKind::BINARY_SHL: printBinary("<<", os, ast, node); break;
        case NodeKind::BINARY_SHR: printBinary(">>", os, ast, node); break;
        case NodeKind::UNARY_BINARY_COMPLEMENT: printUnaryFront("~", os, ast, node); break;
        case NodeKind::UNARY_NOT: printUnaryFront("!", os, ast, node); break;
        case NodeKind::UNARY_POS: printUnaryFront("+", os, ast, node); break;
        case NodeKind::UNARY_NEG: printUnaryFront("-", os, ast, node); break;
        // case NodeKind::UNARY_DEC: printUnaryFront("--", os, ast, node); break; // front or back
        // case NodeKind::UNARY_INC: printUnaryFront("++", os, ast, node); break; // ?
        case NodeKind::UNDEFINED: break;
        case NodeKind::UNARY_SIZEOF: // (int) interpreted as a cast with empty child
        {
            size_t size = ast.getChildren(node).size();
            os << "(sizeof";
            if (size == 0)
            {
                os << "(";
                const Type* t = ast.getType(node);
                if (typeid(*t) != typeid(NamedType))
                    os << t;
                else
                {
                    const NamedType* fty = static_cast<const NamedType*>(t);
                    printStructName(fty, os, m_SymbolTable);
                }
                os << "))";
                break;
            }
            else
                os << ' ';
            printNode(ast.getChildren(node)[0], 0, os, ast);
            os << ")";
            break;
        }
        case NodeKind::ASSIGN:
        {
            os << "(";
            printNode(ast.getChildren(node)[0], 0, os, ast);
            os << " = ";
            printNode(ast.getChildren(node)[1], 0, os, ast);
            os << ")";
            break;
        }
        case NodeKind::CONDITIONAL:
        {
            os << "(";
            printNode(ast.getChildren(node)[0], 0, os, ast);
            os << " ? ";
            printNode(ast.getChildren(node)[1], 0, os, ast);
            os << " : ";
            printNode(ast.getChildren(node)[2], 0, os, ast);
            os << ")";
            break;
        }
        case NodeKind::IF:
        {
            const auto& children = ast.getChildren(node);

            bool semicol = false;
            os << "if (";
            printNode(children[0], 0, os, ast); // Condition
            os << ")";
            if ((ast.getNodeKind(children[1]) == NodeKind::BLOCK && children[1] != nullnode) ||
                (ast.getNodeKind(children[1]) == NodeKind::LABEL &&
                 ast.getNodeKind(ast.getChildren(children[1])[0]) == NodeKind ::BLOCK))
            {
                os << " ";
                printBlock(1, node, ast, os, indent, indentLevel);
            }
            else
            {
                os << "\n";
                if (children[1] != nullnode)
                    printNode(children[1], indentLevel + 1, os, ast);
                else
                    os << indent << "\t";
                if (ast.getNodeKind(children[1]) != NodeKind::LABEL &&
                    ast.getNodeKind(children[1]) != NodeKind::IF &&
                    ast.getNodeKind(children[1]) != NodeKind::IF_ELSE)
                    os << ";";
                semicol = true;
            }
            if (children.size() > 2)
            {
                if (semicol)
                {
                    os << "\n";
                    os << indent << "else";
                }
                else
                    os << " else";
                if (ast.getNodeKind(children[2]) == NodeKind::BLOCK)
                {
                    os << " ";
                    printBlock(2, node, ast, os, indent, indentLevel);
                }
                else
                {
                    if (ast.getNodeKind(children[2]) == NodeKind::IF)
                    {
                        os << " ";
                        printNode(children[2], indentLevel, os, ast, false, false, true);
                    }
                    else
                    {
                        os << "\n";
                        printNode(children[2], indentLevel + 1, os, ast);
                        os << ";";
                    }
                }
            }
            break;
        }
        case NodeKind::IF_ELSE:
        {
            const auto& children = ast.getChildren(node);

            bool semicol = false;
            os << "if (";
            printNode(children[0], 0, os, ast); // Condition
            os << ")";
            if ((ast.getNodeKind(children[1]) == NodeKind::BLOCK && children[1] != nullnode) ||
                (ast.getNodeKind(children[1]) == NodeKind::LABEL &&
                 ast.getNodeKind(ast.getChildren((children[1]))[0]) == NodeKind ::BLOCK))
            {
                os << " ";
                printBlock(1, node, ast, os, indent, indentLevel);
            }
            else
            {
                os << "\n";
                if (children[1] != nullnode)
                    printNode(children[1], indentLevel + 1, os, ast);
                else
                    os << indent << "\t";
                if (ast.getNodeKind(children[1]) != NodeKind::IF &&
                    ast.getNodeKind(children[1]) != NodeKind::IF_ELSE &&
                    ast.getNodeKind(children[1]) != NodeKind::LABEL)
                    os << ";";
                semicol = true;
            }
            if (semicol)
            {
                os << "\n";
                os << indent << "else";
            }
            else
                os << " else";
            if (ast.getNodeKind(children[2]) == NodeKind::BLOCK && children[2] != nullnode)
            {
                os << " ";
                printBlock(2, node, ast, os, indent, indentLevel);
            }
            else
            {
                if (ast.getNodeKind(children[2]) == NodeKind::IF ||
                    ast.getNodeKind(children[2]) == NodeKind::IF_ELSE)
                {
                    os << " ";
                    printNode(children[2], indentLevel, os, ast, false, false, true);
                }
                else
                {
                    os << "\n";
                    if (children[2] != nullnode)
                    {
                        printNode(children[2], indentLevel + 1, os, ast);
                        os << ";";
                    }
                    else
                        os << indent << "\t"
                           << ";";
                    ;
                }
            }
            break;
        }
        case NodeKind::BLOCK:
        {
            const auto& children = ast.getChildren(node);

            bool prevFunc = false;
            if (braces && !initBlock)
                os << "{\n";
            for (unsigned long i = 0; i < children.size(); i++)
            {
                auto kind = ast.getNodeKind(children[i]);

                if (initBlock)
                {
                    if (prevFunc)
                        os << "\n";
                    prevFunc = true;
                    printNode(children[i], indentLevel, os, ast);
                    if (kind != NodeKind::FUNCTION && kind != NodeKind::LABEL)
                        os << ';';
                }
                else
                {
                    printNode(children[i], indentLevel + 1, os, ast);
                    if (kind != NodeKind::BLOCK && kind != NodeKind::WHILE &&
                        kind != NodeKind::IF && kind != NodeKind::IF_ELSE &&
                        kind != NodeKind::LABEL)
                        os << ';';
                }
                if (i != children.size() - 1)
                    os << "\n";
            }
            if (braces && !initBlock && children.size() == 0)
                os << indent << '}';
            else if (braces && !initBlock)
                os << "\n" << indent << '}';
            break;
        }
        case NodeKind::EXTERN_DECLARATION:
        {
            os << "extern ";
            [[fallthrough]];
        }
        case NodeKind::DECLARATION:
        {
            if (typeid(*ast.getType(node)) == typeid(PointerType))
            {
                const PointerType* fty2 = static_cast<const PointerType*>(ast.getType(node));
                printPointer(fty2, ast.getType(node), ast.getContentId(node), os, ast,
                             m_SymbolTable);

                if (ast.getChildren(node).size() == 1)
                {
                    os << " = ";
                    printNode(ast.getChildren(node)[0], 0, os, ast);
                }
            }
            else
            {
                if (typeid(*ast.getType(node)) == typeid(StructType))
                {
                    const StructType* fty = static_cast<const StructType*>(ast.getType(node));
                    printStruct(fty, os, m_SymbolTable, indent, ast);
                }
                else if (typeid(*ast.getType(node)) == typeid(NamedType))
                {
                    const NamedType* fty = static_cast<const NamedType*>(ast.getType(node));
                    printStructName(fty, os, m_SymbolTable);
                }
                else if (typeid(*ast.getType(node)) == typeid(FunctionType))
                {
                    const FunctionType* fty = static_cast<const FunctionType*>(ast.getType(node));
                    printFuncType(fty, node, os, ast, m_SymbolTable);
                    break;
                }
                else
                    os << ast.getType(node);
                os << " ";
                os << *m_SymbolTable[ast.getContentId(node)];
                if (ast.getChildren(node).size() == 1)
                {
                    os << " = ";
                    printNode(ast.getChildren(node)[0], 0, os, ast);
                }
            }
            break;
        }
        case NodeKind::WHILE:
        {
            const auto& children = ast.getChildren(node);

            os << "while (";
            printNode(children[0], 0, os, ast); // Condition
            os << ")";
            if (children.size() == 2)
            {
                if (children[1] == nullnode)
                {
                    os << "\n";
                    os << indent << "\t;";
                }
                else if (ast.getNodeKind(children[1]) == NodeKind::BLOCK ||
                         (ast.getNodeKind(children[1]) == NodeKind::LABEL &&
                          ast.getNodeKind(ast.getChildren(children[1])[0]) == NodeKind ::BLOCK))
                {
                    os << " ";
                    printBlock(1, node, ast, os, indent, indentLevel);
                }
                // case while with no body just statement
                else
                {
                    os << "\n";
                    printNode(children[1], indentLevel + 1, os, ast);
                    if (ast.getNodeKind(children[1]) != NodeKind::LABEL)
                        os << ";";
                }
            }
            break;
        }
        case NodeKind::INDIR: printUnaryFront("*", os, ast, node); break;
        case NodeKind::ADDR: printUnaryFront("&", os, ast, node); break;
        case NodeKind::CONSTANT: os << *m_SymbolTable[ast.getContentId(node)]; break;
        case NodeKind::VAR: os << *m_SymbolTable[ast.getContentId(node)]; break;
        case NodeKind::STRING_LITERAL:
            os << "\"" << *m_SymbolTable[ast.getContentId(node)] << "\"";
            break;
        case NodeKind::RETURN:
        {
            os << "return";
            if (ast.getChildren(node).size() > 0)
            {
                os << " ";
                printNode(ast.getChildren(node)[0], 0, os, ast);
            }

            break;
        }
        case NodeKind::EXPR_STATEMENT:
        {
            printNode(ast.getChildren(node)[0], 0, os, ast);
            break;
        }
        case NodeKind::CONTINUE: os << "continue"; break;
        case NodeKind::BREAK: os << "break"; break;
        case NodeKind::FUNCTION:
        {
            const auto& children = ast.getChildren(node);
            // type wrong
            // printType(ast.getAtt(ast.getType(node)).getChildren()[ast.getChildren(node).size()
            // - 1],
            //          os, ast);
            const FunctionType* ftype = dynamic_cast<const FunctionType*>(ast.getType(node));
            auto retType = ftype->getReturnType();
            int pointerDepth = 0;
            if (typeid(*retType) == typeid(StructType))
            {
                os << "struct ";
                const StructType* stype = dynamic_cast<const StructType*>(retType);
                if (stype->getName() != 0)
                    os << *m_SymbolTable[stype->getName()];
                os << ' ';
            }
            else if (typeid(*retType) == typeid(NamedType))
            {
                const NamedType* ntype = dynamic_cast<const NamedType*>(retType);
                printStructName(ntype, os, m_SymbolTable);
                os << ' ';
            }
            else if (typeid(*retType) != typeid(PointerType))
                os << retType << ' ';
            else
            {
                const PointerType* pt = static_cast<const PointerType*>(retType);
                const Type* fty3 = pt->getPointsTo();
                pointerDepth = printFuncPointer(fty3, os, m_SymbolTable, ast);
            }
            os << '(' << *m_SymbolTable[ast.getContentId(node)];
            os << '(';

            const auto& params = ftype->getParameters();
            // overflow = (ast.getChildren(node).size() - 2 > ast.getChildren(node).size())
            //                ? 0
            //                : ast.getChildren(node).size() - 2;
            if (params.size() == 0)
                os << "void";

            for (uint32_t i = 0; i < params.size(); i++)
            {
                const Type* const type = params[i].type;
                const node_t identifier = params[i].contentId;
                int pointerDepth = 0;
                // if (isPointerToFunc(type))
                //     pointerDepth = printFuncPointer(type, os, m_SymbolTable);
                if (typeid(*type) == typeid(PointerType))
                {
                    const PointerType* fty2 = static_cast<const PointerType*>(type);
                    printPointer(fty2, type, identifier, os, ast, m_SymbolTable);
                }
                else if (typeid(*type) == typeid(NamedType))
                {
                    const NamedType* fty = static_cast<const NamedType*>(type);
                    printStructName(fty, os, m_SymbolTable);
                    if (identifier != 0)
                    {
                        os << ' ';
                        // get name of param
                        os << *m_SymbolTable[identifier];
                    }
                }
                else if (typeid(*type) == typeid(StructType))
                {
                    const StructType* stype = dynamic_cast<const StructType*>(type);
                    printStruct(stype, os, m_SymbolTable, "", ast);
                    if (identifier != 0)
                    {
                        os << ' ';
                        // get name of param
                        os << *m_SymbolTable[identifier];
                    }
                }
                else
                {
                    os << type;
                    if (identifier != 0)
                    {
                        os << ' ';
                        // get name of param
                        os << *m_SymbolTable[identifier];
                    }
                }
                while (pointerDepth > 0)
                {
                    os << ')';
                    pointerDepth--;
                }
                // only print comma when we are not at the last element
                if (i + 1 < params.size())
                    os << ", ";
            }

            os << "))";
            while (pointerDepth > 0)
            {
                os << ')';
                pointerDepth--;
            }
            os << '\n';

            printNode(children[children.size() - 1], indentLevel, os, ast);

            break;
        }
        case NodeKind::CALL_EXPRESSION:
        {
            const auto& children = ast.getChildren(node);
            os << '(';
            // if (ast.getContentId(children[children.size() - 1]) != 0)
            //     os << *m_SymbolTable[ast.getContentId(children[children.size() - 1])];
            // else
            printNode(children[children.size() - 1], 0, os, ast);
            os << '(';

            // what exactly is happenening here?

            long size = children.size();
            long overflow = size - 1 > size ? 0 : size - 1;

            for (long i = 0; i < overflow; ++i)
            {
                printNode(children[i], 0, os, ast);

                if (i != overflow - 1)
                    os << ", ";
            }

            os << "))";
            break;
        }
        case NodeKind::GOTO:
        {
            // something going wrong
            os << "goto ";
            os << *m_SymbolTable[ast.getContentId(node)];
            break;
        }
        case NodeKind::ARRAY_ACCESS:
        {
            const auto& children = ast.getChildren(node);
            os << "(";
            printNode(children[0], 0, os, ast);
            os << "[";
            printNode(children[1], 0, os, ast);
            os << "]";
            os << ")";
            break;
        }
        case NodeKind::LABEL:
        {
            os << *m_SymbolTable[ast.getContentId(node)];
            os << ":\n";

            const node_t child = ast.getChildren(node)[0];
            printNode(child, indentLevel, os, ast);
            if (ast.getNodeKind(child) != NodeKind::LABEL &&
                ast.getNodeKind(child) != NodeKind::BLOCK &&
                ast.getNodeKind(child) != NodeKind::IF_ELSE &&
                ast.getNodeKind(child) != NodeKind::IF)
                os << ';';
            break;
        }
        case NodeKind::CAST:
        {
            os << "((";
            const Type* t = ast.getType(node);
            if (typeid(*t) == typeid(NamedType) || typeid(*t) == typeid(StructType))
            {
                const NamedType* fty = static_cast<const NamedType*>(t);
                printStructName(fty, os, m_SymbolTable);
            }
            else if (typeid(*t) == typeid(PointerType))
            {
                const PointerType* pty = static_cast<const PointerType*>(t);
                printPointer(pty, t, 0, os, ast, m_SymbolTable);
            }
            else
                os << t;
            os << ")";
            printNode(ast.getChildren(node)[0], 0, os, ast);
            os << ")";
            break;
        }
        case NodeKind::STRUCT_DECL:
        {
            if (typeid(*ast.getType(node)) == typeid(StructType))
            {
                const StructType* fty = static_cast<const StructType*>(ast.getType(node));
                printStruct(fty, os, m_SymbolTable, indent, ast);
            }
            else if (typeid(*ast.getType(node)) == typeid(NamedType))
            {
                const NamedType* fty = static_cast<const NamedType*>(ast.getType(node));
                printStructName(fty, os, m_SymbolTable);
            }

            break;
        }
        case NodeKind::DOT_ACCESS:
        {
            os << "(";
            printNode(ast.getChildren(node)[0], 0, os, ast);
            os << "." << *m_SymbolTable[ast.getContentId(node)];
            os << ")";
            break;
        }
        case NodeKind::ARROW_ACCESS:
        {
            os << "(";
            printNode(ast.getChildren(node)[0], 0, os, ast);
            os << "->" << *m_SymbolTable[ast.getContentId(node)];
            os << ")";
            break;
        }
        case NodeKind::EMPTY: os << "";
        case NodeKind::TYPE_HOLDER: break;
        case NodeKind::POINTER_ADD_OFFSET: break;
        case NodeKind::POINTER_SUB_OFFSET: break;
        case NodeKind::POINTER_DIFF: break;
    }
}

std::ostream& operator<<(std::ostream& os, const Ast& ast)
{
    printNode(ast.getOrigin(), 0, os, ast, true);
    return os;
}
