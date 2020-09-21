/*
    *** Generic Algorithms
    *** src/templates/algorithms.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <optional>

// Perform an operation on every pair of elements in a container
template <class Iter, class Lam> void for_each_pair(Iter begin, Iter end, Lam lambda)
{
    int i = 0;
    for (auto elem1 = begin; elem1 != end; ++elem1, ++i)
    {
        int j = i;
        for (auto elem2 = elem1; elem2 != end; ++elem2, ++j)
        {
            lambda(i, *elem1, j, *elem2);
        }
    }
}

enum class EarlyReturnType
{
    Return,  // Give an early return value
    Break,   // Move to the next outer loop
    Continue // Continue the inner loop
};

template <typename T> class EarlyReturn
{
    private:
    EarlyReturnType type_;
    std::optional<T> value_;

    public:
    EarlyReturn(EarlyReturnType t = EarlyReturnType::Continue, std::optional<T> v = std::nullopt) : type_(t), value_(v){};
    EarlyReturn(const T &val) : type_(EarlyReturnType::Return), value_(val){};
    static EarlyReturn Break() { return EarlyReturn(EarlyReturnType::Break); }
    static EarlyReturn Continue() { return EarlyReturn(EarlyReturnType::Break); }
    EarlyReturnType type() const { return type_; }
    std::optional<T> value() const { return value_; }
};

// Perform an operation on every pair of elements in a container
template <class Iter, class Lam>
auto for_each_pair_early(Iter begin, Iter end, Lam lambda) -> decltype(lambda(0, *begin, 0, *end).value())
{
    int i = 0;
    for (auto elem1 = begin; elem1 != end; ++elem1, ++i)
    {
        int j = i;
        for (auto elem2 = elem1; elem2 != end; ++elem2, ++j)
        {
            auto result = lambda(i, *elem1, j, *elem2);
            switch (result.type())
            {
                case EarlyReturnType::Return:
                    return result.value();
                case EarlyReturnType::Break:
                    break;
                case EarlyReturnType::Continue:
                    continue;
            }
        }
    }
    return std::nullopt;
}
