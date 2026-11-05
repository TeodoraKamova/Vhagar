
#include "Backend.h"

#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Casting.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

static int getInstructionCount(Function& function)
{
    int ninstr = 0;

    for (BasicBlock& bb : function)
        ninstr += bb.size();

    return ninstr;
}

// we are breaking tests when allowing inlined functions
// to do pointer arithmetic and similar stuff on their arguments.
//
// as weird stuff counts:
//  - pointer offset calculations
//  - casting pointers to integers
//  - storing the pointer to memory
static bool doesWeirdStuffWithArguments(Function& function)
{
    for (auto& argument : function.args())
    {
        for (auto inst : argument.users())
        {
            if (isa<GetElementPtrInst>(inst))
                return true;

            if (isa<PtrToIntInst>(inst))
                return true;

            if (auto store = dyn_cast<StoreInst>(inst))
                if (store->getValueOperand() == &argument)
                    return true;
        }
    }

    return false;
}

// our analysis is very simple:
// if it doesnt call another function, it doesnt recurse
// we might add 1-hop exploration of the call graph in the future
static void markFunctionWithNoRecurse(Function& function)
{
    if (function.isDeclaration())
        return;

    for (auto& bb : function)
        for (auto& inst : bb)
            if (dyn_cast<CallInst>(&inst))
                return;

    function.addFnAttr(Attribute::NoRecurse);
}

static void markFunctionWithInlineHint(Function& function)
{
    if (function.isDeclaration())
        return;

    if (!function.doesNotRecurse())
        return;

    if (doesWeirdStuffWithArguments(function))
        return;

    // if # bb's is tiny, then its mostly straight line code
    // and it would probably benefit from being inlined
    // since this avoids the function call overhead
    if (function.size() <= 2)
        goto INLINE;

    if (getInstructionCount(function) <= 35)
        goto INLINE;

    return;

INLINE:
    function.addFnAttr(Attribute::InlineHint);
}

static void inlineCallInst(Value* value)
{
    auto inst = dyn_cast<CallInst>(value);

    if (!inst)
        return;

    auto caller = inst->getCaller();
    auto callee = inst->getCalledFunction();

    if (!callee || !callee->hasFnAttribute(Attribute::InlineHint))
        return;

    if (getInstructionCount(*caller) >= 1000)
        return;

    InlineFunctionInfo ifi;

    InlineFunction(*inst, ifi);
}

void Backend::inliner()
{
    for (auto& function : m_Module)
        markFunctionWithNoRecurse(function);

    for (auto& function : m_Module)
        markFunctionWithInlineHint(function);

    m_Buffer.clear();

    for (auto& function : m_Module)
    {
        for (auto& bb : function)
            for (auto& inst : bb)
                if (auto ci = dyn_cast<CallInst>(&inst))
                    m_Buffer.push_back(ci);

        for (auto ci : m_Buffer)
            inlineCallInst(ci);

        m_Buffer.clear();
    }
}
