#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using symbol_t = uint32_t;

class SymbolTable
{
private:
    std::unordered_map<std::string_view, uint32_t> m_Map;
    std::vector<std::string_view> m_ReverseMap;
    uint32_t m_MaxSymbolId;

public:
    SymbolTable();
    // ~SymbolTable();

    uint32_t insert(std::string_view value);
    const std::string_view* operator[](uint32_t key) const;

    void reserve(size_t capacity);

    size_t size(void) const { return m_Map.size(); }
    size_t capacity(void) const { return m_Map.bucket_count(); }
};
