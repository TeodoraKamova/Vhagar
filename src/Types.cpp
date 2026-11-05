#include "Types.h"

bool FunctionType::operator==(const Type& oty) const
{
    if (typeid(*this) != typeid(oty))
        return false;

    const FunctionType& other = static_cast<const FunctionType&>(oty);
    if (typeid(*m_ReturnType) == typeid(PointerType) &&
        typeid(*other.m_ReturnType) == typeid(PointerType))
    {
        const PointerType* pty1 = static_cast<const PointerType*>(m_ReturnType);
        const PointerType* pty2 = static_cast<const PointerType*>(other.m_ReturnType);
        if (!isCompatibleType(pty1, pty2, true))
            return false;
        return true;
    }
    if (m_ReturnType != other.m_ReturnType)
        return false;

    if (m_Params.size() != other.m_Params.size())
        return false;

    for (uint32_t i = 0; i < m_Params.size(); i++)
    {
        auto t1 = m_Params[i].type;
        auto t2 = other.m_Params[i].type;
        if (t1->getTypeMap() == TypeMap::POINTER_TYPE && t2->getTypeMap() == TypeMap::POINTER_TYPE)
        {
            const PointerType* pty1 = static_cast<const PointerType*>(t1);
            const PointerType* pty2 = static_cast<const PointerType*>(t2);
            if (!isCompatibleType(pty1->getPointsTo(), pty2->getPointsTo(), true))
                return false;
        }
        else if (*t1 != *t2)
            return false;
    }

    return true;
}

std::ostream& operator<<(std::ostream& os, const Type* type)
{
    if (type == nullptr)
        diagnostics::error("Tried to print type, but gave nullptr!");

    if (typeid(*type) == typeid(BaseType))
    {
        const BaseType* bty = static_cast<const BaseType*>(type);
        switch (bty->getKind())
        {
            case BasicTypes::VOID: return os << "void";
            case BasicTypes::CHAR: return os << "char";
            case BasicTypes::INT: return os << "int";
            case BasicTypes::UCHAR: return os << "unsigned char";
            case BasicTypes::UINT: return os << "unsigned";
            default: return os << "undefined";
        }
    }
    else if (typeid(*type) == typeid(PointerType))
    {
        const PointerType* pty = static_cast<const PointerType*>(type);
        if (typeid(*pty->getPointsTo()) == typeid(StructType) ||
            typeid(*pty->getPointsTo()) == typeid(NamedType))
            return (os << "(*");
        if (typeid(*pty->getPointsTo()) == typeid(FunctionType))
            return (os << " (*");
        return (os << pty->getPointsTo() << "(*");
    }
    else if (typeid(*type) == typeid(FunctionType))
    {
        const FunctionType* fty = static_cast<const FunctionType*>(type);
        os << fty->getReturnType();
    }
    // else if (typeid(*type) == typeid(StructType))
    //{
    //     const StructType* sty = static_cast<const StructType*>(type);
    //     const auto& members = sty->getMembers();
    //     const uint32_t name = sty->getName();
    //
    //     os << "struct " << name << "\n{";
    //     for (size_t i = 0; i < members.size(); i++)
    //     {
    //         const auto& m = members[i];
    //         std::cout << m.type << ' ' << m.contentId << ";\n";
    //     }
    //     return os << "}\n";
    // }
    else
    {
        diagnostics::todo(
            "std::ostream& operator<<(std::ostream& os, const Type* type), implement ",
            typeid(*type).name());
    }

    return os;
}
