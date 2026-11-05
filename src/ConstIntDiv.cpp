#include "ConstIntDiv.h"
#include <cmath>
#include <vector>

// algorithm from "hackers delight"

// magic signed
struct ms
{
    int M; // Magic number
    int s; // and shift amount.
};

// Must have 2 <= d <= 2**31-1
// or -2**31 <= d <= -2.
// stolen from hackers delight
struct ms magic(int32_t d)
{
    int32_t p;
    uint32_t ad, anc, delta, q1, r1, q2, r2, t;
    const uint32_t two31 = 0x80000000; // 2**31.
    struct ms mag;
    ad = abs(d);
    t = two31 + ((uint32_t)d >> 31);
    anc = t - 1 - t % ad;  // Absolute value of nc.
    p = 31;                // Init. p.
    q1 = two31 / anc;      // Init. q1 = 2**p/|nc|.
    r1 = two31 - q1 * anc; // Init. r1 = rem(2**p, |nc|).
    q2 = two31 / ad;       // Init. q2 = 2**p/|d|.
    r2 = two31 - q2 * ad;  // Init. r2 = rem(2**p, |d|).
    do
    {
        p = p + 1;
        q1 = 2 * q1; // Update q1 = 2**p/|nc|.
        r1 = 2 * r1; // Update r1 = rem(2**p, |nc|).
        if (r1 >= anc)
        {                // (Must be an unsigned
            q1 = q1 + 1; // comparison here.)
            r1 = r1 - anc;
        }
        q2 = 2 * q2; // Update q2 = 2**p/|d|.
        r2 = 2 * r2; // Update r2 = rem(2**p, |d|).
        if (r2 >= ad)
        {                // (Must be an unsigned
            q2 = q2 + 1; // comparison here.)
            r2 = r2 - ad;
        }
        delta = ad - r2;
    } while (q1 < delta || (q1 == delta && r1 == 0));
    mag.M = q2 + 1;
    if (d < 0)
        mag.M = -mag.M; // Magic number and
    mag.s = p - 32;     // shift amount to return.
    return mag;
}

void ConstIntDiv::run(void)
{
    std::vector<llvm::BinaryOperator*> instructions;
    for (llvm::Function& fun : m_Module)
        // skip declarations
        if (!fun.isDeclaration())
            for (llvm::BasicBlock& bb : fun)
            {
                instructions.clear();
                for (llvm::Instruction& inst : bb)
                    if (llvm::BinaryOperator* bin = llvm::dyn_cast<llvm::BinaryOperator>(&inst))
                        instructions.push_back(bin);

                for (llvm::BinaryOperator* inst : instructions)
                    handleInstruction(inst);
            }
}

enum class Which
{
    First,
    Last,
};

/*
constexpr int getBitPos(int32_t c)
{
    if (c == 0)
        return -1;

    for (unsigned i = 0; i < 32; i++)
        if ((uint32_t)c & (1U << i))
            return i;

    // cosmic bit flip, big problem
    return -2;
}
*/

// retuns (counting from right) the last 1 bit if the right most 1-field
//  0b00101011100
//
template <Which _Method = Which::First> constexpr int getBitPos(int32_t c)
{
    if (c == 0)
        return -1;

    for (unsigned i = 0; i < 32; i++)
    {
        if ((uint32_t)c & (1U << i))
        {
            if constexpr (_Method == Which::First)
                return i;

            for (; (i < 32) && ((uint32_t)c & (1U << i)); i++)
                ;
            return i - 1;
        }
    }

    // cosmic bit flip, big problem
    return -2;
}

constexpr unsigned popcount(uint32_t c)
{
    unsigned res = 0;
    for (unsigned i = 0; i < 32; i++)
        if (c & (1U << i))
            res++;
    return res;
}

// removes the last bit field, if no bit field returns 0
constexpr uint32_t withoutLastField(uint32_t c)
{
    uint32_t lsb = c & -c;
    for (; lsb != (1U << 31); lsb <<= 1)
        if ((c & lsb) == 0)
            break;

    return c & ~(lsb - 1);
}

// guessing the number of cycles needed to unroll the multiplication
// assuming the cpu can do 2 logcal instrs at a time
// score system: (only) power of 2: 1
// bit field of size 1:             1
// bit fild of any other size:      2
// negaitive                        +1
constexpr unsigned scoreForMultipl(int32_t c)
{
    unsigned res = c < 0 ? 1 : 0;
    uint32_t abs = c < 0 ? -c : c;
    if ((abs & (abs - 1)) == 0)
        return res + 1;

    for (; abs != 0; abs = withoutLastField(abs))
    {

        const int first = getBitPos<Which::First>(abs);
        const auto last = getBitPos<Which::Last>(abs);

        if (first == last)
            res += 1;
        else if (first == 0)
            res += 2;
        else
            res += 2;
    }

    return res;
}

void ConstIntDiv::handleInstruction(llvm::BinaryOperator* inst)
{
    if (inst->getOpcode() == llvm::Instruction::SDiv)
        return handleSDiv(inst);

    if (inst->getOpcode() == llvm::Instruction::SRem)
        return handleSRem(inst);

    if (inst->getOpcode() == llvm::Instruction::Mul)
        return handleSMul(inst);
}

inline llvm::Value* getDivisiorHelper(int32_t d, llvm::IRBuilder<>& builder, llvm::Value* lhs)
{
    const ms mags = magic(d);
    const int M = mags.M;
    const int s = mags.s;

    // could we post-pone the shift to the end?
    llvm::Value* lhs64 = builder.CreateSExt(lhs, llvm::Type::getInt64Ty(builder.getContext()));
    llvm::Value* tmp = builder.CreateMul(lhs64, llvm::ConstantInt::get(lhs64->getType(), M));
    tmp = builder.CreateLShr(tmp, llvm::ConstantInt::get(lhs64->getType(), 32));
    tmp = builder.CreateTrunc(tmp, llvm::Type::getInt32Ty(builder.getContext()));

    if (d > 0 && M < 0)
        tmp = builder.CreateAdd(tmp, lhs);

    else if (d < 0 && M > 0)
        tmp = builder.CreateSub(tmp, lhs);

    if (s > 0)
        tmp = builder.CreateAShr(tmp, llvm::ConstantInt::get(lhs->getType(), s));

    lhs = builder.CreateLShr(tmp, llvm::ConstantInt::get(lhs->getType(), 31));
    llvm::Value* res = builder.CreateAdd(tmp, lhs);

    return res;
}

void ConstIntDiv::handleSDiv(llvm::BinaryOperator* inst)
{
    // only on 32 bit words
    if (!inst->getType()->isIntegerTy(32))
        return;

    // and we need to know the rhs at compuile time (thx sccp)
    const llvm::Constant* rhs = llvm::dyn_cast<llvm::Constant>(inst->getOperand(1));
    if (!rhs)
        return;

    // lets go!
    llvm::Value* lhs = inst->getOperand(0);
    const int32_t d = rhs->getUniqueInteger().getSExtValue();
    const unsigned abs = d < 0 ? -d : d;
    llvm::IRBuilder<> builder(inst);

    // is it a power of 2?
    if ((abs & (abs - 1)) == 0)
    {
        const int pos = getBitPos(abs);
        if (pos == 0)
        {
            if (d < 0)
                lhs = builder.CreateNeg(lhs);
            inst->replaceAllUsesWith(lhs);
            inst->eraseFromParent();
        }

        else if (pos >= 0)
        {
            llvm::Value* isSigned =
                builder.CreateLShr(lhs, llvm::ConstantInt::get(lhs->getType(), 31));
            lhs = builder.CreateAdd(lhs, isSigned);

            llvm::Value* shiftedValue =
                builder.CreateAShr(lhs, llvm::ConstantInt::get(lhs->getType(), pos));

            if (d < 0)
                shiftedValue = builder.CreateNeg(shiftedValue);

            inst->replaceAllUsesWith(shiftedValue);
            inst->eraseFromParent();
        }

        return;
    }

    llvm::Value* res = getDivisiorHelper(d, builder, lhs);

    // remove old instruction
    inst->replaceAllUsesWith(res);
    inst->eraseFromParent();
}

void ConstIntDiv::handleSRem(llvm::BinaryOperator* inst)
{
    // only on 32 bit words
    if (!inst->getType()->isIntegerTy(32))
        return;

    // and we need to know the rhs at compuile time (thx sccp)
    const llvm::Constant* rhs = llvm::dyn_cast<llvm::Constant>(inst->getOperand(1));
    if (!rhs)
        return;

    // lets go!
    llvm::Value* lhs = inst->getOperand(0);
    llvm::Value* orgiginalLhs = inst->getOperand(0);
    const int32_t d = rhs->getUniqueInteger().getSExtValue();
    const unsigned abs = d < 0 ? -d : d;
    llvm::IRBuilder<> builder(inst);

    // is it a power of 2?
    if ((abs & (abs - 1)) == 0 && d >= 0)
    {
        const int k = getBitPos(abs);
        if (k == 0)
        {
            if (d < 0)
                lhs = llvm::ConstantInt::get(lhs->getType(), 0);

            inst->replaceAllUsesWith(lhs);
            inst->eraseFromParent();
        }

        else if (k >= 0)
        {

            llvm::Value* t = lhs;
            if (k - 1 != 0)
                t = builder.CreateAShr(lhs, llvm::ConstantInt::get(lhs->getType(), k - 1));
            if (32 - k != 0)
                t = builder.CreateLShr(t, llvm::ConstantInt::get(lhs->getType(), 32 - k));

            t = builder.CreateAdd(lhs, t);

            if ((uint32_t)(-(1 << k)) != (uint32_t)UINT32_MAX)
                t = builder.CreateBinOp(llvm::BinaryOperator::And, t,
                                        llvm::ConstantInt::get(lhs->getType(), -(1 << k)));

            llvm::Value* r = builder.CreateSub(lhs, t);

            inst->replaceAllUsesWith(r);
            inst->eraseFromParent();
        }

        return;
    }

    llvm::Value* div = getDivisiorHelper(d, builder, lhs);
    llvm::Value* res = builder.CreateBinOp(llvm::BinaryOperator::Mul, div,
                                           llvm::ConstantInt::get(lhs->getType(), d, true));
    res = builder.CreateSub(orgiginalLhs, res);

    // remove old instruction
    inst->replaceAllUsesWith(res);
    inst->eraseFromParent();
}

void ConstIntDiv::handleSMul(llvm::BinaryOperator* inst)
{
    // only on 32 bit words
    if (!inst->getType()->isIntegerTy(32))
        return;

    // and we need to know the rhs at compuile time (thx sccp)
    unsigned lhsIndex = 0;
    llvm::Constant const* cRhs = llvm::dyn_cast<llvm::Constant>(inst->getOperand(1));

    // ok, since multiplication is commutative, we can try again with the lhs, and just
    // switch sides if lhs is constant
    if (!cRhs)
    {
        cRhs = llvm::dyn_cast<llvm::Constant>(inst->getOperand(0));
        if (!cRhs)
            return;

        lhsIndex = 1;
    }

    // lets go!
    llvm::Value* lhs = inst->getOperand(lhsIndex);
    const int32_t d = cRhs->getUniqueInteger().getSExtValue();
    uint32_t abs = d < 0 ? -d : d;

    // the maximum number of instructions we are willing to make. Note
    // That many may be executed in paralell.
    const unsigned maxInsts = 3;
    const auto score = scoreForMultipl(abs);
    if (score > maxInsts)
        return;

    llvm::IRBuilder<> builder(inst);

    // up to `maxAdds` iterations
    llvm::Value *oldLhs = lhs, *acc = lhs;
    for (unsigned iteration = 0; true; iteration++)
    {
        int k = getBitPos<Which::First>(abs);
        const auto last = getBitPos<Which::Last>(abs), first = k;
        const auto width = last - first;

        // update abs mask
        abs = withoutLastField(abs);

        // multiply by zero
        if (k < 0)
            lhs = llvm::ConstantInt::get(lhs->getType(), 0);

        // multiply by one, so the result will be oldLhs
        else if (k == 0 && last == 0)
            lhs = oldLhs;

        // multiply by a bit-field of size > 1. eg 0b110
        else if (width != 0)
        {
            // 2 shifts can likely be done in paralel, then we only need to wait for the sub
            llvm::Value* s1 =
                first != 0
                    ? builder.CreateShl(oldLhs, llvm::ConstantInt::get(oldLhs->getType(), first))
                    : oldLhs;

            llvm::Value* s2 =
                builder.CreateShl(oldLhs, llvm::ConstantInt::get(oldLhs->getType(), last + 1));

            lhs = builder.CreateSub(s2, s1);
        }

        // number of trailing zeros is bigger: need to multily again
        else if (k > 0)
            lhs = builder.CreateShl(oldLhs, llvm::ConstantInt::get(oldLhs->getType(), k));

        // add values together
        if (iteration > 0)
            acc = lhs = builder.CreateAdd(lhs, acc);
        else
            acc = lhs;

        if (abs == 0)
            break;
    }

    if (d < 0)
        lhs = builder.CreateNeg(lhs);

    inst->replaceAllUsesWith(lhs);
    inst->eraseFromParent();
}
