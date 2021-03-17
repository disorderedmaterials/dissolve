// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <functional>
#include <optional>
#include <tuple>

// Cut a range into a smaller segment for MPI
template <typename T> auto chop_range(const T begin, const T end, const int nChunks, const int index)
{
    auto diff = end - begin;
    T start = begin + std::ldiv((const long int)index * diff, (const long int)nChunks).quot;
    T stop = begin + std::ldiv((const long int)(index + 1) * diff, (const long int)nChunks).quot;
    return std::make_tuple(start, stop);
}

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

// Perform an operation on every pair of elements in a container
template <class Iter, class Lam> void for_each_pair(Iter begin, Iter end, int nChunks, int index, Lam lambda)
{
    auto [start, stop] = chop_range(begin, end, nChunks, index);
    int i = start - begin;
    for (auto elem1 = start; elem1 != stop; ++elem1, ++i)
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

template <typename... Args> class ZipIterator
{
    public:
    ZipIterator(std::tuple<Args...> args) : source_(args){};
    bool operator!=(ZipIterator<Args...> other)
    {
        return std::apply(
            [&other](auto &a, auto &... as) {
                return std::apply(
                    [&a](auto &b, auto &... bs) {
                        // Only test the first elements.  We have to make the
                        // assumption that all the containers are the same length,
                        // anyway and this saves us some tests and some code.
                        return a != b;
                    },
                    other.source_);
            },
            source_);
    }
    void operator++()
    {
        std::apply([](auto &... item) { (item++, ...); }, source_);
    }
    auto operator*()
    {
        return std::apply([](auto &... item) { return std::make_tuple(std::ref(*item)...); }, source_);
    }

    private:
    std::tuple<Args...> source_;
};

template <typename... Args> class zip
{
    public:
    zip(Args &... args) : sources_(args...) {}
    auto begin()
    {
        return ZipIterator(std::apply([](auto &... item) { return std::make_tuple(item.begin()...); }, sources_));
    }
    auto end()
    {
        return ZipIterator(std::apply([](auto &... item) { return std::make_tuple(item.end()...); }, sources_));
    }

    private:
    std::tuple<Args &...> sources_;
};
