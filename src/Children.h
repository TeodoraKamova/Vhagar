#pragma once
#include "Diagnostics.h"
#include "NAryElement.h"

#include <array>
#include <tuple>
#include <type_traits>
#include <variant>

typedef uint32_t node_t;

using none_child_t = std::tuple<>;
using unary_children_t = node_t;
using binary_children_t = std::array<node_t, 2>;
using ternary_children_t = std::array<node_t, 3>;
using quaternary_children_t = std::array<node_t, 4>;
using nary_children_t = NaryElement<node_t>;
class children_t : public std::variant<none_child_t, unary_children_t, binary_children_t,
                                       ternary_children_t, quaternary_children_t, nary_children_t>
{
public:
    using std::variant<none_child_t, unary_children_t, binary_children_t, ternary_children_t,
                       quaternary_children_t, nary_children_t>::variant;

public:
    // TODO: do we want to handle out of range?
    const node_t& operator[](size_t n) const
    {

        switch (index())
        {
            case 0: throw std::out_of_range("Index is out of range for indexing none_child_t");
            case 1:
            {
                if (n == 0)
                    return std::get<unary_children_t>(*this);

                throw std::out_of_range("Index is out of range for indexing unary_children_t");
            }
            case 2:
            {
                if (n < 2)
                    return std::get<binary_children_t>(*this)[n];

                throw std::out_of_range("Index is out of range for indexing binary_children_t");
            }
            case 3:
            {
                if (n < 3)
                    return std::get<ternary_children_t>(*this)[n];

                throw std::out_of_range("Index is out of range for indexing ternary_children_t");
            }
            case 4:
            {
                if (n < 4)
                    return std::get<quaternary_children_t>(*this)[n];

                throw std::out_of_range("Index is out of range for indexing quaternary_children_t");
            }
            case 5: return std::get<nary_children_t>(*this)[(uint32_t)n];

            default: throw std::invalid_argument("Invalid index provided in operator[]");
        }
    }

    size_t size(void) const
    {
        if (index() == 5)
            return std::get<nary_children_t>(*this).size();

        return index();
    }

    void push_back(node_t n)
    {
        int i = index();
        switch (i)
        {
            case 0: operator=(unary_children_t{n}); break;
            case 1: operator=(binary_children_t{operator[](0), n}); break;
            case 2: operator=(ternary_children_t{operator[](0), operator[](1), n}); break;
            case 3:
                operator=(quaternary_children_t{operator[](0), operator[](1), operator[](2), n});
                break;
            case 4:
            {
                nary_children_t newChildren(4);
                for (unsigned i = 0; i < 3; i++)
                    newChildren.push_back(operator[](i));

                newChildren.push_back(n);
                operator=(std::move(newChildren));
                break;
            }
            case 5:
            default: std::get<nary_children_t>(*this).push_back(n); break;
        }
    }
};
