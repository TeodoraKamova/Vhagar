#include "SymbolTable.h"

SymbolTable::SymbolTable() : m_Map(), m_ReverseMap(), m_MaxSymbolId(1) {}

uint32_t SymbolTable::insert(std::string_view value)
{
    // first we check if the element is already in the hash
    // find() has average runnig time of O(1) i think
    const auto it = m_Map.find(value);
    if (it != m_Map.end())
        return it->second;

    // otherwise we have to creata a new entry and
    // also add it to the reverse map
    m_Map[value] = m_MaxSymbolId;
    m_ReverseMap.emplace_back(std::move(value));

    return m_MaxSymbolId++;
}

void SymbolTable::reserve(size_t capacity)
{
    // reserve some space for the maps
    // yes, this may waste a couple of bytes, but will prevent
    // a lot of re-allocations
    m_Map.reserve(capacity);
    m_ReverseMap.reserve(capacity);
}

constexpr std::string_view CUSTOM_LABEL =
    "custom label only used for unique ids, but the string does not exist";

const std::string_view* SymbolTable::operator[](uint32_t key) const
{
    if (key == 0)
        return nullptr;

    if (key > m_ReverseMap.size())
        return &CUSTOM_LABEL;

    return m_ReverseMap.data() + key - 1;
}
