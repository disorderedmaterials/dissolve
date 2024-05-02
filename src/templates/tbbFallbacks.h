// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors
#pragma once
#include <cassert>
#include <iterator>
#include <type_traits>

namespace dissolve
{
template <typename IntType> class counting_iterator
{
    public:
    using value_type = IntType;
    using difference_type = std::make_signed_t<IntType>;
    using reference = IntType &;
    using pointer = IntType *;
    using iterator_category = std::random_access_iterator_tag;

    // Operators : Constructors
    counting_iterator() = default;
    explicit counting_iterator(IntType value) : counter_(value) {}

    // Operators : misc
    reference operator*() { return counter_; }
    value_type operator[](difference_type i) const { return *(*this + i); };
    difference_type operator-(const counting_iterator &it) const { return counter_ - it.counter_; };
    counting_iterator &operator+=(difference_type forward)
    {
        counter_ += forward;
        return *this;
    }
    counting_iterator &operator-=(difference_type backward) { return *this += -backward; };

    // Operators : arithmetic
    counting_iterator &operator++()
    {
        counter_ += 1;
        return *this;
    }
    counting_iterator &operator--()
    {
        counter_ -= 1;
        return *this;
    }
    counting_iterator operator++(int)
    {
        counting_iterator temp = (*this);
        ++(*this);
        return temp;
    }
    counting_iterator operator--(int)
    {
        counting_iterator temp = (*this);
        --(*this);
        return temp;
    }
    counting_iterator operator+(difference_type forward) const { return counting_iterator(counter_ + forward); };
    counting_iterator operator-(difference_type backward) const { return counting_iterator(counter_ - backward); };

    // Operators : comparison
    bool operator==(const counting_iterator &other) const { return this->counter_ == other.counter_; };
    bool operator!=(const counting_iterator &other) const { return !(*this == other); };
    bool operator<(const counting_iterator &other) const { return *this - other < 0; };
    bool operator>(const counting_iterator &other) const { return other < *this; };
    bool operator<=(const counting_iterator &other) const { return !(*this > other); };
    bool operator>=(const counting_iterator &other) const { return !(*this < other); };

    private:
    IntType counter_;
};

template <typename T> class combinable
{
    public:
    template <typename Lambda> combinable(Lambda init) { data_ = init(); }
    T &local() { return data_; }
    template <typename Lambda> T combine(Lambda) { return data_; }
    template <typename Lambda> void combine_each(Lambda unaryOp) { return unaryOp(data_); }

    private:
    T data_;
};

template <typename IntType> class blocked_range
{
    public:
    blocked_range(IntType begin, IntType end) noexcept : begin_(begin), end_(end) {}

    int begin() const noexcept { return begin_; }
    int end() const noexcept { return end_; }

    private:
    IntType begin_;
    IntType end_;
};

template <typename RowType, typename ColType> class blocked_range2d
{
    public:
    blocked_range2d(RowType rowBegin, RowType rowEnd, ColType colBegin, ColType colEnd)
        : rows_(rowBegin, rowEnd), cols_(colBegin, colEnd)
    {
    }

    dissolve::blocked_range<RowType> rows() const noexcept { return rows_; }
    dissolve::blocked_range<ColType> cols() const noexcept { return cols_; }

    private:
    dissolve::blocked_range<RowType> rows_;
    dissolve::blocked_range<ColType> cols_;
};

template <typename Range, typename T, typename Op, typename ReductionOp>
T tbb_parallel_reduce(const Range &range, T init, Op &&op, ReductionOp &&)
{
    return op(range, init);
}

} // namespace dissolve
