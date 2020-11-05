// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

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

// Perform an operation on every pair of elements in a range (begin <= i < end)
template <class Lam> void for_each_pair(int begin, int end, Lam lambda)
{
    for (auto i = begin; i < end; ++i)
        for (auto j = i; j < end; ++j)
            lambda(i, j);
}

template <typename T> class EarlyReturn
{
    public:
    enum Type
    {
        Return,  // Return immediately with the given value
        Break,   // Move to the next step in the outer loop
        Continue // Move to the next step in the inner loop
    };

    private:
    Type type_;
    std::optional<T> value_;

    public:
    using inner = T;
    EarlyReturn(Type t = Type::Continue, std::optional<T> v = std::nullopt) : type_(t), value_(v){};
    EarlyReturn(const T &val) : type_(Type::Return), value_(val){};
    Type type() const { return type_; }
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
                case EarlyReturn<typename decltype(result)::inner>::Return:
                    return result.value();
                case EarlyReturn<typename decltype(result)::inner>::Break:
                    break;
                case EarlyReturn<typename decltype(result)::inner>::Continue:
                    continue;
            }
        }
    }
    return std::nullopt;
}

// Perform an operation on every pair of elements in a range
template <class Lam> auto for_each_pair_early(int begin, int end, Lam lambda) -> decltype(lambda(0, 0).value())
{
    for (auto i = begin; i < end; ++i)
        for (auto j = i; j < end; ++j)
        {
            auto result = lambda(i, j);
            switch (result.type())
            {
                case EarlyReturn<typename decltype(result)::inner>::Return:
                    return result.value();
                case EarlyReturn<typename decltype(result)::inner>::Break:
                    break;
                case EarlyReturn<typename decltype(result)::inner>::Continue:
                    continue;
            }
        }

    return std::nullopt;
}
