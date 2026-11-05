
#include "Codegen.h"
#include "Diagnostics.h"
#include "NodeUtils.h"
#include "SymbolTable.h"
#include "Types.h"

#include <charconv>
#include <cmath>
#include <cstdlib>
#include <string>
#include <string_view>

#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Type.h"

using namespace llvm;

using BB = BasicBlock;

constexpr unsigned int ADDR_SPACE = 0;

// what works:
//  - global vars, local vars
//  - binary operators
//  - function calls
//  - if else, ternary
//  - while loops
//  - string/char literals
//  - casting
//  - function pointers
//  - pointer arith / diff
//  - continue break
//  - implicit conversion
//  - structs

// what needs work:
//  - sizeof
//  - variable shadowing

static int parseInt(std::string_view str)
{
    int value;

    std::from_chars(str.data(), str.data() + str.size(), value);

    // defensive programming? never heard of it.
    // who uses errno these days anyways

    return value;
}

Codegen::Codegen(Backend& backend, const Ast& ast, const Locator& locator)
    : m_Ast(ast), m_SymbolTable(ast.getSymbolTable()), m_Locator(locator), m_Backend(backend),
      m_Builder(m_Backend.getBuilder())
{
}

llvm::Type* Codegen::convertType(const ::Type* type)
{
    if (type == nullptr)
        return nullptr;

    switch (type->getTypeMap())
    {
        case TypeMap::VOID_TYPE: return m_Builder.getVoidTy();
        case TypeMap::CHAR_TYPE: return m_Builder.getInt8Ty();
        case TypeMap::INT_TYPE: return m_Builder.getInt32Ty();
        case TypeMap::POINTER_TYPE:
        {
            const auto ptrType = static_cast<const ::PointerType*>(type);

            // we must do this recursive business because
            // we want to keep track of struct declarations
            const auto dstType = ptrType->getPointsTo();
            assert(dstType && "pointer problems...");

            if (typeid(*dstType) == typeid(::StructType))
                convertType(ptrType->getPointsTo());

            return m_Builder.getPtrTy();
        }
        case TypeMap::FUNCTION_TYPE:
        {
            auto self = dynamic_cast<const ::FunctionType*>(type);
            auto retType = convertType(self->getReturnType());

            SmallVector<llvm::Type*> paramTypes;

            const auto& params = self->getParameters();
            for (size_t i = 0; i < params.size(); ++i)
            {
                const auto& param = params[i];

                const auto paramType = convertType(param.type);
                paramTypes.push_back(paramType);
            }

            return llvm::FunctionType::get(retType, paramTypes, false);
        }
        case TypeMap::STRUCT_TYPE:
        {
            // struct S { ... };
            if (typeid(*type) == typeid(::StructType))
            {
                const auto self = dynamic_cast<const ::StructType*>(type);
                const auto name = self->getName();

                SmallVector<llvm::Type*> memberTypes;

                const auto& members = self->getMembers();
                for (size_t i = 0; i < members.size(); ++i)
                {
                    const auto& member = members[i];

                    auto memberType = convertType(member.type);
                    memberTypes.push_back(memberType);
                }

                auto structType = llvm::StructType::get(m_Backend.getContext(), memberTypes);

                m_Structs.insert_or_assign(name, structType);

                return structType;
            }
            else // struct S;
            {
                // we have to lookup the struct type ina map
                auto self = dynamic_cast<const NamedType*>(type);
                auto symbol = self->getName();

                auto tmp = m_Structs.lookup(symbol);

                assert(tmp && "STRUCT NOT FOUND IN TABLE");

                return tmp;
            }
        }
        default:
            diagnostics::error("failed to convert types during codegen!", (int)type->getTypeMap());
    }
}

/**
 * to anybody from the future wondering why this function exists and what it does lemme explain:
 *
 * A long time ago we made the decision to make just one sizeof node for both kinds of operators
 * and if we have a subexpression we will store it as first children
 * and if we have sizeof with a type as argument _we will store it in the type of the sizeof node_
 * (?!)
 *
 * there is no need to question/defend/ponder/frustrate over this decision now.
 * with only 14 days left and 5 other exams within those 14 days, I decided to fix it like this.
 * By making a new `getType()` function which gets the actual type... as the name might imply.
 */
static const ::Type* getActualType(const Ast& ast, node_t node)
{
    if (ast.getNodeKind(node) == NodeKind::UNARY_SIZEOF && ast.getChildren(node).size() == 0)
        return ast.getIntType();

    return ast.getType(node);
}

Value* Codegen::implicitCast(Value* value, llvm::Type* desiredType)
{
    auto valueType = value->getType();

    if (valueType == desiredType)
        return value;

    assert(desiredType && "you desire to be of type NULL ?");

    if (valueType->isIntegerTy() && desiredType->isIntegerTy())
    {
        return m_Builder.CreateIntCast(value, desiredType, true);
    }

    // this should only really happen when you try to assign
    // an pointer to an integer literal, I believe
    if (valueType->isIntegerTy() && desiredType->isPointerTy())
    {
        return m_Builder.CreateIntToPtr(value, desiredType);
    }

    // this should only really happen when you try to check
    // that a pointer is not null
    if (valueType->isPointerTy() && desiredType->isIntegerTy())
    {
        return m_Builder.CreateIntToPtr(value, desiredType);
    }

    assert(!valueType->isVoidTy());

    assert(0 && "implicit conversion of what?");
}

BasicBlock* Codegen::createBasicBlock(const std::string& name = "")
{
    return BasicBlock::Create(m_Backend.getContext(), name, m_Function);
}

void Codegen::registerFunction(std::pair<symbol_t, Function*> pair) { m_Functions.insert(pair); }

Function* Codegen::lookupFunction(symbol_t symbol) { return m_Functions.lookup(symbol); }

void Codegen::registerGlobalVariable(std::pair<symbol_t, GlobalVariable*> pair)
{
    m_Globals.insert(pair);
}

GlobalVariable* Codegen::lookupGlobalVariable(symbol_t symbol) { return m_Globals.lookup(symbol); }

void Codegen::registerLocalVariable(std::pair<symbol_t, Value*> pair) { m_Locals.insert(pair); }

Value* Codegen::lookupLocalVariable(symbol_t symbol) { return m_Locals.lookup(symbol); }

void Codegen::registerLabel(std::pair<symbol_t, BasicBlock*> pair) { m_Labels.insert(pair); }

BasicBlock* Codegen::lookupLabel(symbol_t symbol) { return m_Labels.lookup(symbol); }

// ----------------------------------------------------------------------------------------------
//  TOPLEVEL    TOPLEVEL    TOPLEVEL    TOPLEVEL    TOPLEVEL    TOPLEVEL    TOPLEVEL    TOPLEVEL
// ----------------------------------------------------------------------------------------------

void Codegen::emitToplevel()
{
    const auto root = m_Ast.getOrigin();
    const auto& children = m_Ast.getChildren(root);
    const size_t nchildren = children.size();

    for (size_t i = 0; i < nchildren; ++i)
    {
        // toplevel statements
        const auto node = children[i];
        const auto kind = m_Ast.getNodeKind(node);

        switch (kind)
        {
            case NodeKind::FUNCTION:
            {
                emitFunction(node);
                break;
            }
            case NodeKind::DECLARATION:
            {
                emitGlobalDeclaration(node);
                break;
            }
            case NodeKind::STRUCT_DECL:
            {
                emitStructDeclaration(node);
                break;
            }
            case NodeKind::EXTERN_DECLARATION:
            {
                emitExternVariableDeclaration(node);
                break;
            }
            default:
            {
                auto offset = m_Ast.getOffset(node);
                auto location = m_Locator.getLocationByOffset(offset);

                diagnostics::errorloc(location, "unknown toplevel statement!");
            }
        }
    }
}

// 1. converts return/parameter types to llvm's type system
// 2. creates a new function inside the module
// 3. creates 2 basic blocks. one for allocas, the other one for code
// 4. emits the function body
// 5. closes the alloca-block with an unconditional branch
void Codegen::emitFunction(node_t node)
{
    const auto astType = m_Ast.getType(node);
    const auto astFunctionType = dynamic_cast<const ::FunctionType*>(astType);

    auto functionType = static_cast<llvm::FunctionType*>(convertType(astType));

    const auto symbol = m_Ast.getContentId(node);
    const auto functionName = std::string(*m_SymbolTable[symbol]);

    const auto linkage = Function::ExternalLinkage;
    auto& module = m_Backend.getModule();

    // function has been previously declared, we dont wanna emit it again
    // so what we do instead is attach the bodt of this definition
    // to the previous declaration.
    if (!(m_Function = module.getFunction(functionName)))
    {
        // create a llvm function in the current module
        // and add it to the our data structures
        m_Function = Function::Create(functionType, linkage, ADDR_SPACE, functionName, &module);

        registerFunction({symbol, m_Function});
    }

    m_Locals.clear();
    m_Labels.clear();

    // we create two basic blocks.
    // one for allocas only. the other one for the code of the body
    BB* entry = createBasicBlock();
    BB* bb1 = createBasicBlock();

    m_Builder.SetInsertPoint(entry);

    // in order to handle parameters the same way as local variables
    // we alloca some stack space and store them into there
    const auto& params = astFunctionType->getParameters();
    for (size_t i = 0; i < params.size(); ++i)
    {
        auto llvmParam = m_Function->getArg(i);
        auto value = m_Builder.CreateAlloca(llvmParam->getType(), ADDR_SPACE);
        m_Builder.CreateStore(llvmParam, value);

        auto symbol = params[i].contentId;

        registerLocalVariable({symbol, value});
    }

    // now we recursively emit the code for the function body
    const auto& children = m_Ast.getChildren(node);
    const auto functionBody = children[children.size() - 1];

    m_Builder.SetInsertPoint(bb1);
    emitBlock(functionBody);

    // if the body of the function isnt terminated with a return/branch
    // then we put this return at the end and just always return zero.
    auto bb = m_Builder.GetInsertBlock();
    if (bb->getTerminator() == nullptr)
    {
        auto returnType = functionType->getReturnType();

        if (returnType->isVoidTy())
            m_Builder.CreateRetVoid();
        else
            m_Builder.CreateRet(Constant::getNullValue(returnType));
    }

    // we are almost done with this function
    // so now we close the alloca block by adding
    // an uncoditional jump to the start of the body.
    m_Builder.SetInsertPoint(entry);
    m_Builder.CreateBr(bb1);

    // clean up any unreachable blocks which might got created
    // by dangeling goto/label/while/return constructs
    m_Backend.removeUnreachableBlocks(*m_Function);
}

void Codegen::emitExternFunctionDeclaration(node_t node)
{
    const auto symbol = m_Ast.getContentId(node);
    const auto functionName = *m_SymbolTable[symbol];

    const auto astType = m_Ast.getType(node);
    auto functionType = static_cast<llvm::FunctionType*>(convertType(astType));

    const auto linkage = Function::ExternalLinkage;
    auto& module = m_Backend.getModule();

    // this creates a function declaration with external linkage.
    auto function = Function::Create(functionType, linkage, ADDR_SPACE, functionName, &module);

    registerFunction({symbol, function});
}

void Codegen::emitGlobalDeclaration(node_t node)
{
    auto& module = m_Backend.getModule();

    const auto& children = m_Ast.getChildren(node);
    const auto symbol = m_Ast.getContentId(node);
    const auto name = std::string(*m_SymbolTable[symbol]);

    const auto astType = m_Ast.getType(node);

    if (astType->getTypeMap() == TypeMap::FUNCTION_TYPE)
        return emitExternFunctionDeclaration(node);

    auto valType = convertType(m_Ast.getType(node));
    auto alignment = module.getDataLayout().getPrefTypeAlign(valType);

    module.getOrInsertGlobal(name, valType);
    auto var = module.getNamedGlobal(name);

    var->setLinkage(GlobalValue::CommonLinkage);
    var->setAlignment(alignment);
    var->setInitializer(Constant::getNullValue(valType));

    if (children.size() == 1)
    {
        if (astType->getTypeMap() == TypeMap::INT_TYPE)
        {
            assert(m_Ast.getNodeKind(children[0]) == NodeKind::CONSTANT);

            auto symbol = m_Ast.getContentId(children[0]);
            auto string = m_SymbolTable[symbol];
            auto value = parseInt(*string);

            var->setInitializer(ConstantInt::get(valType, value));
        }
        else
        {
            assert(0 && "initialization of non-int globals not yet supported");
        }
    }

    registerGlobalVariable({symbol, var});
}

void Codegen::emitExternVariableDeclaration(node_t node)
{
    auto& module = m_Backend.getModule();

    auto astType = m_Ast.getType(node);

    if (astType->getTypeMap() == TypeMap::FUNCTION_TYPE)
        return emitExternFunctionDeclaration(node);

    auto valType = convertType(astType);
    auto alignment = module.getDataLayout().getPrefTypeAlign(valType);

    const auto symbol = m_Ast.getContentId(node);
    const auto name = std::string(*m_SymbolTable[symbol]);

    module.getOrInsertGlobal(name, valType);
    auto var = module.getNamedGlobal(name);

    var->setLinkage(GlobalValue::ExternalLinkage);
    var->setAlignment(alignment);

    registerGlobalVariable({symbol, var});
}

void Codegen::emitStructDeclaration(node_t node)
{
    auto astType = dynamic_cast<const ::StructType*>(m_Ast.getType(node));

    // converting types does the insertion now
    // m_Structs.insert({symbol, structType});
    if (astType)
        convertType(astType);

    // TODO: not done yet my friend
}

// --------------------------------------------------------------------------------------
//  R-VALUE    R-VALUE    R-VALUE    R-VALUE    R-VALUE    R-VALUE    R-VALUE    R-VALUE
// --------------------------------------------------------------------------------------

Value* Codegen::emitExprR(node_t node)
{
    using Ops = Instruction::BinaryOps;
    using Pred = CmpInst::Predicate;

    switch (m_Ast.getNodeKind(node))
    {
        case NodeKind::CONSTANT: return emitConstantR(node);
        case NodeKind::VAR: return emitVariableR(node);
        case NodeKind::CALL_EXPRESSION: return emitCallR(node);
        case NodeKind::ASSIGN: return emitAssignR(node);
        case NodeKind::ARRAY_ACCESS: return emitArrayAccessR(node);
        case NodeKind::CAST: return emitCastR(node);
        case NodeKind::ADDR: return emitAddrR(node);
        case NodeKind::INDIR: return emitIndirR(node);

        case NodeKind::BINARY_ADD: return emitBinaryOpR(node, Ops::Add);
        case NodeKind::BINARY_SUB: return emitBinaryOpR(node, Ops::Sub);
        case NodeKind::BINARY_MUL: return emitBinaryOpR(node, Ops::Mul);
        case NodeKind::BINARY_DIV: return emitBinaryOpR(node, Ops::SDiv);
        case NodeKind::BINARY_REM: return emitBinaryOpR(node, Ops::SRem);
        case NodeKind::BINARY_XOR: return emitBinaryOpR(node, Ops::Xor);
        case NodeKind::BINARY_OR: return emitBinaryOpR(node, Ops::Or);
        case NodeKind::BINARY_AND: return emitBinaryOpR(node, Ops::And);
        case NodeKind::BINARY_SHL: return emitBinaryOpR(node, Ops::Shl);
        case NodeKind::BINARY_SHR: return emitBinaryOpR(node, Ops::LShr);

        case NodeKind::BINARY_LOGICAL_EQ: return emitCompareOpR(node, Pred::ICMP_EQ);
        case NodeKind::BINARY_LOGICAL_NEQ: return emitCompareOpR(node, Pred::ICMP_NE);
        case NodeKind::BINARY_LOGICAL_LE: return emitCompareOpR(node, Pred::ICMP_SLE);
        case NodeKind::BINARY_LOGICAL_LT: return emitCompareOpR(node, Pred::ICMP_SLT);
        case NodeKind::BINARY_LOGICAL_GE: return emitCompareOpR(node, Pred::ICMP_SGE);
        case NodeKind::BINARY_LOGICAL_GT: return emitCompareOpR(node, Pred::ICMP_SGT);

        case NodeKind::POINTER_DIFF: return emitPointerDiffR(node);
        case NodeKind::POINTER_ADD_OFFSET: return emitPointerAddOffsetR(node);
        case NodeKind::POINTER_SUB_OFFSET: return emitPointerSubOffsetR(node);
        case NodeKind::UNARY_NOT: return emitUnaryNotR(node);
        case NodeKind::UNARY_NEG: return emitUnaryNegR(node);
        case NodeKind::UNARY_POS: return emitUnaryPosR(node);
        case NodeKind::UNARY_BINARY_COMPLEMENT: return emitUnaryComplementR(node);
        case NodeKind::CONDITIONAL: return emitTernaryR(node);
        case NodeKind::STRING_LITERAL: return emitStringR(node);
        case NodeKind::DOT_ACCESS: return emitDotAccessR(node);
        case NodeKind::ARROW_ACCESS: return emitArrowAccessR(node);
        case NodeKind::UNARY_SIZEOF: return emitSizeofR(node);

        case NodeKind::BINARY_LOGICAL_AND:
        case NodeKind::BINARY_LOGICAL_OR: return emitLogicalR(node);

        default:
        {
            const auto kind = m_Ast.getNodeKind(node);
            const auto offset = m_Ast.getOffset(node);
            const auto location = m_Locator.getLocationByOffset(offset);

            diagnostics::errorloc(location, "failed to emit r-value: ", (int)kind);
        }
    }

    return nullptr;
}

Value* Codegen::emitExprR(node_t node, llvm::Type* desiredType)
{
    return implicitCast(emitExprR(node), desiredType);
}

constexpr static std::array<uint8_t, 128> ESCAPE_TABLE = []
{
    std::array<uint8_t, 128> arr = {};

    arr['\\'] = '\\';
    arr['\''] = '\'';
    arr['\"'] = '\"';

    arr['?'] = '\?';
    arr['a'] = '\a';
    arr['b'] = '\b';
    arr['f'] = '\f';
    arr['n'] = '\n';
    arr['r'] = '\r';
    arr['t'] = '\t';
    arr['v'] = '\v';

    return arr;
}();

static int parseChar(std::string_view str)
{
    return (str[1] == '\\') ? ESCAPE_TABLE[str[2]] : str[1];
}

Value* Codegen::emitConstantR(node_t node)
{
    const auto str = *m_SymbolTable[m_Ast.getContentId(node)];

    int value = str[0] == '\'' ? parseChar(str) : parseInt(str);

    const auto type = m_Backend.getBuilder().getInt32Ty();

    return ConstantInt::get(type, value, false);
}

Value* Codegen::emitVariableR(node_t node)
{
    const auto symbol = m_Ast.getContentId(node);
    const auto astType = m_Ast.getType(node);

    assert(astType && "type of variable not set!");

    auto type = convertType(astType);

    Value* value = lookupLocalVariable(symbol);
    if ((value = lookupLocalVariable(symbol)))
    {
        return m_Builder.CreateLoad(type, value);
    }
    else if ((value = lookupGlobalVariable(symbol)))
    {
        return m_Builder.CreateLoad(type, value);
    }
    else if ((value = lookupFunction(symbol)))
    {
        // we don't load those
        return value;
    }

    assert(0 && "VARIABLE NOT FOUND!");
}

Value* Codegen::emitCallR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    // callee
    const auto astFunc = children[children.size() - 1];
    const auto astType = m_Ast.getType(astFunc);

    // if we are calling a indirection e.g.: (**f)();
    // we don't wan't to load it 3 times, only two!
    // one would happen in the emitVariableR because of the alloca
    // check out emitIndirR to see the fix.

    llvm::FunctionType* functionType;
    if (astType->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        const auto astPtrType = static_cast<const ::PointerType*>(astType);
        const auto astFuncType = convertType(astPtrType->getPointsTo());
        functionType = static_cast<llvm::FunctionType*>(astFuncType);
    }
    else
    {
        functionType = static_cast<llvm::FunctionType*>(convertType(astType));
    }

    const auto value = emitExprR(astFunc);

    // arguments
    llvm::SmallVector<Value*> args;
    for (size_t i = 0; i < children.size() - 1; ++i)
    {
        // TODO: implicit convert these
        const auto arg = children[i];
        auto paramType = functionType->getParamType(i);

        args.push_back(emitExprR(arg, paramType));
    }

    return m_Builder.CreateCall(functionType, value, args);
}

Value* Codegen::emitAssignR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    const auto astType = m_Ast.getType(node);
    auto type = convertType(astType);

    auto lhs = emitExprL(children[0]);
    auto rhs = emitExprR(children[1], type);

    m_Builder.CreateStore(rhs, lhs, false);

    return rhs;
}

Value* Codegen::emitArrayAccessR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    const auto astArrayType = static_cast<const ::PointerType*>(m_Ast.getType(children[0]));
    const auto astElementType = astArrayType->getPointsTo();

    auto eltype = convertType(astElementType);

    auto array = emitExprR(children[0]);
    auto index = emitExprR(children[1]);

    // getelementptr inbounds
    auto addr = m_Builder.CreateInBoundsGEP(eltype, array, index);
    return m_Builder.CreateLoad(eltype, addr);
}

// this does not seem to work!
Value* Codegen::emitCastR(node_t node)
{
    using TypeID = llvm::Type::TypeID;
    using CastOps = llvm::Instruction::CastOps;

    auto astType = m_Ast.getType(node);
    auto type = convertType(astType);

    auto value = emitExprR(m_Ast.getChildren(node)[0]);

    auto castTypeID = type->getTypeID();
    auto valueTypeID = value->getType()->getTypeID();

    if (valueTypeID == TypeID::PointerTyID && castTypeID == TypeID::PointerTyID)
    {
        // pointer cast? what is a pointer cast?
        // they are all opaque anyways..
        return value;
    }

    if (valueTypeID == TypeID::PointerTyID && castTypeID == TypeID::IntegerTyID)
    {
        auto tmp = m_Builder.CreateCast(CastOps::PtrToInt, value, m_Builder.getInt64Ty());
        return m_Builder.CreateTrunc(tmp, type);
    }

    if (valueTypeID == TypeID::IntegerTyID && castTypeID == TypeID::PointerTyID)
    {
        auto tmp = m_Builder.CreateZExt(value, m_Builder.getInt64Ty());
        return m_Builder.CreateCast(CastOps::IntToPtr, tmp, type);
    }

    if (valueTypeID == TypeID::IntegerTyID && castTypeID == TypeID::IntegerTyID)
    {
        return m_Builder.CreateIntCast(value, type, true);
    }

    assert(0 && "this type of cast is not yet implemented!");
}

Value* Codegen::emitAddrR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);
    return emitExprL(children[0]);
}

Value* Codegen::emitIndirR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    const auto astType = m_Ast.getType(node);
    auto type = convertType(astType);

    auto val = emitExprR(children[0]);

    // load, unless its a function pointer
    // because we don't load those...
    if (type->isFunctionTy())
        return val;

    return m_Builder.CreateLoad(type, val);
}

Value* Codegen::emitBinaryOpR(node_t node, Instruction::BinaryOps binop)
{
    const auto& children = m_Ast.getChildren(node);

    const auto astType = getActualType(m_Ast, node);
    auto type = convertType(astType);

    auto lhs = emitExprR(children[0], type);
    auto rhs = emitExprR(children[1], type);

    return m_Builder.CreateBinOp(binop, lhs, rhs);
}

Value* Codegen::emitCompareOpR(node_t node, CmpInst::Predicate pred)
{
    const auto& children = m_Ast.getChildren(node);

    const auto lhsType = getActualType(m_Ast, children[0]);
    const auto rhsType = getActualType(m_Ast, children[1]);
    auto type = convertType(getMax(lhsType, rhsType));

    auto lhs = emitExprR(children[0], type);
    auto rhs = emitExprR(children[1], type);

    auto cmp = m_Builder.CreateICmp(pred, lhs, rhs);

    // TODO: is this actually future proof?
    return m_Builder.CreateZExt(cmp, m_Builder.getInt32Ty());
}

Value* Codegen::emitUnaryNotR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    auto value = emitExprR(children[0]);

    auto cmp = m_Builder.CreateICmpEQ(value, m_Builder.getInt32(0));

    return m_Builder.CreateZExt(cmp, m_Builder.getInt32Ty());
}

Value* Codegen::emitUnaryNegR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    auto value = emitExprR(children[0]);

    return m_Builder.CreateNeg(value);
}

Value* Codegen::emitUnaryPosR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    return emitExprR(children[0]);
}

Value* Codegen::emitUnaryComplementR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    auto value = emitExprR(children[0]);

    // I dont think LLVM has a direct complement instruction
    // so we gotta do some bit-trickery
    auto mask = ConstantInt::get(value->getType(), -1);
    return m_Builder.CreateXor(value, mask);
}

Value* Codegen::emitPointerDiffR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    auto astPtrType = static_cast<const ::PointerType*>(m_Ast.getType(children[0]));
    auto astElementType = astPtrType->getPointsTo();
    auto elementType = convertType(astElementType);

    auto lhs = emitExprR(children[0]);
    auto rhs = emitExprR(children[1]);

    auto diff = m_Builder.CreatePtrDiff(elementType, lhs, rhs);
    return m_Builder.CreateIntCast(diff, m_Builder.getInt32Ty(), false);
}

Value* Codegen::emitPointerAddOffsetR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    node_t ptrNode, intNode;
    if (m_Ast.getType(children[0])->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        ptrNode = children[0];
        intNode = children[1];
    }
    else
    {
        ptrNode = children[1];
        intNode = children[0];
    }

    const auto astType = m_Ast.getType(ptrNode);
    const auto astElementType = static_cast<const ::PointerType*>(astType)->getPointsTo();
    auto elementType = convertType(astElementType);

    auto ptr = emitExprR(ptrNode);
    auto num = emitExprR(intNode);

    return m_Builder.CreateInBoundsGEP(elementType, ptr, num);
}

Value* Codegen::emitPointerSubOffsetR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    node_t ptrNode = children[0];
    node_t intNode = children[1];

    const auto astType = m_Ast.getType(ptrNode);
    const auto astElementType = static_cast<const ::PointerType*>(astType)->getPointsTo();
    auto elementType = convertType(astElementType);

    auto ptr = emitExprR(ptrNode);
    auto num = m_Builder.CreateNeg(emitExprR(intNode));

    return m_Builder.CreateInBoundsGEP(elementType, ptr, num);
}

// (c) ? (t) : (f)
// we compile this by making a temp value
// and from each branch storing the result into that temp value
// followed by a final block which loads and yields that temporary
Value* Codegen::emitTernaryR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    auto bbt = createBasicBlock();
    auto bbf = createBasicBlock();

    auto bb = createBasicBlock();

    // emit condition
    emitCF(children[0], *bbt, *bbf);

    // emit temporary
    const auto astType = m_Ast.getType(node);
    auto type = convertType(astType);

    m_Builder.SetInsertPoint(&m_Function->getEntryBlock());
    auto temporary = m_Builder.CreateAlloca(type);

    // emit true case
    m_Builder.SetInsertPoint(bbt);
    auto trueValue = emitExprR(children[1], type);
    m_Builder.CreateStore(trueValue, temporary);
    m_Builder.CreateBr(bb);

    // emit false case
    m_Builder.SetInsertPoint(bbf);
    auto falseValue = emitExprR(children[2], type);
    m_Builder.CreateStore(falseValue, temporary);
    m_Builder.CreateBr(bb);

    // combine true/false into temporary
    m_Builder.SetInsertPoint(bb);

    return m_Builder.CreateLoad(type, temporary);
}

static bool hasEscapeSequence(const std::string_view src)
{
    bool flag = false;

    // lets hope gcc decides to vectorize this
    for (char cc : src)
        flag |= (cc == '\\');

    return true;
}

static int convertEscapeSequences(char* dst, const std::string_view src)
{

    char* old = dst;

    // indicating whether there was an unescaped backslash
    // the previous iteration
    bool flag = false;

    for (char cc : src)
    {
        *dst = flag ? ESCAPE_TABLE[cc] : cc;
        flag = !flag & (cc == '\\');
        dst += !flag;
    }

    // We compute how many bytes were actually written
    // by calculating how much the dst ptr has moved.
    // This is needed because the buffer is overallocated
    // and we dont want to emit a string with trailing junk
    return dst - old;
}

Value* Codegen::emitStringR(node_t node)
{
    const auto symbol = m_Ast.getContentId(node);
    const auto string = *m_SymbolTable[symbol];

    if (hasEscapeSequence(string))
    {
        auto buffer = new char[string.size()];

        // copies the string_view into the buffer
        // while converting \n and co. to their byte repr
        int len = convertEscapeSequences(buffer, string);

        auto value = m_Builder.CreateGlobalString(std::string_view(buffer, len));

        delete[] buffer;
        return value;
    }
    else
    {
        return m_Builder.CreateGlobalString(string);
    }
}

Value* Codegen::emitDotAccessR(node_t node)
{
    const auto astMemberType = m_Ast.getType(node);
    auto memberType = convertType(astMemberType);

    auto ptr = emitDotAccessL(node);
    auto val = m_Builder.CreateLoad(memberType, ptr);

    return val;
}

Value* Codegen::emitArrowAccessR(node_t node)
{
    const auto structPtrNode = m_Ast.getChildren(node)[0];
    const auto symbol = m_Ast.getContentId(node);

    auto structValue = emitExprR(structPtrNode);

    // struct type
    const auto astType = m_Ast.getType(structPtrNode);
    const auto astPtrType = static_cast<const ::PointerType*>(astType);
    const auto astStructType = static_cast<const ::StructType*>(astPtrType->getPointsTo());

    auto structType = convertType(astStructType);

    // member type
    const auto astMemberType = m_Ast.getType(node);
    auto memberType = convertType(astMemberType);

    int index = astStructType->findMember(symbol);

    auto addr = m_Builder.CreateStructGEP(structType, structValue, index);
    auto value = m_Builder.CreateLoad(memberType, addr);

    return value;
}

Value* Codegen::emitSizeofR(node_t node)
{
    const auto& children = m_Ast.getChildren(node);
    auto layout = m_Backend.getModule().getDataLayout();

    // differentiate between sizeof (int) and sizeof *p
    if (children.size() == 0)
    {
        auto astType = m_Ast.getType(node);
        auto type = convertType(astType);
        auto size = layout.getTypeAllocSize(type);

        return m_Builder.getInt32(size);
    }
    else
    {

        if (m_Ast.getNodeKind(children[0]) == NodeKind::STRING_LITERAL)
        {
            auto symbol = m_Ast.getContentId(children[0]);
            auto string = *m_SymbolTable[symbol];

            auto buffer = new char[string.size()];

            int len = convertEscapeSequences(buffer, string);

            delete[] buffer;

            return m_Builder.getInt32(len + 1);
        }

        auto astType = getActualType(m_Ast, children[0]);
        auto type = convertType(astType);
        auto size = layout.getTypeAllocSize(type);

        return m_Builder.getInt32(size);
    }
}

// short circuiting stuff
Value* Codegen::emitLogicalR(node_t node)
{
    auto intType = m_Builder.getInt32Ty();

    BB& entry = m_Function->getEntryBlock();
    BB* before = m_Builder.GetInsertBlock();
    BB* bbt = createBasicBlock();
    BB* bbf = createBasicBlock();
    BB* after = createBasicBlock();

    m_Builder.SetInsertPoint(&entry);
    auto tmp = m_Builder.CreateAlloca(intType);

    m_Builder.SetInsertPoint(before);
    emitCF(node, *bbt, *bbf);

    m_Builder.SetInsertPoint(bbt);
    m_Builder.CreateStore(m_Builder.getInt32(1), tmp);
    m_Builder.CreateBr(after);

    m_Builder.SetInsertPoint(bbf);
    m_Builder.CreateStore(m_Builder.getInt32(0), tmp);
    m_Builder.CreateBr(after);

    m_Builder.SetInsertPoint(after);
    return m_Builder.CreateLoad(intType, tmp);
}

// --------------------------------------------------------------------------------------
//  L-VALUE    L-VALUE    L-VALUE    L-VALUE    L-VALUE    L-VALUE    L-VALUE    L-VALUE
// --------------------------------------------------------------------------------------

Value* Codegen::emitExprL(node_t node)
{
    switch (m_Ast.getNodeKind(node))
    {
        case NodeKind::VAR: return emitVariableL(node);
        case NodeKind::ARRAY_ACCESS: return emitArrayAccessL(node);
        case NodeKind::INDIR: return emitIndirL(node);
        case NodeKind::DOT_ACCESS: return emitDotAccessL(node);
        case NodeKind::ARROW_ACCESS: return emitArrowAccessL(node);

        // you might be wondering why call expressions can have l-value stuff?!
        // since they dont posses lvalue semantics e.g. calling &(f()) is not valid.
        // but detecting this is not my job. Its the job of semantic analysis.
        // This exists because for f().x we need to get the address to calculate
        // the offset of x however f() doesnt have an address, so thats why this hack is here.
        // We just make a local `tmp = f(); tmp.x` because this works and hey, if it works (:
        case NodeKind::CALL_EXPRESSION: return emitCallL(node);

        default:
        {
            const auto kind = m_Ast.getNodeKind(node);
            const auto offset = m_Ast.getOffset(node);
            const auto location = m_Locator.getLocationByOffset(offset);

            diagnostics::errorloc(location, "failed to emit l-value: ", (int)kind);
        }
    }

    return nullptr;
}

Value* Codegen::emitVariableL(node_t node)
{
    // TODO: handle shadowing
    // TODO: handle same name in diff scopes
    const auto symbol = m_Ast.getContentId(node);
    auto local = lookupLocalVariable(symbol);

    if (local != nullptr)
        return local;

    auto global = lookupGlobalVariable(symbol);

    if (global != nullptr)
        return global;

    auto func = lookupFunction(symbol);

    assert(func && "(l-value) variable not found");

    return func;
}

Value* Codegen::emitArrayAccessL(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    const auto astElementType = m_Ast.getType(node);
    auto eltype = convertType(astElementType);

    auto array = emitExprR(children[0]);
    auto index = emitExprR(children[1]);

    return m_Builder.CreateInBoundsGEP(eltype, array, index);
}

Value* Codegen::emitIndirL(node_t node)
{
    auto value = emitExprR(m_Ast.getChildren(node)[0]);
    return value;
}

Value* Codegen::emitDotAccessL(node_t node)
{
    const auto structNode = m_Ast.getChildren(node)[0];
    const auto symbol = m_Ast.getContentId(node);

    auto structValue = emitExprL(structNode);

    // struct type
    const auto astType = m_Ast.getType(structNode);
    assert(astType && "struct broken beyond repair.");
    assert(typeid(*astType) != typeid(NamedType) && "ALARM! NAMED TYPE DETECTED!");
    const auto astStructType = static_cast<const ::StructType*>(astType);
    auto structType = convertType(astType);

    int index = astStructType->findMember(symbol);

    return m_Builder.CreateStructGEP(structType, structValue, index);
}

Value* Codegen::emitArrowAccessL(node_t node)
{
    const auto structNode = m_Ast.getChildren(node)[0];
    const auto symbol = m_Ast.getContentId(node);

    auto structValue = emitExprR(structNode);

    // struct type
    const auto astType = m_Ast.getType(structNode);
    const auto astPtrType = static_cast<const ::PointerType*>(astType);
    const auto astStructType = static_cast<const ::StructType*>(astPtrType->getPointsTo());
    auto structType = convertType(astStructType);

    int index = astStructType->findMember(symbol);

    return m_Builder.CreateStructGEP(structType, structValue, index);
}

// struct S s; s = f(); s.x
Value* Codegen::emitCallL(node_t node)
{
    BB* entry = &m_Function->getEntryBlock();
    BB* bb = m_Builder.GetInsertBlock();

    const auto astType = m_Ast.getType(node);
    auto type = convertType(astType);

    // make local variable
    m_Builder.SetInsertPoint(entry);
    auto tmp = m_Builder.CreateAlloca(type);

    // emit function call
    m_Builder.SetInsertPoint(bb);
    auto val = emitCallR(node);
    m_Builder.CreateStore(val, tmp);

    return tmp;
}

// ----------------------------------------------------------------------------------------------
//  CONTROL FLOW    CONTROL FLOW    CONTROL FLOW    CONTROL FLOW    CONTROL FLOW    CONTROL FLOW
// ----------------------------------------------------------------------------------------------

void Codegen::emitCF(node_t node, BB& bbt, BB& bbf)
{
    // whoever calls this function is responsible themselves
    // to set the insert point correctly afterwards

    using Ops = CmpInst::Predicate;

    switch (m_Ast.getNodeKind(node))
    {
        case NodeKind::BINARY_LOGICAL_LE: emitCmpCF(node, bbt, bbf, Ops::ICMP_SLE); break;
        case NodeKind::BINARY_LOGICAL_LT: emitCmpCF(node, bbt, bbf, Ops::ICMP_SLT); break;
        case NodeKind::BINARY_LOGICAL_GE: emitCmpCF(node, bbt, bbf, Ops::ICMP_SGE); break;
        case NodeKind::BINARY_LOGICAL_GT: emitCmpCF(node, bbt, bbf, Ops::ICMP_SGT); break;
        case NodeKind::BINARY_LOGICAL_EQ: emitCmpCF(node, bbt, bbf, Ops::ICMP_EQ); break;
        case NodeKind::BINARY_LOGICAL_NEQ: emitCmpCF(node, bbt, bbf, Ops::ICMP_NE); break;
        case NodeKind::BINARY_LOGICAL_AND: emitLogicalAndCF(node, bbt, bbf); break;
        case NodeKind::BINARY_LOGICAL_OR: emitLogicalOrCF(node, bbt, bbf); break;
        case NodeKind::IF: emitIfCF(node, bbt, bbf); break;
        case NodeKind::IF_ELSE: emitIfElseCF(node, bbt, bbf); break;
        case NodeKind::WHILE: emitWhileCF(node, bbt, bbf); break;
        case NodeKind::UNARY_NOT: emitNotCF(node, bbt, bbf); break;
        case NodeKind::CONDITIONAL:
        case NodeKind::ASSIGN:
        case NodeKind::UNARY_NEG:
        case NodeKind::UNARY_POS:
        case NodeKind::BINARY_ADD:
        case NodeKind::BINARY_SUB:
        case NodeKind::BINARY_MUL:
        case NodeKind::BINARY_DIV:
        case NodeKind::BINARY_REM:
        case NodeKind::BINARY_XOR:
        case NodeKind::BINARY_OR:
        case NodeKind::BINARY_AND:
        case NodeKind::BINARY_SHL:
        case NodeKind::BINARY_SHR:
        case NodeKind::CONSTANT:
        case NodeKind::VAR:
        case NodeKind::CALL_EXPRESSION:
        case NodeKind::UNARY_SIZEOF:
        case NodeKind::STRING_LITERAL:
        case NodeKind::INDIR:
        case NodeKind::ADDR:
        case NodeKind::CAST:
        case NodeKind::ARRAY_ACCESS:
        case NodeKind::UNARY_BINARY_COMPLEMENT:
        case NodeKind::POINTER_DIFF:
        case NodeKind::POINTER_ADD_OFFSET:
        case NodeKind::POINTER_SUB_OFFSET:
        {
            emitExprCF(node, bbt, bbf);
            break;
        }
        case NodeKind::EXPR_STATEMENT:
        {
            emitCF(m_Ast.getChildren(node)[0], bbt, bbf);
            break;
        }
        default:
        {
            const auto kind = m_Ast.getNodeKind(node);
            const auto offset = m_Ast.getOffset(node);
            const auto location = m_Locator.getLocationByOffset(offset);

            diagnostics::errorloc(location, "failed to emit cf-value: ", (int)kind);
        }
    }
}

void Codegen::emitExprCF(node_t node, BB& bbt, BB& bbf)
{
    auto value = emitExprR(node);
    auto type = value->getType();

    auto cmp = m_Builder.CreateICmpNE(value, Constant::getNullValue(type));
    m_Builder.CreateCondBr(cmp, &bbt, &bbf);
}

void Codegen::emitCmpCF(node_t node, BB& bbt, BB& bbf, CmpInst::Predicate op)
{
    const auto& children = m_Ast.getChildren(node);

    const auto lhsType = getActualType(m_Ast, children[0]);
    const auto rhsType = getActualType(m_Ast, children[1]);

    auto type = convertType(getMax(lhsType, rhsType));

    auto lhs = emitExprR(children[0], type);
    auto rhs = emitExprR(children[1], type);

    auto value = m_Builder.CreateICmp(op, lhs, rhs);
    m_Builder.CreateCondBr(value, &bbt, &bbf);
}

void Codegen::emitNotCF(node_t node, BB& bbt, BB& bbf)
{
    const auto& children = m_Ast.getChildren(node);
    emitCF(children[0], bbf, bbt);
}

void Codegen::emitIfElseCF(node_t node, BB& bbt, BB& bbf)
{
    const auto& children = m_Ast.getChildren(node);

    // emit condition. this will also create the branch instruction
    emitCF(children[0], bbt, bbf);

    BB *prev, *next = createBasicBlock();

    // emit code of the branches to their basic block
    m_Builder.SetInsertPoint(&bbt);
    emitStmt(children[1]);

    // basically, if this case returns or similar
    // we dont need a jump to the next block anyways
    prev = m_Builder.GetInsertBlock();
    if (prev->getTerminator() == nullptr)
    {
        m_Builder.SetInsertPoint(prev);
        m_Builder.CreateBr(next);
    }

    m_Builder.SetInsertPoint(&bbf);
    emitStmt(children[2]);

    prev = m_Builder.GetInsertBlock();
    if (prev->getTerminator() == nullptr)
    {
        m_Builder.SetInsertPoint(prev);
        m_Builder.CreateBr(next);
    }

    // whoever emits after this call returns doesnt have to worry
    // since they will get a fresh basic block
    m_Builder.SetInsertPoint(next);
}

void Codegen::emitIfCF(node_t node, BB& bbt, BB& bbf)
{
    const auto& children = m_Ast.getChildren(node);

    emitCF(children[0], bbt, bbf);

    m_Builder.SetInsertPoint(&bbt);
    emitStmt(children[1]);

    BB* prev = m_Builder.GetInsertBlock();
    if (prev->getTerminator() == nullptr)
    {
        m_Builder.SetInsertPoint(prev);
        m_Builder.CreateBr(&bbf);
    }

    m_Builder.SetInsertPoint(&bbf);
}

void Codegen::emitWhileCF(node_t node, BB& bbt, BB& bbf)
{
    const auto& children = m_Ast.getChildren(node);

    BB* cond = createBasicBlock("cond");

    // this is a special case for multi-entrance loops
    if (!m_Builder.GetInsertBlock()->getTerminator())
        m_Builder.CreateBr(cond);

    m_Builder.SetInsertPoint(cond);

    // emit cond
    emitCF(children[0], bbt, bbf);

    // we keep track of the loop condition block and loop exit block
    // this information is needed to implement break and continue statements
    m_Loops.push_back({cond, &bbf});

    // emit loop body
    m_Builder.SetInsertPoint(&bbt);
    emitStmt(children[1]);

    m_Loops.pop_back();

    // emit branch back to cond
    if (!m_Builder.GetInsertBlock()->getTerminator())
        m_Builder.CreateBr(cond);

    m_Builder.SetInsertPoint(&bbf);
}

// 1. eval lhs
// 2. if false goto bbf
// 3. eval rhs in new bb
// 4. if true goto bbt
// 5. else goto bbf
void Codegen::emitLogicalAndCF(node_t node, BB& bbt, BB& bbf)
{
    const auto& children = m_Ast.getChildren(node);

    BB* bb = createBasicBlock();

    emitCF(children[0], *bb, bbf);

    m_Builder.SetInsertPoint(bb);
    emitCF(children[1], bbt, bbf);
}

void Codegen::emitLogicalOrCF(node_t node, BB& bbt, BB& bbf)
{
    const auto& children = m_Ast.getChildren(node);

    BB* bb = createBasicBlock();

    emitCF(children[0], bbt, *bb);

    m_Builder.SetInsertPoint(bb);
    emitCF(children[1], bbt, bbf);
}

// ***************************************
// *             STATEMENTS              *
// ***************************************

// the entry basic block corresponds to the first block of the current function
// the bb basic block is the current one, where this node is to be emitted to.
void Codegen::emitStmt(node_t node)
{
    switch (m_Ast.getNodeKind(node))
    {
        case NodeKind::EMPTY: break;
        case NodeKind::BLOCK: emitBlock(node); break;
        case NodeKind::DECLARATION: emitLocalDeclaration(node); break;
        case NodeKind::RETURN: emitReturn(node); break;
        case NodeKind::GOTO: emitGoto(node); break;
        case NodeKind::LABEL: emitLabel(node); break;
        case NodeKind::CONTINUE: emitContinue(node); break;
        case NodeKind::BREAK: emitBreak(node); break;
        case NodeKind::STRUCT_DECL: emitStructDeclaration(node); break;
        case NodeKind::EXPR_STATEMENT:
        {
            emitExprR(m_Ast.getChildren(node)[0]);
            break;
        }
        case NodeKind::WHILE:
        case NodeKind::IF:
        case NodeKind::IF_ELSE:
        {
            auto bbt = createBasicBlock();
            auto bbf = createBasicBlock();

            emitCF(node, *bbt, *bbf);
            break;
        }
        default: diagnostics::error("trying to emit: ", (int)m_Ast.getNodeKind(node));
    }
}

void Codegen::emitBlock(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    for (size_t i = 0; i < children.size(); ++i)
    {
        emitStmt(children[i]);

        // who tf writes such code. not even us would do that..
        // we are dealing here with a return/goto in the middle of the block.
        // but this might be legit if there is a label afterwards.
        // this basic block is probably unreachable -- but yea
        // thats something for the optimizer to deal with :)
        if (m_Builder.GetInsertBlock()->getTerminator())
        {
            auto bb = createBasicBlock();
            m_Builder.SetInsertPoint(bb);
        }
    }
}

void Codegen::emitReturn(node_t node)
{
    const auto& children = m_Ast.getChildren(node);

    if (children.size() == 0)
    {
        m_Builder.CreateRetVoid();
    }
    else
    {
        auto retType = m_Builder.getCurrentFunctionReturnType();
        const auto val = emitExprR(children[0], retType);
        m_Builder.CreateRet(val);
    }
}

void Codegen::emitLocalDeclaration(node_t node)
{
    const auto symbol = m_Ast.getContentId(node);
    const auto name = *m_SymbolTable[symbol];
    auto type = convertType(m_Ast.getType(node));

    // we safe the current block, get the entry block
    // insert an alloca into the entry block
    // and then reset the builder to point at the prev block
    BB* bb = m_Builder.GetInsertBlock();
    BB* entry = &m_Function->getEntryBlock();
    m_Builder.SetInsertPoint(entry);

    auto value = m_Builder.CreateAlloca(type, ADDR_SPACE, nullptr, name);

    m_Builder.SetInsertPoint(bb);

    // in the end we have to update our infos
    // to be able to lookup the variable in the future
    registerLocalVariable({symbol, value});

    // extra support for immediate initialization
    //      int x = 42;
    const auto& children = m_Ast.getChildren(node);

    assert(children.size() != 1 && "DIRECT INITIALIZATION NOT SUPPORTED!");
}

void Codegen::emitGoto(node_t node)
{
    const auto symbol = m_Ast.getContentId(node);

    BB* bb = lookupLabel(symbol);

    if (bb == nullptr)
    {
        // the label we are gonna jump to doesnt yet have basic block
        // so we create a new one for it and store it in our info

        // auto name = m_SymbolTable[symbol];
        bb = createBasicBlock();
        registerLabel({symbol, bb});
    }

    m_Builder.CreateBr(bb);
}

void Codegen::emitLabel(node_t node)
{
    auto symbol = m_Ast.getContentId(node);

    BB* bb = lookupLabel(symbol);

    if (bb == nullptr)
    {
        // maybe another goto already created a label for us.
        // but if not we have to do it ourselves

        // auto name = m_SymbolTable[symbol];
        bb = createBasicBlock();
        registerLabel({symbol, bb});

        // we terminate the above block and now continue emitting in the newly created one
        m_Builder.CreateBr(bb);
    }
    else
    {
        if (!m_Builder.GetInsertBlock()->getTerminator())
            m_Builder.CreateBr(bb);
    }

    m_Builder.SetInsertPoint(bb);

    auto child = m_Ast.getChildren(node)[0];
    emitStmt(child);
}

void Codegen::emitContinue([[maybe_unused]] node_t node)
{
    auto bb = m_Loops.back().first;

    m_Builder.CreateBr(bb);
}

void Codegen::emitBreak([[maybe_unused]] node_t node)
{
    auto bb = m_Loops.back().second;

    m_Builder.CreateBr(bb);
}
