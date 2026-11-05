#include "SemanticAnalysis.h"
#include "Diagnostics.h"
#include "NodeUtils.h"
#include "SymbolTable.h"

#include <cassert>
#include <unordered_set>

bool checkExistsInScopes(Env e, uint32_t name)
{
    Env temp = e;
    while (!temp.empty())
    {
        if (temp.top().count(name))
        {
            return true;
        }
        temp.pop();
    }
    return false;
}

void typeCheckStruct(const Type* t, Env& structs, Locator m_Locator, uint32_t offset, Ast& m_Ast)
{
    StructType* sty = const_cast<StructType*>(static_cast<const StructType*>(t));
    uint32_t strName = sty->getName();
    const auto location = m_Locator.getLocationByOffset(offset);
    std::unordered_set<uint32_t> mems = {};
    if (typeid(*t) == typeid(StructType))
    {
        if (structs.top().count(strName))
            diagnostics::errorloc(location, "Struct type already declared");
        nary_members_t& members = sty->getMembers();

        for (size_t i = 0; i < members.size(); i++)
        {
            // segFault TODO
            TypeMap tm = members[i].type->getTypeMap();
            // A structure or union shall not contain a member with incomplete or function -
            // p113 type
            if (tm == TypeMap::FUNCTION_TYPE)
                diagnostics::errorloc(location, "Illegal func type member");
            if (typeid(*members[i].type) == typeid(NamedType))
            {
                const auto offsetPar = members[i].offset;
                const auto locationPar = m_Locator.getLocationByOffset(offsetPar);
                const NamedType* nty = static_cast<const NamedType*>(members[i].type);
                if (!checkExistsInScopes(structs, nty->getName()))
                    diagnostics::errorloc(locationPar, "Illegal incompl struct type member");
                uint32_t strName = nty->getName();
                Env temp = structs;
                while (!temp.empty())
                {
                    if (temp.top().count(strName))
                    {
                        MemberField& member = members[i];
                        member.setType(temp.top()[strName]);
                        break;
                    }
                    temp.pop();
                }
            }
            else if (typeid(*members[i].type) == typeid(StructType))
            {
                const StructType* sty = static_cast<const StructType*>(members[i].type);
                structs.top().insert({sty->getName(), members[i].type});
            }
            else if (isPointerToStruct(members[i].type))
            {
                const PointerType* pt = static_cast<const PointerType*>(members[i].type);
                const Type* ppt = pt->getPointsTo();
                int pDepth = 1;
                while (typeid(*ppt) == typeid(PointerType))
                {
                    const PointerType* hpt = static_cast<const PointerType*>(ppt);
                    ppt = hpt->getPointsTo();
                    pDepth++;
                }
                if (typeid(*ppt) == typeid(StructType))
                {
                    typeCheckStruct(ppt, structs, m_Locator, offset, m_Ast);
                }
                if (typeid(*ppt) == typeid(NamedType))
                {
                    const NamedType* nty = static_cast<const NamedType*>(ppt);
                    uint32_t strName = nty->getName();
                    Env temp = structs;
                    while (!temp.empty())
                    {
                        if (temp.top().count(strName))
                        {
                            ppt = temp.top()[strName];
                            break;
                        }
                        temp.pop();
                    }
                }
                while (pDepth > 0)
                {
                    ppt = m_Ast.createPointerType(ppt);
                    pDepth--;
                }
                MemberField& member = members[i];
                member.setType(ppt);
            }
            // Struct members must have unique names.
            if (mems.count(members[i].contentId))
            {
                const auto location = m_Locator.getLocationByOffset(members[i].offset);
                diagnostics::errorloc(location, "Member already declared");
            }
            mems.insert(members[i].contentId);
        }
        structs.top().insert({strName, static_cast<const Type*>(t)});
    }
}

bool compStructs(const Type* t1, const Type* t2, Env structs)
{
    uint32_t name1 = 0;
    uint32_t name2 = 0;
    if (typeid(*t1) == typeid(NamedType))
    {
        const NamedType* nt = static_cast<const NamedType*>(t1);
        name1 = nt->getName();
    }
    if (typeid(*t1) == typeid(StructType))
    {
        const StructType* nt = static_cast<const StructType*>(t1);
        name1 = nt->getName();
    }
    if (typeid(*t2) == typeid(StructType))
    {
        const StructType* nt = static_cast<const StructType*>(t2);
        name2 = nt->getName();
    }
    if (typeid(*t2) == typeid(NamedType))
    {
        const NamedType* nt = static_cast<const NamedType*>(t2);
        name2 = nt->getName();
    }
    if (name2 != 0 && name2 == name1)
    {
        while (!structs.empty())
        {
            if (structs.top().count(name2))
            {
                return true;
            }
            structs.pop();
        }
    }
    return false;
}

void SemanticAnalysis::typeCheckStatement(node_t node, Env& vars, Env& functions,
                                          Env& functionDecls, Env& structs, LabEnv& labelsUsed,
                                          const Type* ftype, bool insideLoop, Scope funcArg,
                                          bool global)
{
    // const SymbolTable& m_SymbolTable = m_Ast.getSymbolTable();
    auto ctx = Context(vars, functions, functionDecls, structs, labelsUsed);
    const auto offset = m_Ast.getOffset(node);
    const auto location = m_Locator.getLocationByOffset(offset);

    switch (m_Ast.getNodeKind(node))
    {
        case NodeKind::EXPR_STATEMENT:
        {
            typeCheckExpression(m_Ast.getChildren(node)[0], ctx);
            typeCheckStatement(m_Ast.getChildren(node)[0], vars, functions, functionDecls, structs,
                               labelsUsed, ftype, insideLoop, funcArg);
            break;
        }
        // done
        case NodeKind::IF:
        {
            const auto& children = m_Ast.getChildren(node);
            const Type* t1 = typeCheckExpression(children[0], ctx);

            typeCheckStatement(children[1], vars, functions, functionDecls, structs, labelsUsed,
                               ftype, insideLoop);

            if (!isOfTypeMap(t1->getTypeMap(), TypeMap::SCALAR_TYPE))
                diagnostics::errorloc(location, "If wrong type ");

            break;
        }
        // done
        case NodeKind::IF_ELSE:
        {
            const auto& children = m_Ast.getChildren(node);

            const Type* t1 = typeCheckExpression(children[0], ctx);

            typeCheckStatement(children[1], vars, functions, functionDecls, structs, labelsUsed,
                               ftype, insideLoop);
            typeCheckStatement(children[2], vars, functions, functionDecls, structs, labelsUsed,
                               ftype, insideLoop);

            if (!isOfTypeMap(t1->getTypeMap(), TypeMap::SCALAR_TYPE))
                diagnostics::errorloc(location, "If wrong type ");

            break;
        }
        // done
        case NodeKind::WHILE:
        {
            const auto& children = m_Ast.getChildren(node);
            const Type* t1 = typeCheckExpression(children[0], ctx);
            if (t1 != 0)
            {
                if (!isOfTypeMap(t1->getTypeMap(), TypeMap::SCALAR_TYPE))
                    diagnostics::errorloc(location, "While wrong type ");

                typeCheckStatement(children[1], vars, functions, functionDecls, structs, labelsUsed,
                                   ftype, true);
            }
            break;
        }
        // done
        case NodeKind::BLOCK:
        {
            Scope innerScopeVars = funcArg;
            Scope innerScopeFun = {};
            Scope innerScopeFunDecl = {};
            Scope structsScope = {};
            vars.push(innerScopeVars);
            functions.push(innerScopeFun);
            functionDecls.push(innerScopeFunDecl);
            structs.push(structsScope);

            const auto& children = m_Ast.getChildren(node);

            for (size_t i = 0; i < children.size(); i++)
                typeCheckStatement(children[i], vars, functions, functionDecls, structs, labelsUsed,
                                   ftype, insideLoop, {}, global);

            vars.pop();
            functions.pop();
            functionDecls.pop();
            structs.pop();
            break;
        }
        // done hopefully
        case NodeKind::GOTO:
        {
            uint32_t name = m_Ast.getContentId(node);
            LabEnv temp = labelsUsed;
            bool flag = false;
            while (!temp.empty())
            {
                if (temp.top().count(name))
                {
                    flag = true;
                    break;
                }
                temp.pop();
            }
            if (flag)
                break;
            labelsSeen.insert({name, node});
            break;
        }
        // done
        case NodeKind::LABEL:
        {
            // Label names shall be unique within a function.
            uint32_t name = m_Ast.getContentId(node);
            if (labelsUsed.top().count(name))
                diagnostics::errorloc(location, "Label already exists in the function");
            if (labelsSeen.count(name))
                labelsSeen.erase(name);
            labelsUsed.top().insert({name});
            // check child
            typeCheckStatement(m_Ast.getChildren(node)[0], vars, functions, functionDecls, structs,
                               labelsUsed, ftype, insideLoop);
            // propagate insideLoop
            break;
        }
        // done
        case NodeKind::CONTINUE:
        case NodeKind::BREAK:
        {
            // only inside of a loop
            if (!insideLoop)
                diagnostics::errorloc(location, "Continue/Break not in loop");

            break;
        }
        // done
        case NodeKind::RETURN:
        {
            if (m_Ast.getChildren(node).size() > 0)
            {
                const Type* t1;
                if (isPointerToFunc(ftype))
                    t1 = typeCheckExpression(m_Ast.getChildren(node)[0], ctx, true);
                else
                    t1 = typeCheckExpression(m_Ast.getChildren(node)[0], ctx);
                if (t1->getTypeMap() == TypeMap::INT_TYPE &&
                    ftype->getTypeMap() == TypeMap::POINTER_TYPE &&
                    m_Ast.getNodeKind(m_Ast.getChildren(node)[0]) == NodeKind::CONSTANT)
                {
                    const std::string_view view =
                        *m_Ast.getSymbolTable()[m_Ast.getContentId(m_Ast.getChildren(node)[0])];
                    if (view == "0")
                        break;
                }
                if (isPointerToFunc(ftype) && ftype->getTypeMap() == TypeMap::POINTER_TYPE)
                {
                    const PointerType* pt1 = static_cast<const PointerType*>(ftype);
                    if (isCompatibleType(pt1->getPointsTo(), t1) ||
                        isCompatibleType(ftype, t1, true))
                        break;
                }
                // check compatible types
                if (!isCompatibleType(ftype, t1) || ftype->getTypeMap() == TypeMap::VOID_TYPE)
                {
                    diagnostics::errorloc(location, "Return wrong type ");
                }
            }
            else
            {
                if (ftype->getTypeMap() != TypeMap::VOID_TYPE)
                {
                    // void type
                    diagnostics::errorloc(location, "Return wrong type ");
                }
            }
            break;
        }
        case NodeKind::FUNCTION:
        {
            const FunctionType* ftype = dynamic_cast<const FunctionType*>(m_Ast.getType(node));
            uint32_t name = m_Ast.getContentId(node);
            if (functions.top().count(name) != 0)
                diagnostics::errorloc(location, "Func already exists");
            if (functionDecls.top().count(name) != 0)
            {
                const Type* tgot = functionDecls.top().find(name)->second;
                const FunctionType* fty2 = static_cast<const FunctionType*>(tgot);
                if (!(*fty2 == *ftype))
                    diagnostics::errorloc(location, "Func decl with this name already exists");
            }
            functions.top().insert({name, static_cast<const Type*>(ftype)});

            const Type* rt = ftype->getReturnType();
            if (!isOfTypeMap(rt->getTypeMap(), TypeMap::OBJECT_TYPE) &&
                rt->getTypeMap() != TypeMap::VOID_TYPE)
                diagnostics::errorloc(location, "Return type wrong");
            const auto& params = ftype->getParameters();
            Scope funcA = {};
            for (uint32_t i = 0; i < params.size(); i++)
            {
                const auto offsetPar = params[i].offset;
                const auto locationPar = m_Locator.getLocationByOffset(offsetPar);
                const Type* type = params[i].type;
                const node_t identifier = params[i].contentId;
                if (identifier == 0)
                    diagnostics::errorloc(locationPar, "Arguments should be named");
                if (funcA.count(identifier))
                    diagnostics::errorloc(locationPar, "Argument already exists");
                if (typeid(*type) == typeid(NamedType))
                {
                    const NamedType* sty = static_cast<const NamedType*>(type);
                    uint32_t strName = sty->getName();
                    Env temp = structs;
                    while (!temp.empty())
                    {
                        if (temp.top().count(strName))
                        {
                            type = temp.top()[strName];
                            break;
                        }
                        temp.pop();
                    }
                }
                if (typeid(*type) == typeid(PointerType) && isPointerToStruct(type))
                {
                    const PointerType* pt = static_cast<const PointerType*>(type);
                    const Type* ppt = pt->getPointsTo();
                    int pDepth = 1;
                    while (typeid(*ppt) == typeid(PointerType))
                    {
                        const PointerType* hpt = static_cast<const PointerType*>(ppt);
                        ppt = hpt->getPointsTo();
                        pDepth++;
                    }
                    if (typeid(*ppt) == typeid(StructType))
                    {
                        typeCheckStruct(ppt, structs, m_Locator, offset, m_Ast);
                    }
                    if (typeid(*ppt) == typeid(NamedType))
                    {
                        const NamedType* nty = static_cast<const NamedType*>(ppt);
                        uint32_t strName = nty->getName();
                        Env temp = structs;
                        while (!temp.empty())
                        {
                            if (temp.top().count(strName))
                            {
                                ppt = temp.top()[strName];
                                break;
                            }
                            temp.pop();
                        }
                    }
                    while (pDepth > 0)
                    {
                        ppt = m_Ast.createPointerType(ppt);
                        pDepth--;
                    }
                    type = ppt;
                }
                funcA.insert({identifier, type});
            }
            LabScope ns = {};
            labelsUsed.push(ns);
            // pass return type of the function
            typeCheckStatement(m_Ast.getChildren(node)[0], vars, functions, functionDecls, structs,
                               labelsUsed, rt, insideLoop, funcA);
            labelsUsed.pop();
            break;
        }
        // done
        case NodeKind::EXTERN_DECLARATION:
        case NodeKind::DECLARATION:
        {
            uint32_t name = m_Ast.getContentId(node);
            const Type* t = m_Ast.getType(node);
            if (typeid(*t) == typeid(StructType))
            {
                if (vars.top().count(name))
                    diagnostics::errorloc(location, "Var already declared");
                vars.top().insert({name, static_cast<const Type*>(t)});

                goto typeCheckStruct;
            }
            else if (typeid(*t) == typeid(NamedType))
            {
                const NamedType* sty = static_cast<const NamedType*>(t);
                uint32_t strName = sty->getName();
                const Type* strType = 0;
                Env temp = structs;
                bool flag = false;
                while (!temp.empty())
                {
                    if (temp.top().count(strName))
                    {
                        strType = temp.top()[strName];
                        flag = true;
                        break;
                    }
                    temp.pop();
                }
                if (!flag)
                    diagnostics::errorloc(location, "No such struct: ");
                // if (structs.top().count(strName))
                if (vars.top().count(name))
                    diagnostics::errorloc(location, "Var already declared");
                vars.top().insert({name, static_cast<const Type*>(strType)});
            }
            else if (typeid(*t) == typeid(FunctionType) || isPointerToFunc(t))
            {
                const Type* originalType = t;
                while (typeid(*t) == typeid(PointerType))
                {
                    const PointerType* pty = static_cast<const PointerType*>(t);
                    t = pty->getPointsTo();
                    if (typeid(*t) != typeid(FunctionType) && typeid(*t) != typeid(PointerType))
                        break;
                }
                const FunctionType* fty = static_cast<const FunctionType*>(t);
                if (functions.top().count(name) != 0)
                {
                    const Type* tgot = functions.top().find(name)->second;
                    const FunctionType* fty2 = static_cast<const FunctionType*>(tgot);
                    if (!(*fty == *fty2))
                        diagnostics::errorloc(location, "Func defintion with diff type exists");
                }
                // should be fine because we do not have inner func dec
                // if (vars.top().count(name))
                //     vars.top().erase(name);
                if (functionDecls.top().count(name) == 0)
                {
                    functionDecls.top().insert({name, static_cast<const Type*>(originalType)});
                    break;
                }
                const Type* tgot = functionDecls.top().find(name)->second;
                const FunctionType* fty2 = static_cast<const FunctionType*>(tgot);
                if (*fty == *fty2)
                    break;
                diagnostics::errorloc(location, "Func decl with diff type exists");
            }
            else
            {
                if (isPointerToStruct(t))
                {
                    const PointerType* pt = static_cast<const PointerType*>(t);
                    const Type* ppt = pt->getPointsTo();
                    int pointerDepth = 1;
                    while (typeid(*ppt) == typeid(PointerType))
                    {
                        const PointerType* hpt = static_cast<const PointerType*>(ppt);
                        ppt = hpt->getPointsTo();
                        pointerDepth++;
                    }
                    if (typeid(*ppt) == typeid(StructType))
                    {
                        typeCheckStruct(ppt, structs, m_Locator, offset, m_Ast);
                    }
                    if (typeid(*ppt) == typeid(NamedType))
                    {
                        const NamedType* sty = static_cast<const NamedType*>(ppt);
                        uint32_t strName = sty->getName();
                        const Type* strType = 0;
                        Env temp = structs;
                        while (!temp.empty())
                        {
                            if (temp.top().count(strName))
                            {
                                strType = temp.top()[strName];
                                break;
                            }
                            temp.pop();
                        }
                        if (strType != 0)
                        {
                            const Type* toRet = strType;
                            while (pointerDepth > 0)
                            {
                                toRet = m_Ast.createPointerType(toRet);
                                pointerDepth--;
                            }
                            if (vars.top().count(name))
                                diagnostics::errorloc(location, "Var already declared");
                            vars.top().insert({name, toRet}); // add var to the env
                            break;
                        }
                    }
                }
                if (global && vars.top().count(name))
                {
                    const Type* tgot = vars.top().find(name)->second;
                    if (typeid(*t) != typeid(*tgot) && t->getTypeMap() != tgot->getTypeMap())
                        diagnostics::errorloc(location, "Global var with diff type");
                    break;
                }
                if (vars.top().count(name))
                    diagnostics::errorloc(location, "Var already declared");
                // if (functionDecls.top().count(name))
                //     functionDecls.top().erase(name);
                vars.top().insert({name, static_cast<const Type*>(t)}); // add var to the env
            }
            break;
        }
        // done
        case NodeKind::STRUCT_DECL:
        {
        typeCheckStruct:
            const Type* t = m_Ast.getType(node);
            typeCheckStruct(t, structs, m_Locator, offset, m_Ast);
        }
        default: break;
    }
}

const Type* SemanticAnalysis::checkBinarySub(node_t node, Context& ctx, bool func)
{
    // two pointers of same type -> result pointer
    // pointer and intger -> result integer

    const auto& children = m_Ast.getChildren(node);

    const Type* t1 = typeCheckExpression(children[0], ctx, func);
    const Type* t2 = typeCheckExpression(children[1], ctx, func);

    // both operands have arithmetic type;
    if (isOfTypeMap(t1->getTypeMap(), TypeMap::INTEGER_TYPE) &&
        isOfTypeMap(t2->getTypeMap(), TypeMap::INTEGER_TYPE))
    {
        const Type* ti = m_Ast.getIntType();
        m_Ast.setType(node, ti);
        return ti;
    }

    // both operands are pointers to qualified or unqualified versions of compatible complete object
    // types;
    else if ((t2->getTypeMap() == TypeMap::POINTER_TYPE) &&
             t1->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        const PointerType* pt1 = static_cast<const PointerType*>(t1);
        const PointerType* pt2 = static_cast<const PointerType*>(t2);
        if (isCompatibleType(t1, t2, true) &&
            isOfTypeMap(pt1->getPointsTo()->getTypeMap(), TypeMap::COMPLETE_TYPE) &&
            isOfTypeMap(pt2->getPointsTo()->getTypeMap(), TypeMap::COMPLETE_TYPE))
        {
            const Type* ti = m_Ast.getIntType();
            m_Ast.setType(node, ti);
            m_Ast.setNodeKind(node, NodeKind::POINTER_DIFF);
            return ti;
        }
    }

    // the left operand is a pointer to a complete object type and the right operand has integer
    // type.
    else if (t1->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        const PointerType* pt1 = static_cast<const PointerType*>(t1);
        if (isOfTypeMap(t2->getTypeMap(), TypeMap::INTEGER_TYPE) &&
            isOfTypeMap(pt1->getPointsTo()->getTypeMap(), TypeMap::COMPLETE_TYPE))
        {
            m_Ast.setType(node, t1);
            m_Ast.setNodeKind(node, NodeKind::POINTER_SUB_OFFSET);
            return t1;
        }
    }
    auto offset = m_Ast.getOffset(node);
    auto location = m_Locator.getLocationByOffset(offset);

    diagnostics::errorloc(location, "BinSub wrong type");
}

const Type* SemanticAnalysis::checkBinaryAdd(node_t node, Context& ctx, bool func)
{
    const auto& children = m_Ast.getChildren(node);

    const Type* t1 = typeCheckExpression(children[0], ctx, func);
    const Type* t2 = typeCheckExpression(children[1], ctx, func);

    if (isOfTypeMap(t1->getTypeMap(), TypeMap::INTEGER_TYPE))
    {
        if (isOfTypeMap(t2->getTypeMap(), TypeMap::INTEGER_TYPE))
        {
            const Type* ti = m_Ast.getIntType();
            m_Ast.setType(node, ti);
            return ti;
        }
        if (t2->getTypeMap() == TypeMap::POINTER_TYPE)
        {
            m_Ast.setType(node, t2);
            m_Ast.setNodeKind(node, NodeKind::POINTER_ADD_OFFSET);
            return t2;
        }
    }
    else if (t1->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        const PointerType* pt1 = static_cast<const PointerType*>(t1);
        if (isOfTypeMap(t2->getTypeMap(), TypeMap::INTEGER_TYPE) &&
            pt1->getPointsTo()->getTypeMap() != TypeMap::VOID_TYPE)
        {
            m_Ast.setType(node, t1);
            m_Ast.setNodeKind(node, NodeKind::POINTER_ADD_OFFSET);
            return t1;
        }
    }

    auto offset = m_Ast.getOffset(node);
    auto location = m_Locator.getLocationByOffset(offset);

    diagnostics::errorloc(location, "Binary + wrong type");
}

const Type* SemanticAnalysis::checkBinaryMul(node_t node, Context& ctx, bool func)
{
    // arithmetic type
    const auto& children = m_Ast.getChildren(node);

    const Type* t1 = typeCheckExpression(children[0], ctx, func);
    const Type* t2 = typeCheckExpression(children[1], ctx, func);

    if (isOfTypeMap(t1->getTypeMap(), TypeMap::INTEGER_TYPE) &&
        isOfTypeMap(t2->getTypeMap(), TypeMap::INTEGER_TYPE))
    {
        const Type* ti = m_Ast.getIntType();
        m_Ast.setType(node, ti);
        return ti;
    }

    auto offset = m_Ast.getOffset(node);
    auto location = m_Locator.getLocationByOffset(offset);

    diagnostics::errorloc(location, "Binary */ wrong type");
}

const Type* SemanticAnalysis::checkBinaryXor(node_t node, Context& ctx, bool func)
{
    // integer type
    const auto& children = m_Ast.getChildren(node);

    const Type* t1 = typeCheckExpression(children[0], ctx, func);
    const Type* t2 = typeCheckExpression(children[1], ctx, func);

    if (isOfTypeMap(t1->getTypeMap(), TypeMap::INTEGER_TYPE) &&
        isOfTypeMap(t2->getTypeMap(), TypeMap::INTEGER_TYPE))
    {
        const Type* ti = m_Ast.getIntType();
        m_Ast.setType(node, ti);
        return ti;
    }

    auto offset = m_Ast.getOffset(node);
    auto location = m_Locator.getLocationByOffset(offset);

    diagnostics::errorloc(location, "binary op wrong type");
}

const Type* SemanticAnalysis::checkBinaryAnd(node_t node, Context& ctx, bool func)
{
    // Each of the operands shall have scalar type
    // return Int
    const auto& children = m_Ast.getChildren(node);

    const Type* t1 = typeCheckExpression(children[0], ctx, func);
    const Type* t2 = typeCheckExpression(children[1], ctx, func);

    if (isOfTypeMap(t1->getTypeMap(), TypeMap::SCALAR_TYPE) &&
        isOfTypeMap(t2->getTypeMap(), TypeMap::SCALAR_TYPE))
    {
        const Type* ti = m_Ast.getIntType();
        m_Ast.setType(node, ti);
        return ti;
    }

    auto offset = m_Ast.getOffset(node);
    auto location = m_Locator.getLocationByOffset(offset);

    diagnostics::errorloc(location, "Binary and/or wrong type");
}

const Type* SemanticAnalysis::checkBinaryEqNeq(node_t node, Context& ctx, bool func)
{
    const auto& children = m_Ast.getChildren(node);

    const Type* t1 = typeCheckExpression(children[0], ctx, func);
    const Type* t2 = typeCheckExpression(children[1], ctx, func);
    // both operands have arithmetic type;
    if (isOfTypeMap(t1->getTypeMap(), TypeMap::INTEGER_TYPE) &&
        isOfTypeMap(t2->getTypeMap(), TypeMap::INTEGER_TYPE))
    {
        const Type* ti = m_Ast.getIntType();
        m_Ast.setType(node, ti);
        return ti;
    }
    // nullPtr and a pointer
    if (t2->getTypeMap() == TypeMap::POINTER_TYPE && t1->getTypeMap() == TypeMap::INT_TYPE &&
        m_Ast.getNodeKind(children[0]) == NodeKind::CONSTANT &&
        *m_Ast.getSymbolTable()[m_Ast.getContentId(children[0])] == "0")
    {
        const Type* ti = m_Ast.getIntType();
        m_Ast.setType(node, ti);
        return ti;
    }
    // TODO: temporary fix
    if (t1->getTypeMap() == TypeMap::POINTER_TYPE && t2->getTypeMap() == TypeMap::INT_TYPE &&
        m_Ast.getNodeKind(children[1]) == NodeKind::CONSTANT &&
        *m_Ast.getSymbolTable()[m_Ast.getContentId(children[1])] == "0")
    {
        const Type* ti = m_Ast.getIntType();
        m_Ast.setType(node, ti);
        return ti;
    }
    // func pointers
    if (isPointerToFunc(t1) && isPointerToFunc(t2))
    {
        const Type* ti = m_Ast.getIntType();
        m_Ast.setType(node, ti);
        return ti;
    }
    // — both operands are pointers to qualified or unqualified versions of compatible
    // types;
    if (t2->getTypeMap() == TypeMap::POINTER_TYPE && t1->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        const PointerType* pt1 = static_cast<const PointerType*>(t1);
        const PointerType* pt2 = static_cast<const PointerType*>(t2);
        if (isCompatibleType(t1, t2, true) &&
            isOfTypeMap(pt1->getPointsTo()->getTypeMap(), TypeMap::OBJECT_TYPE) &&
            isOfTypeMap(pt2->getPointsTo()->getTypeMap(), TypeMap::OBJECT_TYPE))
        {
            const Type* ti = m_Ast.getIntType();
            m_Ast.setType(node, ti);
            return ti;
        }
        // — one operand is a pointer to an object type and the other is a pointer to
        //  a qualified or unqualified version of void;
        // const PointerType* pt2 = static_cast<const PointerType*>(t2);
        // const PointerType* pt3 = static_cast<const PointerType*>(t1);
        if (pt2->getPointsTo()->getTypeMap() == TypeMap::VOID_TYPE &&
            isOfTypeMap(pt1->getPointsTo()->getTypeMap(), TypeMap::OBJECT_TYPE))
        {
            const Type* ti = m_Ast.getIntType();
            m_Ast.setType(node, ti);
            return ti;
        }
        if (pt1->getPointsTo()->getTypeMap() == TypeMap::VOID_TYPE &&
            isOfTypeMap(pt2->getPointsTo()->getTypeMap(), TypeMap::OBJECT_TYPE))
        {
            const Type* ti = m_Ast.getIntType();
            m_Ast.setType(node, ti);
            return ti;
        }
        // or — one operand is a pointer and the other is a null pointer constant.
        if (pt1->getTypeMap() == TypeMap::NULLPTR_TYPE ||
            pt2->getTypeMap() == TypeMap::NULLPTR_TYPE)
        {
            const Type* ti = m_Ast.getIntType();
            m_Ast.setType(node, ti);
            return ti;
        }
    }
    // null pointer constant??

    auto offset = m_Ast.getOffset(node);
    auto location = m_Locator.getLocationByOffset(offset);

    diagnostics::errorloc(location, "Binary =/!= wrong type");
}

const Type* SemanticAnalysis::checkBinaryLessGreat(node_t node, Context& ctx, bool func)
{
    const auto& children = m_Ast.getChildren(node);
    const Type* t1 = typeCheckExpression(children[0], ctx, func);
    const Type* t2 = typeCheckExpression(children[1], ctx, func);
    // both operands have real type; or
    if (isOfTypeMap(t1->getTypeMap(), TypeMap::INTEGER_TYPE) &&
        isOfTypeMap(t2->getTypeMap(), TypeMap::INTEGER_TYPE))
    {
        const Type* ti = m_Ast.getIntType();
        m_Ast.setType(node, ti);
        return ti;
    }

    // — both operands are pointers to qualified or unqualified versions of compatible
    // object types.
    if (t2->getTypeMap() == TypeMap::POINTER_TYPE && t1->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        const PointerType* pt1 = static_cast<const PointerType*>(t1);
        const PointerType* pt2 = static_cast<const PointerType*>(t2);
        if (isCompatibleType(t1, t2, true) &&
            isOfTypeMap(pt1->getPointsTo()->getTypeMap(), TypeMap::OBJECT_TYPE) &&
            isOfTypeMap(pt2->getPointsTo()->getTypeMap(), TypeMap::OBJECT_TYPE))
        {
            const Type* ti = m_Ast.getIntType();
            m_Ast.setType(node, ti);
            return ti;
        }
    }
    auto offset = m_Ast.getOffset(node);
    auto location = m_Locator.getLocationByOffset(offset);

    diagnostics::errorloc(location, "</>/<=/>= wrong type");
}

const Type* SemanticAnalysis::checkConditional(node_t node, Context& ctx, bool func)
{
    // The first operand shall have scalar type
    const auto& children = m_Ast.getChildren(node);

    const Type* t1 = typeCheckExpression(children[0], ctx, func);

    if (!isOfTypeMap(t1->getTypeMap(), TypeMap::SCALAR_TYPE))
    {
        auto offset = m_Ast.getOffset(node);
        auto location = m_Locator.getLocationByOffset(offset);

        diagnostics::errorloc(location, "Conditional cond wrong type");
    }

    const Type* t2 = typeCheckExpression(children[1], ctx, func);
    const Type* t3 = typeCheckExpression(children[2], ctx, func);

    // both operands have arithmetic type;
    if (isOfTypeMap(t2->getTypeMap(), TypeMap::INTEGER_TYPE) &&
        isOfTypeMap(t3->getTypeMap(), TypeMap::INTEGER_TYPE))
    {
        const Type* ti = m_Ast.getIntType();
        m_Ast.setType(node, ti);
        return ti;
    }

    // — both operands have the same structure or union type;
    if (t2->getTypeMap() == TypeMap::STRUCT_TYPE && t3->getTypeMap() == TypeMap::STRUCT_TYPE)
    {
        try
        {
            const StructType* st2 = static_cast<const StructType*>(t2);
            const StructType* st3 = static_cast<const StructType*>(t3);
            if (*st2 == *st3)
            {
                m_Ast.setType(node, t2);
                return t2;
            }
        }
        catch (...)
        {

            auto offset = m_Ast.getOffset(node);
            auto location = m_Locator.getLocationByOffset(offset);
            diagnostics::errorloc(location, "Conditional wrong type");
        }
    }

    // — both operands have void type;  result has void type.
    if (t2->getTypeMap() == TypeMap::VOID_TYPE && t3->getTypeMap() == TypeMap::VOID_TYPE)
    {
        m_Ast.setType(node, t2);
        return t2;
    }

    // — both operands are pointers to qualified or unqualified versions of compatible
    // types;
    // or — one operand is a pointer to an object type and the other is a pointer to a qualified or
    // unqualified version of void.
    if (t2->getTypeMap() == TypeMap::POINTER_TYPE && t3->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        if (isCompatibleType(t2, t3, true))
        {
            m_Ast.setType(node, t2);
            return t2;
        }
        const PointerType* pt2 = static_cast<const PointerType*>(t2);
        const PointerType* pt3 = static_cast<const PointerType*>(t3);
        if (pt2->getPointsTo()->getTypeMap() == TypeMap::VOID_TYPE &&
            isOfTypeMap(pt3->getPointsTo()->getTypeMap(), TypeMap::OBJECT_TYPE))
        {
            m_Ast.setType(node, t3);
            return t3;
        }
        if (pt3->getPointsTo()->getTypeMap() == TypeMap::VOID_TYPE &&
            isOfTypeMap(pt2->getPointsTo()->getTypeMap(), TypeMap::OBJECT_TYPE))
        {
            m_Ast.setType(node, t2);
            return t2;
        }
    }
    // — one operand is a pointer and the other is a null pointer constant;
    if (t2->getTypeMap() == TypeMap::POINTER_TYPE && t3->getTypeMap() == TypeMap::INT_TYPE &&
        m_Ast.getNodeKind(children[2]) == NodeKind::CONSTANT &&
        *m_Ast.getSymbolTable()[m_Ast.getContentId(children[2])] == "0")
    {
        m_Ast.setType(node, t2);
        return t2;
    }
    if (t3->getTypeMap() == TypeMap::POINTER_TYPE && t2->getTypeMap() == TypeMap::INT_TYPE &&
        m_Ast.getNodeKind(children[1]) == NodeKind::CONSTANT &&
        *m_Ast.getSymbolTable()[m_Ast.getContentId(children[1])] == "0")
    {
        m_Ast.setType(node, t3);
        return t3;
    }
    // function pointers
    if (t2->getTypeMap() == TypeMap::FUNCTION_TYPE &&
        (isPointerToFunc(t3) || (t3->getTypeMap() == TypeMap::INT_TYPE &&
                                 m_Ast.getNodeKind(children[2]) == NodeKind::CONSTANT &&
                                 *m_Ast.getSymbolTable()[m_Ast.getContentId(children[2])] == "0")))
    {
        const Type* ti = m_Ast.createPointerType(t2);
        m_Ast.setType(node, ti);
        return ti;
    }
    if (t3->getTypeMap() == TypeMap::FUNCTION_TYPE &&
        (isPointerToFunc(t2) || (t2->getTypeMap() == TypeMap::INT_TYPE &&
                                 m_Ast.getNodeKind(children[1]) == NodeKind::CONSTANT &&
                                 *m_Ast.getSymbolTable()[m_Ast.getContentId(children[1])] == "0")))
    {
        const Type* ti = m_Ast.createPointerType(t3);
        m_Ast.setType(node, ti);
        return ti;
    }

    const auto offset = m_Ast.getOffset(node);
    const auto location = m_Locator.getLocationByOffset(offset);

    diagnostics::errorloc(location, "Conditional wrong type");
    /*
    If both the second and third operands are pointers or one is a null pointer constant and
    the other is a pointer, the result type is a pointer to a type qualified with all the
    type qualifiers of the types referenced by both operands. Furthermore, if both operands
    are pointers to compatible types or to differently qualified versions of compatible
    types, the result type is a pointer to an appropriately qualified version of the
    composite type; if one operand is a null pointer constant, the result has the type of
    the other operand; otherwise, one operand is a pointer to void or a qualified version of
    void, in which case the result type is a pointer to an appropriately qualified version
    of void.
    */
}

const Type* SemanticAnalysis::checkArrayAccess(node_t node, Context& ctx, bool func)
{
    // One of the expressions shall have type ‘‘pointer to complete object type’
    // the other expression shall have integer type, and the result has type ‘‘type’’
    const auto& children = m_Ast.getChildren(node);

    const Type* t1 = typeCheckExpression(children[0], ctx, func);
    const Type* t2 = typeCheckExpression(children[1], ctx, func);

    const auto offset = m_Ast.getOffset(node);
    const auto location = m_Locator.getLocationByOffset(offset);

    if (typeid(*t1) == typeid(PointerType))
    {
        const PointerType* tt1 = static_cast<const PointerType*>(t1);
        if (!isOfTypeMap(t2->getTypeMap(), TypeMap::INTEGER_TYPE))
            diagnostics::errorloc(location, "Array access wrong type ");

        const Type* pt1 = tt1->getPointsTo();
        if (!isOfTypeMap(pt1->getTypeMap(), TypeMap::COMPLETE_TYPE))
            diagnostics::errorloc(location, "Array access wrong type ");

        m_Ast.setType(node, pt1);
        return pt1;
    }
    if (typeid(*t2) == typeid(PointerType))
    {
        const PointerType* tt2 = static_cast<const PointerType*>(t2);
        if (!isOfTypeMap(t1->getTypeMap(), TypeMap::INTEGER_TYPE))
            diagnostics::errorloc(location, "Array access wrong type ");

        const Type* pt2 = tt2->getPointsTo();
        if (!isOfTypeMap(pt2->getTypeMap(), TypeMap::COMPLETE_TYPE))
            diagnostics::errorloc(location, "Array access wrong type ");

        m_Ast.setType(node, pt2);
        return pt2;
    }

    diagnostics::errorloc(location, "Array access wrong type ");
}

const Type* SemanticAnalysis::checkCallExpression(node_t node, Context& ctx)
{
    const auto& children = m_Ast.getChildren(node);

    // posible exception
    const node_t callee = children[children.size() - 1];
    const Type* t1;
    if (m_Ast.getNodeKind(callee) == NodeKind::VAR)
        t1 = typeCheckExpression(callee, ctx, true);
    else
        t1 = typeCheckExpression(callee, ctx, false);
    const auto offset = m_Ast.getOffset(node);
    const auto location = m_Locator.getLocationByOffset(offset);
    if (isPointerToFunc(t1) && t1->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        const PointerType* pt = dynamic_cast<const PointerType*>(t1);
        t1 = pt->getPointsTo();
    }

    if (typeid(*t1) != typeid(FunctionType))
        diagnostics::errorloc(location, "Not calling a function ");

    const FunctionType* func_type = dynamic_cast<const FunctionType*>(t1);

    const Type* ret_type = func_type->getReturnType();
    const auto& params = func_type->getParameters();

    if (params.size() != m_Ast.getChildren(node).size() - 1)
        diagnostics::errorloc(location, "Wrong number of arguments");

    bool flag = true;
    for (uint32_t i = 0; i < params.size(); i++)
    {
        const Type* arg_type;
        const Type* param_type = params[i].type;
        if (isPointerToFunc(param_type) && m_Ast.getNodeKind(children[i]) == NodeKind::VAR)
            arg_type = typeCheckExpression(children[i], ctx, true);
        else
            arg_type = typeCheckExpression(children[i], ctx, false);

        // TODO: @teddy
        if (typeid(*arg_type) == typeid(StructType) || typeid(*arg_type) == typeid(NamedType))
            flag &= compStructs(arg_type, param_type, ctx.structs);
        if (isPointerToFunc(param_type) && typeid(*param_type) == typeid(PointerType))
        {
            const PointerType* pty1 = static_cast<const PointerType*>(param_type);
            flag &= isCompatibleType(arg_type, pty1->getPointsTo(), true) ||
                    isCompatibleType(arg_type, param_type, true);
        }
        else if (param_type->getTypeMap() == TypeMap::POINTER_TYPE &&
                 arg_type->getTypeMap() == TypeMap::INT_TYPE &&
                 m_Ast.getNodeKind(children[i]) == NodeKind::CONSTANT)
            flag &= *m_Ast.getSymbolTable()[m_Ast.getContentId(children[i])] == "0";
        else if (typeid(*arg_type) == typeid(PointerType))
            flag &= isCompatibleType(arg_type, param_type, true);
        else if (typeid(*arg_type) == typeid(FunctionType))
            flag &= isCompatibleType(m_Ast.createPointerType(arg_type), param_type, true);
        // TODO pointers to structs names in Env
        else
            flag &= isCompatibleType(arg_type, param_type);
    }

    if (!flag)
        diagnostics::errorloc(location, "Wrong arguments type");
    if (typeid(*ret_type) == typeid(NamedType))
    {
        const NamedType* sty = static_cast<const NamedType*>(ret_type);
        uint32_t strName = sty->getName();
        Env temp = ctx.structs;
        while (!temp.empty())
        {
            if (temp.top().count(strName))
            {
                ret_type = temp.top()[strName];
                break;
            }
            temp.pop();
        }
    }

    m_Ast.setType(node, ret_type);
    return ret_type;
}

const Type* SemanticAnalysis::checkDotAccess(node_t node, Context& ctx, bool func)
{
    //  The first operand of the . operator shall have an atomic, qualified, or unqualified
    //  structure or union type
    // and the second operand shall name a member of that type.
    const Type* t1;
    if (m_Ast.getNodeKind(m_Ast.getChildren(node)[0]) != NodeKind::CALL_EXPRESSION)
        t1 = typeCheckExpression(m_Ast.getChildren(node)[0], ctx, false);
    else
        t1 = typeCheckExpression(m_Ast.getChildren(node)[0], ctx, func);

    // if (typeid(*t1) != typeid(StructType) || typeid(*t1) != typeid(NamedType))
    //     diagnostics::error("Not calling a struct type ", location);
    const StructType* struct_type = 0;
    if (typeid(*t1) == typeid(NamedType))
    {
        const NamedType* nt = static_cast<const NamedType*>(t1);
        Env temp = ctx.structs;
        uint32_t name = nt->getName();
        while (!temp.empty())
        {
            if (temp.top().count(name))
            {
                const Type* ti = temp.top()[name];
                struct_type = static_cast<const StructType*>(ti);
                break;
            }
            temp.pop();
        }
    }
    if (typeid(*t1) == typeid(StructType))
        struct_type = static_cast<const StructType*>(t1);
    if (struct_type != 0)
    {
        const auto& members = struct_type->getMembers();

        for (size_t i = 0; i < members.size(); i++)
        {
            auto type = members[i].type;
            auto cid = members[i].contentId;
            if (cid == m_Ast.getContentId(node))
            {
                m_Ast.setType(node, type);
                return type;
            }
        }
    }

    const auto offset = m_Ast.getOffset(node);
    const auto location = m_Locator.getLocationByOffset(offset);

    diagnostics::errorloc(location, "Not such member in a struct ");
}

const Type* SemanticAnalysis::checkArrowAccess(node_t node, Context& ctx, bool func)
{
    const auto offset = m_Ast.getOffset(node);
    const auto location = m_Locator.getLocationByOffset(offset);
    //  The first operand of the -> operator shall have type
    // ‘‘pointer to atomic, qualified, or unqualified structure’
    const Type* t1;
    if (m_Ast.getNodeKind(m_Ast.getChildren(node)[0]) != NodeKind::CALL_EXPRESSION)
        t1 = typeCheckExpression(m_Ast.getChildren(node)[0], ctx, false);
    else
        t1 = typeCheckExpression(m_Ast.getChildren(node)[0], ctx, func);
    if (typeid(*t1) != typeid(PointerType))
        diagnostics::errorloc(location, "Not a pointer in ->");

    const PointerType* ptr_type = static_cast<const PointerType*>(t1);

    if (ptr_type->getPointsTo()->getTypeMap() != TypeMap::STRUCT_TYPE)
        diagnostics::errorloc(location, "Not a pointer to struct type ");

    const StructType* struct_type = 0;
    if (typeid(*ptr_type->getPointsTo()) == typeid(NamedType))
    {
        const NamedType* nt = static_cast<const NamedType*>(ptr_type->getPointsTo());
        Env temp = ctx.structs;
        uint32_t name = nt->getName();
        while (!temp.empty())
        {
            if (temp.top().count(name))
            {
                const Type* ti = temp.top()[name];
                struct_type = static_cast<const StructType*>(ti);
                break;
            }
            temp.pop();
        }
    }
    if (typeid(*ptr_type->getPointsTo()) == typeid(StructType))
        struct_type = static_cast<const StructType*>(ptr_type->getPointsTo());

    if (struct_type != 0)
    {
        m_Ast.setType(m_Ast.getChildren(node)[0], m_Ast.createPointerType(struct_type));
        const auto& members = struct_type->getMembers();
        for (size_t i = 0; i < members.size(); i++)
        {
            auto type = members[i].type;
            auto cid = members[i].contentId;
            if (cid == m_Ast.getContentId(node))
            {
                m_Ast.setType(node, type);
                return type;
            }
        }
    }

    diagnostics::errorloc(location, "Not such member in a struct ");
}

const Type* SemanticAnalysis::checkAddrExpression(node_t node, Context& ctx, bool func)
{
    //  either a function designator, the result of a [] or unary * operator
    // or an lvalue that designates an object that is not a bit-field and is not declared

    const auto child = m_Ast.getChildren(node)[0];
    const Type* t1 = typeCheckExpression(child, ctx, func);
    const auto kind = m_Ast.getNodeKind(child);

    bool flag = false;

    flag |= kind == NodeKind::ARRAY_ACCESS;
    flag |= kind == NodeKind::INDIR;
    flag |= kind == NodeKind::VAR;
    flag |= kind == NodeKind::DOT_ACCESS;
    flag |= kind == NodeKind::ARROW_ACCESS;

    if (!flag)
    {
        auto offset = m_Ast.getOffset(node);
        auto location = m_Locator.getLocationByOffset(offset);

        diagnostics::errorloc(location, "AddressOf wrong type ");
    }

    m_Ast.setType(node, m_Ast.createPointerType(t1));
    return m_Ast.createPointerType(t1);
}

const Type* SemanticAnalysis::checkIndirExpression(node_t node, Context& ctx, bool func)
{
    const Type* t1 = typeCheckExpression(m_Ast.getChildren(node)[0], ctx, func);

    if (typeid(*t1) != typeid(PointerType) && typeid(*t1) != typeid(FunctionType))
    {
        auto offset = m_Ast.getOffset(node);
        auto location = m_Locator.getLocationByOffset(offset);

        diagnostics::errorloc(location, "Indir wrong type ");
    }
    if (typeid(*t1) == typeid(FunctionType))
    {
        m_Ast.setType(node, t1);
        return t1;
    }
    const PointerType* ptr_type = static_cast<const PointerType*>(t1);

    const Type* tp = ptr_type->getPointsTo();
    if (typeid(*tp) == typeid(NamedType))
    {
        const NamedType* nt = static_cast<const NamedType*>(tp);
        Env temp = ctx.structs;
        uint32_t name = nt->getName();
        while (!temp.empty())
        {
            if (temp.top().count(name))
            {
                const Type* ti = temp.top()[name];
                m_Ast.setType(node, ti);
                return ti;
            }
            temp.pop();
        }
    }
    m_Ast.setType(node, tp);
    return tp;
}

const Type* SemanticAnalysis::checkUnaryNeg(node_t node, Context& ctx, bool func)
{
    // aritmetic type
    const Type* t1 = typeCheckExpression(m_Ast.getChildren(node)[0], ctx, func);

    if (!isOfTypeMap(t1->getTypeMap(), TypeMap::INTEGER_TYPE))
    {
        auto offset = m_Ast.getOffset(node);
        auto location = m_Locator.getLocationByOffset(offset);

        diagnostics::errorloc(location, "Unary +/- wrong type ");
    }

    m_Ast.setType(node, t1);
    return t1;
}

const Type* SemanticAnalysis::checkUnaryComplement(node_t node, Context& ctx, bool func)
{
    // integer type
    const Type* t1 = typeCheckExpression(m_Ast.getChildren(node)[0], ctx, func);

    if (!isOfTypeMap(t1->getTypeMap(), TypeMap::INTEGER_TYPE))
    {
        auto offset = m_Ast.getOffset(node);
        auto location = m_Locator.getLocationByOffset(offset);

        diagnostics::errorloc(location, "~ wrong type ");
    }

    m_Ast.setType(node, t1);
    return t1;
}

const Type* SemanticAnalysis::checkUnaryNot(node_t node, Context& ctx, bool func)
{
    // of the ! operator, scalar type
    const Type* t1 = typeCheckExpression(m_Ast.getChildren(node)[0], ctx, func);

    if (!isOfTypeMap(t1->getTypeMap(), TypeMap::SCALAR_TYPE))
    {
        auto offset = m_Ast.getOffset(node);
        auto location = m_Locator.getLocationByOffset(offset);

        diagnostics::errorloc(location, "! wrong type ");
    }

    m_Ast.setType(node, t1);
    return t1;
}

const Type* SemanticAnalysis::checkUnarySizeof(node_t node, Context& ctx, bool func)
{
    const auto& children = m_Ast.getChildren(node);
    const auto size = children.size();

    const Type* t1;
    if (size > 0)
    {
        // not be applied to an expression that has function type or an incomplete type
        t1 = typeCheckExpression(children[0], ctx, func);
        assert(t1 != nullptr && "type was nullptr around line 1312");
    }
    else
    {
        t1 = m_Ast.getType(node);
        assert(t1);

        // if we have a NamedType, convert it into StructType
        // so that codegen can compute the size properly
        if (typeid(*t1) == typeid(NamedType))
        {
            const NamedType* nt = static_cast<const NamedType*>(t1);
            symbol_t name = nt->getName();

            Env temp = ctx.structs;
            while (!temp.empty())
            {
                if (temp.top().count(name))
                {
                    const Type* structType = temp.top()[name];
                    m_Ast.setType(node, structType);
                    break;
                }
                temp.pop();
            }
        }
    }

    if (t1->getTypeMap() == TypeMap::VOID_TYPE)
    {
        auto offset = m_Ast.getOffset(node);
        auto location = m_Locator.getLocationByOffset(offset);

        diagnostics::errorloc(location, "Sizeof wrong type ");
    }

    const Type* ti = m_Ast.getIntType();
    if (size != 0)
        m_Ast.setType(node, ti);
    return ti;
}

const Type* SemanticAnalysis::checkCastExpression(node_t node, Context& ctx, bool func)
{
    // operand shall have scalar type.
    // Unless the type name specifies a void type, the type name shall specify atomic,
    // qualified, or unqualified scalar type, and the operand shall have scalar type.
    const Type* t1 = typeCheckExpression(m_Ast.getChildren(node)[0], ctx, func);
    const Type* t2 = m_Ast.getType(node);

    const auto offset = m_Ast.getOffset(node);
    const auto location = m_Locator.getLocationByOffset(offset);

    // check scalar
    if (!isOfTypeMap(t1->getTypeMap(), TypeMap::SCALAR_TYPE) && !isVoidPointer(t1) &&
        !isPointerToFunc(t1))
        diagnostics::errorloc(location, "Cast should be scalar type ");

    if (isOfTypeMap(t2->getTypeMap(), TypeMap ::SCALAR_TYPE) ||
        isOfTypeMap(t2->getTypeMap(), TypeMap ::VOID_TYPE))
    {
        m_Ast.setType(node, t2);
        return t2;
    }

    diagnostics::errorloc(location, "Cast wrong type ");
}

const Type* SemanticAnalysis::checkConstant(node_t node, const SymbolTable& m_Symbol_Table)
{
    const std::string_view view = *m_Symbol_Table[m_Ast.getContentId(node)];
    if (view[0] == '\'' && view.back() == '\'')
    {
        const Type* ti = m_Ast.getIntType();
        m_Ast.setType(node, ti);
        return ti;
    }
    size_t pos;
    try
    {
        (void)std::stoi(std::string(view), &pos, 0);
    }
    catch (const std::exception&)
    {
        const auto offset = m_Ast.getOffset(node);
        const auto location = m_Locator.getLocationByOffset(offset);

        diagnostics::errorloc(location, "Const wrong type");
    }
    const Type* ti = m_Ast.getIntType();
    m_Ast.setType(node, ti);
    return ti;
}

const Type* SemanticAnalysis::checkStringLit(node_t node)
{
    const Type* ti = m_Ast.createPointerType(m_Ast.getCharType());
    m_Ast.setType(node, ti);
    return ti;
}

const Type* SemanticAnalysis::checkVar(node_t node, Context& ctx, bool func)
{
    const symbol_t name = m_Ast.getContentId(node);
    const auto offset = m_Ast.getOffset(node);
    const auto location = m_Locator.getLocationByOffset(offset);

    if (func && ctx.vars.top().count(name) && !isPointerToFunc(ctx.vars.top()[name]) &&
        !isVoidPointer((ctx.vars.top()[name])))
        diagnostics::errorloc(location, "Var is shadowing the func");
    if (func)
    {
        Env temp = ctx.functionDecls;
        while (!temp.empty())
        {
            if (temp.top().count(name))
            {
                const Type* ti = temp.top()[name];
                if (func || isPointerToFunc(ti) || isVoidPointer(ti))
                {
                    m_Ast.setType(node, ti);
                    return ti;
                }
            }
            temp.pop();
        }
    }
    Env temp = func ? ctx.functions : ctx.vars;

    while (!temp.empty())
    {
        if (temp.top().count(name))
        {
            const Type* ti = temp.top()[name];
            m_Ast.setType(node, ti);
            return ti;
        }
        temp.pop();
    }
    const auto symbolTable = m_Ast.getSymbolTable();
    temp = ctx.functionDecls;
    while (!temp.empty())
    {
        if (temp.top().count(name))
        {
            const Type* ti = temp.top()[name];
            if (func || isPointerToFunc(ti) || isVoidPointer(ti))
            {
                m_Ast.setType(node, ti);
                return ti;
            }
        }
        temp.pop();
    }
    temp = ctx.functions;
    while (!temp.empty())
    {
        if (temp.top().count(name))
        {
            const Type* ti = temp.top()[name];
            if (func || isPointerToFunc(ti) || isVoidPointer(ti))
            {
                m_Ast.setType(node, ti);
                return ti;
            }
        }
        temp.pop();
    }
    if (func)
    {
        temp = ctx.vars;
        while (!temp.empty())
        {
            if (temp.top().count(name))
            {
                const Type* ti = temp.top()[name];
                if (isPointerToFunc(ti) || isVoidPointer(ti))
                {
                    m_Ast.setType(node, ti);
                    return ti;
                }
            }
            temp.pop();
        }
    }
    diagnostics::errorloc(location, "No such variable: ", *symbolTable[name]);
}

const Type* SemanticAnalysis::checkAssign(node_t node, Context& ctx, bool func)
{
    auto offset = m_Ast.getOffset(node);
    auto location = m_Locator.getLocationByOffset(offset);
    const auto& children = m_Ast.getChildren(node);

    const Type* t1 = typeCheckExpression(children[0], ctx, func);
    const Type* t2;
    if (isPointerToFunc(t1) && m_Ast.getNodeKind(children[1]) == NodeKind::VAR)
        t2 = typeCheckExpression(children[1], ctx, true);
    else
        t2 = typeCheckExpression(children[1], ctx, func);

    NodeKind nk1 = m_Ast.getNodeKind(children[0]);
    if (nk1 != NodeKind::VAR && nk1 != NodeKind::INDIR && nk1 != NodeKind::ARRAY_ACCESS &&
        nk1 != NodeKind::ARROW_ACCESS && nk1 != NodeKind::DOT_ACCESS)
        diagnostics::errorloc(location, "Non lvalue assign");

    if (isOfTypeMap(t1->getTypeMap(), TypeMap::INTEGER_TYPE) &&
        isOfTypeMap(t2->getTypeMap(), TypeMap::INTEGER_TYPE))
    {
        m_Ast.setType(node, t1);
        return t1;
    }

    if (t1->getTypeMap() == TypeMap::STRUCT_TYPE && t2->getTypeMap() == TypeMap::STRUCT_TYPE)
    {
        if (compStructs(t1, t2, ctx.structs))
        {
            m_Ast.setType(node, t1);
            return t1;
        }
    }

    if (t1->getTypeMap() == TypeMap::POINTER_TYPE && t2->getTypeMap() == TypeMap::POINTER_TYPE &&
        isCompatibleType(t1, t2, true))
    {
        m_Ast.setType(node, t1);
        return t1;
    }
    if (t1->getTypeMap() == TypeMap::POINTER_TYPE && t2->getTypeMap() == TypeMap::FUNCTION_TYPE)
    {
        const PointerType* pt1 = static_cast<const PointerType*>(t1);
        const Type* t11 = pt1->getPointsTo();
        if (t11->getTypeMap() == TypeMap::FUNCTION_TYPE)
        {
            const FunctionType* ft1 = static_cast<const FunctionType*>(t11);
            const FunctionType* ft2 = static_cast<const FunctionType*>(t2);
            if (*ft1 == *ft2)
            {
                m_Ast.setType(node, t1);
                return t1;
            }
        }
    }

    // nullptr
    if (t1->getTypeMap() == TypeMap::POINTER_TYPE && t2->getTypeMap() == TypeMap::INT_TYPE)
    {
        if (m_Ast.getNodeKind(children[1]) == NodeKind::CONSTANT &&
            *m_Ast.getSymbolTable()[m_Ast.getContentId(children[1])] == "0")
        {
            // TO-DO update value of ptr to null ptr
            // uint32_t name = m_Ast.getContentId(children[0]);
            // if (ctx.vars.top().count(name))
            //         ctx.vars.top().erase(name)
            // vars.top().insert({name, static_cast<const Type*>(m_Ast.getNullPtrType())});
            // return nullPointer
            m_Ast.setType(node, t1);
            return t1;
        }
        else
            diagnostics::errorloc(location, "Assign pointer wrong type");
    }

    if (t1->getTypeMap() == TypeMap::POINTER_TYPE && t2->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        const PointerType* pt1 = static_cast<const PointerType*>(t1);
        const PointerType* pt2 = static_cast<const PointerType*>(t2);
        if (pt1->getTypeMap() == TypeMap::VOID_TYPE &&
            isOfTypeMap(pt2->getTypeMap(), TypeMap::OBJECT_TYPE))
        {
            m_Ast.setType(node, t2);
            return t2;
        }
        if (pt2->getTypeMap() == TypeMap::VOID_TYPE &&
            isOfTypeMap(pt1->getTypeMap(), TypeMap::OBJECT_TYPE))
        {
            m_Ast.setType(node, t2);
            return t2;
        }
    }

    if (t2->getTypeMap() == TypeMap::NULLPTR_TYPE && t1->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        m_Ast.setType(node, t2);
        return t2;
    }
    diagnostics::errorloc(location, "Assign wrong type");
}

const Type* SemanticAnalysis::typeCheckExpression(node_t node, Context& ctx, bool func)
{
    const SymbolTable& m_SymbolTable = m_Ast.getSymbolTable();

    switch (m_Ast.getNodeKind(node))
    {
        // done
        case NodeKind::BINARY_ADD: return checkBinaryAdd(node, ctx, func);
        // done
        case NodeKind::BINARY_SUB: return checkBinarySub(node, ctx, func);
        // done
        case NodeKind::BINARY_SHL:
        case NodeKind::BINARY_SHR:
        case NodeKind::BINARY_DIV:
        case NodeKind::BINARY_MUL: return checkBinaryMul(node, ctx, func);
        // done
        case NodeKind::BINARY_REM:
        case NodeKind::BINARY_AND:
        case NodeKind::BINARY_OR:
        case NodeKind::BINARY_XOR: return checkBinaryXor(node, ctx, func);
        // done
        case NodeKind::BINARY_LOGICAL_OR:
        case NodeKind::BINARY_LOGICAL_AND: return checkBinaryAnd(node, ctx, func);
        // done
        case NodeKind::BINARY_LOGICAL_LT:
        case NodeKind::BINARY_LOGICAL_GE:
        case NodeKind::BINARY_LOGICAL_LE:
        case NodeKind::BINARY_LOGICAL_GT: return checkBinaryLessGreat(node, ctx, func);
        case NodeKind::BINARY_LOGICAL_EQ:
        case NodeKind::BINARY_LOGICAL_NEQ: return checkBinaryEqNeq(node, ctx, func);
        case NodeKind::CONDITIONAL: return checkConditional(node, ctx, func);
        // done
        case NodeKind::ARRAY_ACCESS: return checkArrayAccess(node, ctx, func);
        case NodeKind::CALL_EXPRESSION: return checkCallExpression(node, ctx);
        // done
        case NodeKind::DOT_ACCESS: return checkDotAccess(node, ctx, func);
        // done
        case NodeKind::ARROW_ACCESS: return checkArrowAccess(node, ctx, func);
        // done
        case NodeKind::INDIR: return checkIndirExpression(node, ctx, func);
        // maybe done?
        case NodeKind::ADDR: return checkAddrExpression(node, ctx, func);
        // done
        case NodeKind::UNARY_POS:
        case NodeKind::UNARY_NEG: return checkUnaryNeg(node, ctx, func);
        // done
        case NodeKind::UNARY_BINARY_COMPLEMENT: return checkUnaryComplement(node, ctx, func);
        // done
        case NodeKind::UNARY_NOT: return checkUnaryNot(node, ctx, func);
        // done
        case NodeKind::UNARY_SIZEOF: return checkUnarySizeof(node, ctx, func);
        // done I think
        case NodeKind::CAST: return checkCastExpression(node, ctx, func);
        // done
        case NodeKind::CONSTANT: return checkConstant(node, m_SymbolTable);
        // done
        case NodeKind::STRING_LITERAL: return checkStringLit(node);
        // done
        case NodeKind::VAR: return checkVar(node, ctx, func);
        // done?
        case NodeKind::ASSIGN: return checkAssign(node, ctx, func);
        default: return nullptr;
    }
}

void SemanticAnalysis::validate()
{
    // top-level declarations (globals & functions)
    const node_t toplevel = m_Ast.getOrigin();
    Env vars = {};
    Env functions = {};
    Env functionDecls = {};
    Env structs = {};
    LabEnv labelsUsed = {};

    typeCheckStatement(toplevel, vars, functions, functionDecls, structs, labelsUsed, 0, false, {},
                       true);
    if (!labelsSeen.empty())
    {
        auto gotoNode = labelsSeen.begin()->second;
        auto offset = m_Ast.getOffset(gotoNode);
        auto location = m_Locator.getLocationByOffset(offset);
        // not neceserilly first not declared label!
        diagnostics::errorloc(location, "Label not declared");
    }
}
