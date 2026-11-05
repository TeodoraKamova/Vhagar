#pragma once
#include "NodeUtils.h"

#include <iostream>
#include <stdint.h>
#include <typeinfo>

// If the 3rd bit is set, it is unsigned.
// If the 3rd bit is removed, the type is converted to its signed counterpart
enum class TypeMap : uint16_t
{
    UNDEFINED = 0,
    UNSIGNED_TYPE = 1 << 0,
    CHAR_TYPE = 1 << 2,
    INT_TYPE = 1 << 3,
    UINT_TYPE = INT_TYPE | UNSIGNED_TYPE,
    UCHAR_TYPE = CHAR_TYPE | UNSIGNED_TYPE,
    INTEGER_TYPE = INT_TYPE | UINT_TYPE | CHAR_TYPE | UCHAR_TYPE,
    NULLPTR_TYPE = 1 << 4 | INTEGER_TYPE | UINT_TYPE, // TODO: correct?
    POINTER_TYPE = NULLPTR_TYPE | 1 << 5,
    SCALAR_TYPE = INTEGER_TYPE | POINTER_TYPE,
    VOID_TYPE = 1 << 6,
    FUNCTION_TYPE = 1 << 7,
    STRUCT_TYPE = 1 << 8,
    OBJECT_TYPE = SCALAR_TYPE | VOID_TYPE | STRUCT_TYPE,
    COMPLETE_TYPE = OBJECT_TYPE ^ VOID_TYPE,

    UNKNOWN,
};

enum class BasicTypes : uint8_t
{
    INT,
    UINT,
    CHAR,
    UCHAR,
    VOID,
};

constexpr TypeMap typeToTypeMap(BasicTypes ty, bool isNullPtr)
{
    TypeMap tm = TypeMap::INT_TYPE;
    switch (ty)
    {
        case BasicTypes::INT: tm = TypeMap::INT_TYPE; break;
        case BasicTypes::UINT: tm = TypeMap::UINT_TYPE; break;
        case BasicTypes::CHAR: tm = TypeMap::CHAR_TYPE; break;
        case BasicTypes::UCHAR: tm = TypeMap::UCHAR_TYPE; break;
        case BasicTypes::VOID: tm = TypeMap::VOID_TYPE; break;
    }

    if (isNullPtr)
        tm = (TypeMap)((unsigned)tm | (unsigned)TypeMap::NULLPTR_TYPE);

    return tm;
}

constexpr size_t INT_SIZE = 4;
constexpr size_t CHAR_SIZE = 1;
constexpr size_t VOID_SIZE = 1;
constexpr size_t POINTER_SIZE = 8;
constexpr size_t FUNCTION_SIZE = 8;

class Type
{
protected:
    const TypeMap m_TypeMap;

    constexpr Type(BasicTypes ty, bool isNullPtr) : m_TypeMap(typeToTypeMap(ty, isNullPtr)) {}
    constexpr Type(TypeMap typeMap) : m_TypeMap(typeMap) {}

public:
    virtual ~Type(void) {}

    virtual size_t sizeOf(void) const = 0;
    // virtual bool accepts(const Type& type) const = 0;
    virtual bool operator==(const Type& other) const = 0;

    constexpr TypeMap getTypeMap(void) const { return m_TypeMap; }
    bool isOfType(TypeMap ty) const { return ((unsigned)m_TypeMap & (unsigned)ty) != 0; }
    bool operator!=(const Type& other) const { return !(*this == other); }
};

struct identifier_info
{
    uint32_t identifier : 31;
    uint32_t implicitAsterix : 1;
    uint32_t offset;
};

constexpr bool operator==(const identifier_info& lhs, const identifier_info& rhs)
{
    return lhs.identifier == rhs.identifier && lhs.offset == rhs.offset;
}

constexpr identifier_info nullident = identifier_info{0, 0, 0};

// holds information of a struct member
struct MemberField
{
    const ::Type* type;
    struct
    {
        uint32_t contentId : 31;
        uint32_t implicitAsterix : 1;
    };
    uint32_t offset;

    MemberField(const ::Type* type, const identifier_info idf)
        : type(type), contentId(idf.identifier), implicitAsterix(idf.implicitAsterix),
          offset(idf.offset)
    {
    }

    MemberField(MemberField&& mf)
        : type(mf.type), contentId(mf.contentId), implicitAsterix(mf.implicitAsterix),
          offset(mf.offset)
    {
    }

    constexpr void operator=(const MemberField& other)
    {
        type = other.type;
        contentId = other.contentId;
        implicitAsterix = other.implicitAsterix;
        offset = other.offset;
    }
    constexpr void setType(const Type* t) { type = t; }
};

// using nary_types_t = NaryElement<const Type*>;
using nary_members_t = NaryElement<MemberField>;

class BaseType : public ::Type
{
private:
    const BasicTypes m_Ty;

public:
    constexpr BaseType(BasicTypes ty, bool isNullPtr = false) : Type(ty, isNullPtr), m_Ty(ty) {}

    BasicTypes getKind(void) const { return m_Ty; }

    size_t sizeOf(void) const
    {
        switch (m_Ty)
        {
            case BasicTypes::INT:
            case BasicTypes::UINT: return INT_SIZE;
            case BasicTypes::CHAR:
            case BasicTypes::UCHAR: return CHAR_SIZE;
            case BasicTypes::VOID:
            default: return VOID_SIZE;
        }
    }

    // bool accepts(const Type& type) const
    // {
    //     switch (m_Ty)
    //     {
    //         case BasicTypes::INT:
    //         case BasicTypes::UINT:
    //         case BasicTypes::CHAR:
    //         case BasicTypes::UCHAR: return type.isOfType(TypeMap::INTEGER_TYPE);
    //         case BasicTypes::VOID: // type checking should prevent this
    //         default: return operator==(type);
    //     }
    // }

    bool operator==(const Type& oty) const
    {
        if (typeid(*this) != typeid(oty))
            return false;

        const BaseType& other = static_cast<const BaseType&>(oty);
        return m_Ty == other.m_Ty;
    }
};

class StructType : public ::Type
{
private:
    nary_members_t m_Members;
    const uint32_t m_Name;

public:
    constexpr StructType(uint32_t name, nary_members_t&& members)
        : Type(TypeMap::STRUCT_TYPE), m_Members(std::move(members)), m_Name(name)
    {
    }

    uint32_t getName(void) const { return m_Name; }
    const nary_members_t& getMembers(void) const { return m_Members; }
    nary_members_t& getMembers(void) { return m_Members; }

    int findMember(uint32_t symbol) const
    {
        for (size_t i = 0; i < m_Members.size(); ++i)
            if (m_Members[i].contentId == symbol)
                return i;

        return -1;
    }

    size_t sizeOf(void) const
    {
        size_t acc = 0;
        for (uint32_t i = 0; i < m_Members.size(); i++)
            acc += m_Members[i].type->sizeOf();
        return acc == 0 ? 1 : acc;
    }

    // bool accepts(const Type& type) const { return operator==(type); }
    bool operator==(const Type& oty) const
    {
        if (typeid(*this) != typeid(oty))
            return false;

        const StructType& other = static_cast<const StructType&>(oty);
        if (m_Name != other.m_Name)
            return false;

        for (uint32_t i = 0; i < m_Members.size(); i++)
        {
            const auto& m1 = m_Members[i];
            const auto& m2 = other.m_Members[i];

            if (m1.contentId != m2.contentId)
                return false;

            if (m1.type != m2.type)
                return false;
        }

        return true;
    }
};

class FunctionType : public ::Type
{
private:
    const Type* m_ReturnType;
    const nary_members_t m_Params;

public:
    constexpr FunctionType(const Type* returnType, nary_members_t&& parameters)
        : Type(TypeMap::FUNCTION_TYPE), m_ReturnType(returnType), m_Params(std::move(parameters))
    {
    }

    const Type* getReturnType(void) const { return m_ReturnType; }
    const nary_members_t& getParameters(void) const { return m_Params; }
    void setNewReturnType(const Type* returnType) { m_ReturnType = returnType; }

    size_t sizeOf(void) const { return FUNCTION_SIZE; }
    // bool accepts(const Type& type) const { return operator==(type); }
    bool operator==(const Type& oty) const;
};

constexpr bool isCompatibleType(const ::Type* t1, const ::Type* t2, bool pointer);

class PointerType : public ::Type
{
    friend class Parser;

private:
    const Type* m_PointsTo;

public:
    constexpr PointerType(const Type* pointsTo) : Type(TypeMap::POINTER_TYPE), m_PointsTo(pointsTo)
    {
    }

    const Type* getPointsTo(void) const { return m_PointsTo; }
    void setPointsTo(const Type* pointsTo) { m_PointsTo = pointsTo; }

    size_t sizeOf(void) const { return POINTER_SIZE; }
    // bool accepts(const Type& type) const { return type.isOfType(TypeMap::POINTER_TYPE); }
    bool operator==(const Type& oty) const
    {
        if (typeid(*this) != typeid(oty))
            return false;

        const PointerType& other = static_cast<const PointerType&>(oty);
        if (m_PointsTo->getTypeMap() == TypeMap::STRUCT_TYPE &&
            other.m_PointsTo->getTypeMap() == TypeMap::STRUCT_TYPE)
        {
            const StructType* nty1 = static_cast<const StructType*>(m_PointsTo);
            const StructType* nty2 = static_cast<const StructType*>(other.m_PointsTo);
            return nty1->getName() == nty2->getName();
        }
        if (m_PointsTo->getTypeMap() == TypeMap::FUNCTION_TYPE &&
            other.m_PointsTo->getTypeMap() == TypeMap::FUNCTION_TYPE)
        {
            const FunctionType* nty1 = static_cast<const FunctionType*>(m_PointsTo);
            const FunctionType* nty2 = static_cast<const FunctionType*>(other.m_PointsTo);
            return nty1 == nty2;
        }
        if (m_PointsTo->getTypeMap() == TypeMap::POINTER_TYPE &&
            other.m_PointsTo->getTypeMap() == TypeMap::POINTER_TYPE)
        {
            const PointerType* nty1 = static_cast<const PointerType*>(m_PointsTo);
            const PointerType* nty2 = static_cast<const PointerType*>(other.m_PointsTo);
            return nty1 == nty2;
        }
        return m_PointsTo == other.m_PointsTo;
    }
};

constexpr bool isPointerToStruct(const ::Type* t1);

// used for parsing, when we only know the name of a type.
// Will be replaced by the correct type once we type-check.
class NamedType : public ::Type
{
private:
    const uint32_t m_Name;

public:
    constexpr NamedType(uint32_t name) : Type(TypeMap::STRUCT_TYPE), m_Name(name) {}

    // TODO: make these 3 funs correct
    uint32_t getName(void) const { return m_Name; }
    size_t sizeOf(void) const { return 0; }
    bool accepts(const Type& type) const
    {
        (void)type;
        return false;
    }
    bool operator==(const Type& oty) const
    {
        (void)oty;
        return false;
    }
};

// check if tm1 is smaller/equal than tm2
constexpr bool isOfTypeMap(TypeMap tm1, TypeMap tm2)
{
    return ((int)(tm1) & (int)(tm2)) == (int)(tm1);
}

constexpr const Type* getMax(const Type* t1, const Type* t2)
{
    int tm1 = (int)t1->getTypeMap();
    int tm2 = (int)t2->getTypeMap();

    return tm1 >= tm2 ? t1 : t2;
}

constexpr bool isCompatibleType(const ::Type* t1, const ::Type* t2, bool pointer = false)
{
    TypeMap tm1 = t1->getTypeMap();
    TypeMap tm2 = t2->getTypeMap();
    // same type
    if (tm1 == tm2 && tm1 != TypeMap::POINTER_TYPE)
        return true;
    // void* compatibility
    if (pointer && (tm1 == TypeMap::VOID_TYPE || tm2 == TypeMap::VOID_TYPE))
        return true;
    // char - int
    if (!pointer && isOfTypeMap(tm1, TypeMap::INTEGER_TYPE) &&
        isOfTypeMap(tm2, TypeMap::INTEGER_TYPE))
        return true;
    // pointer types
    if (tm1 == TypeMap::POINTER_TYPE && tm2 == TypeMap::POINTER_TYPE)
    {
        const ::PointerType* pt1 = static_cast<const ::PointerType*>(t1);
        const ::PointerType* pt2 = static_cast<const ::PointerType*>(t2);
        return isCompatibleType(pt1->getPointsTo(), pt2->getPointsTo(), true);
    }
    if (tm1 == TypeMap::FUNCTION_TYPE && tm2 == TypeMap::FUNCTION_TYPE)
    {
        const ::FunctionType* ft1 = static_cast<const ::FunctionType*>(t1);
        const ::FunctionType* ft2 = static_cast<const ::FunctionType*>(t2);
        return ft1 == ft2;
    }
    if (tm1 == TypeMap::STRUCT_TYPE && tm2 == TypeMap::STRUCT_TYPE)
    {
        uint32_t name1 = 0;
        uint32_t name2 = 0;
        if (typeid(*t1) == typeid(NamedType))
        {
            const ::StructType* st1 = static_cast<const ::StructType*>(t1);
            name1 = st1->getName();
        }
        else
        {
            const NamedType* st1 = static_cast<const NamedType*>(t1);
            name1 = st1->getName();
        }
        if (typeid(*t2) == typeid(NamedType))
        {
            const ::StructType* st1 = static_cast<const ::StructType*>(t2);
            name2 = st1->getName();
        }
        else
        {
            const ::NamedType* st1 = static_cast<const ::NamedType*>(t2);
            name2 = st1->getName();
        }
        if (name1 != 0 && name2 == name1)
            return true;
        // TODO check if it is in the structs env
    }
    return false;
};
constexpr bool isPointerToFunc(const ::Type* t1)
{
    const ::Type* pt = t1;
    while (pt->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        const ::PointerType* pt1 = static_cast<const ::PointerType*>(pt);
        pt = pt1->getPointsTo();
    }
    return pt->getTypeMap() == TypeMap::FUNCTION_TYPE;
};
constexpr bool isVoidPointer(const ::Type* t1)
{
    const ::Type* pt = t1;
    while (pt->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        const ::PointerType* pt1 = static_cast<const ::PointerType*>(pt);
        pt = pt1->getPointsTo();
    }
    return pt->getTypeMap() == TypeMap::VOID_TYPE;
};
constexpr bool isPointerToStruct(const ::Type* t1)
{
    const ::Type* pt = t1;
    while (pt->getTypeMap() == TypeMap::POINTER_TYPE)
    {
        const ::PointerType* pt1 = static_cast<const ::PointerType*>(pt);
        pt = pt1->getPointsTo();
    }
    return pt->getTypeMap() == TypeMap::STRUCT_TYPE;
};

std::ostream& operator<<(std::ostream& os, const ::Type* type);
