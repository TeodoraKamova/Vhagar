#pragma once
#include "Diagnostics.h"

#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <utility>

template <typename _Ty> struct NaryElement
{
private:
    _Ty* m_Nodes;
    uint32_t m_Size;
    uint32_t m_Capacity;

public:
    /// Be careful: WE DO NOT INITIALIZE ANY MEMORY!
    NaryElement(uint32_t n = 0, bool reserve = true)
        : m_Nodes((n == 0) ? nullptr : (_Ty*)std::malloc(n * sizeof(_Ty))), m_Size(reserve ? 0 : n),
          m_Capacity(n)
    {
    }

    /// Take over ownership
    constexpr NaryElement(_Ty* nodes, uint32_t size, uint32_t capacity)
        : m_Nodes(nodes), m_Size(size), m_Capacity(capacity)
    {
    }

    constexpr NaryElement(NaryElement&& other) noexcept
        : m_Nodes(other.m_Nodes), m_Size(other.m_Size), m_Capacity(other.m_Capacity)
    {
        other.m_Nodes = nullptr;
        other.m_Size = 0;
        other.m_Capacity = 0;

        // std::cout << "called NaryChildren(AnatyChildren&& other)" << std::endl;
    }

    ~NaryElement() { std::free((void*)m_Nodes); }

    NaryElement& operator=(NaryElement&& other) noexcept
    {
        if (this != &other)
        {
            std::free((void*)m_Nodes);
            m_Nodes = other.m_Nodes;
            m_Size = other.m_Size;
            m_Capacity = other.m_Capacity;
        }

        return *this;
    }

    const _Ty& operator[](uint32_t i) const { return m_Nodes[i]; }
    _Ty& operator[](uint32_t i) { return m_Nodes[i]; }

    _Ty* data(void) const { return m_Nodes; }
    size_t size(void) const { return m_Size; }
    size_t capacity(void) const { return m_Capacity; }

    _Ty* begin() { return m_Nodes; }
    const _Ty* begin() const { return m_Nodes; }

    _Ty* end() { return m_Nodes + m_Size; }
    const _Ty* end() const { return m_Nodes + m_Size; }

    void push_back(_Ty node)
    {
        // need reallocation
        if (m_Size >= m_Capacity)
        {
            // we allocate 50% more memory with a default minimum size of 5
            const uint32_t newSize = std::max<uint32_t>((m_Size * 3) >> 1, 5);
            // node_t* newData = new node_t[newSize == 0 ? 8 : newSize];
            // std::copy(m_Nodes, m_Nodes + m_Size, newData);
            // delete[] m_Nodes;
            // m_Nodes = newData;

            _Ty* newData = (_Ty*)std::realloc((void*)m_Nodes, newSize * sizeof(_Ty));
            if (newData == nullptr)
                // TODO: what do we do? Panic?
                diagnostics::error("error reallocating buffer for NaryChildren.");

            m_Nodes = newData;
            m_Capacity = newSize;
        }

        m_Nodes[m_Size++] = node;
    }

    void pop_back(void)
    {
        if (m_Size != 0)
            m_Size--;
    }
};

template <typename T> constexpr bool operator==(const NaryElement<T>& a, const NaryElement<T>& b)
{
    if (a.size() != b.size())
        return false;

    for (size_t i = 0; i < a.size(); i++)
        if (a[i] != b[i])
            return false;

    return true;
}

template <typename T> constexpr bool operator!=(const NaryElement<T>& a, const NaryElement<T>& b)
{
    return !(a == b);
}
