// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors
#pragma once

#include <iterator>
#include <tuple>

template <typename A, typename B> class ProductIterator
{
    private:
    A x_, xBegin_, xEnd_;
    B y_, yBegin_, yEnd_;
    std::decay_t<decltype(yEnd_ - yBegin_)> ySize_;

    public:
    using valueA = std::decay_t<decltype(*x_)>;
    using valueB = std::decay_t<decltype(*y_)>;
    using value = std::tuple<valueA, valueB>;
    using value_type = std::tuple<valueA, valueB>;
    using difference_type = int;
    using reference = std::tuple<const valueA &, const valueB &>;
    using pointer = std::tuple<valueA, valueB> *;
    using iterator_category = std::random_access_iterator_tag;

    public:
    int toIndex() const { return (x_ - xBegin_) * ySize_ + (y_ - yBegin_); }
    void fromIndex(int index)
    {
        x_ = xBegin_ + (index / ySize_);
        y_ = yBegin_ + (index % ySize_);
    }

    public:
    ProductIterator(A xBegin, A xEnd, B yBegin, B yEnd, int index = 0)
        : xBegin_(xBegin), xEnd_(xEnd), yBegin_(yBegin), yEnd_(yEnd), ySize_(yEnd_ - yBegin_), x_(xBegin), y_(yBegin)
    {
        if (index != 0)
        {
            fromIndex(index);
        }
    }

    ProductIterator begin() const { return ProductIterator(xBegin_, xEnd_, yBegin_, yEnd_); }
    ProductIterator end() const
    {
        ProductIterator it(xBegin_, xEnd_, yBegin_, yEnd_);
        it.x_ = xEnd_;
        it.y_ = yBegin_;
        return it;
    }
    reference operator*() { return std::tie(*x_, *y_); }
    reference operator[](difference_type i) const { return *(*this + i); }

    difference_type operator-(const ProductIterator &it) const { return toIndex() - it.toIndex(); }
    ProductIterator &operator+=(difference_type forward)
    {
        fromIndex(forward + toIndex());
        return *this;
    }

    ProductIterator &operator-=(difference_type backward)
    {
        fromIndex(toIndex() - backward);
        return *this;
    }

    // Operators : arithmetic
    ProductIterator &operator++()
    {
        ++y_;
        if (y_ >= yEnd_)
        {
            ++x_;
            y_ = yBegin_;
        }
        return *this;
    }

    ProductIterator &operator--();

    ProductIterator operator++(int);

    ProductIterator operator--(int);

    ProductIterator operator+(difference_type forward) const
    {
        return ProductIterator(xBegin_, xEnd_, yBegin_, yEnd_, forward + toIndex());
    }
    ProductIterator operator-(difference_type backward) const;

    // Operators : comparison
    bool operator==(const ProductIterator &other) const { return x_ == other.x_ && y_ == other.y_; }
    bool operator!=(const ProductIterator &other) const { return x_ != other.x_ || y_ != other.y_; }
    bool operator<(const ProductIterator &other) const
    {
        if (x_ != other.x_)
            return x_ < other.x_;
        return y_ < other.y_;
    }
    bool operator>(const ProductIterator &other) const;
    bool operator<=(const ProductIterator &other) const;
    bool operator>=(const ProductIterator &other) const;
};
