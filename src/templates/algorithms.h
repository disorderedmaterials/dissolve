// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors
#pragma once

#include "classes/array3DIterator.h"
#include "classes/fullPairIterator.h"
#include "classes/pairIterator.h"
#include "templates/parallelDefs.h"
#include <format>
#include <functional>
#include <optional>
#include <ranges>
#include <sstream>
#include <tuple>
#include <utility>

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
    EarlyReturn(Type t = Type::Continue, std::optional<T> v = std::nullopt) : type_(t), value_(v) {};
    EarlyReturn(const T &val) : type_(Type::Return), value_(val) {};
    Type type() const { return type_; }
    std::optional<T> value() const { return value_; }
};

// A way to check if a lambda *only* takes two int parameters
template <typename T>
concept PairIndexLambda = requires(T lam, int x, int y) { lam(x, y); };

// Perform an operation on every pair of elements in a container,
// or the half-matrix only ([i,j] == [j,i])
// Please note that this can *not* be transformed to use the
// FullPairIterator, since it would prevent using `Break` to move to
// the next loop iteration
template <std::ranges::range Range, class Lam>
auto for_each_pair_early(Range range, Lam lambda, bool half = true) -> std::optional<bool>
{
    int i = 0;
    for (auto elem1 = range.begin(); elem1 != range.end(); ++elem1, ++i)
    {
        int j = half ? i : 0;
        for (auto elem2 = half ? elem1 : range.begin(); elem2 != range.end(); ++elem2, ++j)
        {
            EarlyReturn<bool> result;
            if constexpr (PairIndexLambda<Lam>)
                result = lambda(i, j);
            else
                result = lambda(i, *elem1, j, *elem2);
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

// Overload to avoid using iota everywhere
template <class Lam> auto for_each_pair_early(int count, Lam lambda, bool half = true) -> std::optional<bool>
{
    return for_each_pair_early(std::views::iota(0, count), lambda, half);
}

template <typename... Args> class ZipIterator
{
    public:
    ZipIterator(std::tuple<Args...> args) : source_(std::move(args)) {};
    bool operator!=(ZipIterator<Args...> other)
    {
        return std::apply(
            [&other](auto &a, auto &...as)
            {
                return std::apply(
                    [&a](auto &b, auto &...bs)
                    {
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
        std::apply([](auto &...item) { (item++, ...); }, source_);
    }
    auto operator*()
    {
        return std::apply([](auto &...item) { return std::make_tuple(std::ref(*item)...); }, source_);
    }

    private:
    std::tuple<Args...> source_;
};

template <typename... Args> class zip
{
    public:
    zip(Args &...args) : sources_(args...) {}
    auto begin()
    {
        return ZipIterator(std::apply([](auto &...item) { return std::make_tuple(item.begin()...); }, sources_));
    }
    auto end()
    {
        return ZipIterator(std::apply([](auto &...item) { return std::make_tuple(item.end()...); }, sources_));
    }

    private:
    std::tuple<Args &...> sources_;
};

namespace dissolve
{
// Transform algorithms
template <typename ParallelPolicy, class Iter, typename OutputIt, class UnaryOp,
          std::enable_if_t<dissolve::internal::is_execution_policy<ParallelPolicy>::value, bool> = true>
OutputIt transform(ParallelPolicy policy, Iter begin, Iter end, OutputIt out, UnaryOp unaryOp)
{
    return std::transform(policy, begin, end, out, unaryOp);
}
template <typename ParallelPolicy, class Iter, typename OutputIt, class UnaryOp,
          std::enable_if_t<std::is_same_v<ParallelPolicy, FakeParallelPolicy>, bool> = true>
OutputIt transform(ParallelPolicy policy, Iter begin, Iter end, OutputIt out, UnaryOp unaryOp)
{
    return std::transform(begin, end, out, unaryOp);
}
template <typename ParallelPolicy, class Iter, class Jter, typename OutputIt, class BinaryOp,
          std::enable_if_t<dissolve::internal::is_execution_policy<ParallelPolicy>::value, bool> = true>
OutputIt transform(ParallelPolicy policy, Iter begin, Iter end, Jter other, OutputIt out, BinaryOp binaryOp)
{
    return std::transform(policy, begin, end, other, out, binaryOp);
}
template <typename ParallelPolicy, class Iter, class Jter, typename OutputIt, class BinaryOp,
          std::enable_if_t<std::is_same_v<ParallelPolicy, FakeParallelPolicy>, bool> = true>
OutputIt transform(ParallelPolicy policy, Iter begin, Iter end, Jter other, OutputIt out, BinaryOp binaryOp)
{
    return std::transform(begin, end, other, out, binaryOp);
}
// Transform reduce algorithms
// unaryOp transform container element into type T, which is reduced (summed) by the binaryOp
// Base tranform_reduce, no parallel policy
template <class Iter, typename T, class UnaryOp, class BinaryOp>
T transform_reduce(Iter begin, Iter end, T initialVal, BinaryOp binaryOp, UnaryOp unaryOp)
{
    return std::transform_reduce(begin, end, initialVal, binaryOp, unaryOp);
}
// Only enabled if parallelpolicies are fully defined i.e. we have compiled with multithreading enabled
template <typename ParallelPolicy, class Iter, typename T, class UnaryOp, class BinaryOp,
          std::enable_if_t<dissolve::internal::is_execution_policy<ParallelPolicy>::value, bool> = true>
T transform_reduce(ParallelPolicy policy, Iter begin, Iter end, T initialVal, BinaryOp binaryOp, UnaryOp unaryOp)
{
    return std::transform_reduce(policy, begin, end, initialVal, binaryOp, unaryOp);
}

// Enabled if parallelpolicy is not a real execution policy, i.e. we haven't compiled with multithreading but attempted to
// set a parallel policy
template <typename ParallelPolicy, class Iter, typename T, class UnaryOp, class BinaryOp,
          std::enable_if_t<std::is_same_v<ParallelPolicy, FakeParallelPolicy>, bool> = true>
T transform_reduce(ParallelPolicy, Iter begin, Iter end, T initialVal, BinaryOp binaryOp, UnaryOp unaryOp)
{
    return dissolve::transform_reduce(begin, end, initialVal, binaryOp, unaryOp);
}

// For each algorithm
// unaryOp applies to each element in the range
// Base for_each, no parallel policy
template <class Iter, class UnaryOp> void for_each(Iter begin, Iter end, UnaryOp unaryOp)
{
    std::for_each(begin, end, unaryOp);
}
// Only enabled if parallelpolicies are fully defined i.e. we have compiled with multithreading enabled
template <typename ParallelPolicy, class Iter, class UnaryOp,
          std::enable_if_t<dissolve::internal::is_execution_policy<ParallelPolicy>::value, bool> = true>
void for_each(ParallelPolicy policy, Iter begin, Iter end, UnaryOp unaryOp)
{
    std::for_each(policy, begin, end, unaryOp);
}

// Enabled if parallelpolicy is not a real execution policy, i.e. we haven't compiled with multithreading but attempted to
// set a parallel policy
template <typename ParallelPolicy, class Iter, class UnaryOp,
          std::enable_if_t<std::is_same_v<ParallelPolicy, FakeParallelPolicy>, bool> = true>
void for_each(ParallelPolicy, Iter begin, Iter end, UnaryOp unaryOp)
{
    dissolve::for_each(begin, end, unaryOp);
}

// Base for_each, no parallel policy
template <class Range, class UnaryOp> void for_each(Range range, UnaryOp unaryOp) { std::ranges::for_each(range, unaryOp); }
// Only enabled if parallelpolicies are fully defined i.e. we have compiled with multithreading enabled
template <typename ParallelPolicy, class Range, class UnaryOp,
          std::enable_if_t<dissolve::internal::is_execution_policy<ParallelPolicy>::value, bool> = true>
void for_each(ParallelPolicy policy, Range range, UnaryOp unaryOp)
{
    std::for_each(policy, range, unaryOp);
}

// Enabled if parallelpolicy is not a real execution policy, i.e. we haven't compiled with multithreading but attempted to
// set a parallel policy
template <typename ParallelPolicy, class Range, class UnaryOp,
          std::enable_if_t<std::is_same_v<ParallelPolicy, FakeParallelPolicy>, bool> = true>
void for_each(ParallelPolicy, Range range, UnaryOp unaryOp)
{
    dissolve::for_each(range, unaryOp);
}

// Perform an operation on every pair of elements in a contained, or the half-matrix only ([i,j] == [j,i])
template <typename ParallelPolicy, std::ranges::range Range, class Lam>
void for_each_pair(ParallelPolicy policy, Range range, Lam lambda, bool half = true)
{
    auto actions = [&lambda, &range](const auto pair)
    {
        auto &[i, j] = pair;
        if constexpr (PairIndexLambda<Lam>)
            lambda(i, j);
        else
            lambda(i, range.begin()[i], j, range.begin()[j]);
    };
    if (half)
    {
        PairIterator start(range.end() - range.begin()),
            stop(range.end() - range.begin(), ((range.end() - range.begin()) * (range.end() - range.begin() + 1)) / 2);
        for_each(policy, start, stop, actions);
    }
    else
    {
        FullPairIterator start(range.end() - range.begin()),
            stop(range.end() - range.begin(), (range.end() - range.begin()) * (range.end() - range.begin()));
        for_each(policy, start, stop, actions);
    }
}

// Overload to avoid using iota everywhere
template <typename ParallelPolicy, class Lam> void for_each_pair(ParallelPolicy policy, int count, Lam lambda, bool half = true)
{
    for_each_pair(policy, std::views::iota(0, count), lambda, half);
}

template <typename ParallelPolicy, class Iter, class Lam>
void for_each_triplet(ParallelPolicy policy, Iter begin, Iter end, Lam lambda)
{
    for_each(policy, begin, end,
             [&lambda](const auto triplet)
             {
                 auto [x, y, z] = triplet;
                 lambda(triplet, x, y, z);
             });
}
} // namespace dissolve

// Join elements into a delimited string
template <typename Iterator> std::string joinStrings(Iterator begin, Iterator end, std::string delim = ", ")
{
    if (begin == end)
        return std::string();
    std::stringstream stream;
    stream << std::format("{}", *begin);
    std::for_each(std::next(begin), end, [&stream, &delim](const auto value) { stream << delim << std::format("{}", value); });
    return stream.str();
}
template <typename Iterator, class Lam> std::string joinStrings(Iterator begin, Iterator end, std::string delim, Lam lambda)
{
    if (begin == end)
        return std::string();
    std::stringstream stream;
    stream << std::format("{}", lambda(*begin));
    std::for_each(std::next(begin), end,
                  [&stream, &lambda, &delim](const auto &value) { stream << delim << std::format("{}", lambda(value)); });
    return stream.str();
}
template <class Class> std::string joinStrings(Class range, std::string delim = ", ")
{
    return joinStrings(range.begin(), range.end(), delim);
}
template <class Class, class Lam> std::string joinStrings(Class range, std::string delim, Lam lambda)
{
    return joinStrings(range.begin(), range.end(), delim, lambda);
}

// Template functions to determine if a given class derives from a specific base
template <class T, template <class...> class U> inline constexpr bool is_instance_of_v = std::false_type{};
template <template <class...> class U, class... Vs> inline constexpr bool is_instance_of_v<U<Vs...>, U> = std::true_type{};
