// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors
#pragma once

#include <iterator>
#include <tuple>

class ProductIterator
{
    private:
    int x_, y_, xSize_, ySize_;
    int toIndex() const;
    void fromIndex(int);

    public:
    using value = std::tuple<int, int>;
    using value_type = std::tuple<int, int>;
    using difference_type = int;
    using reference = std::tuple<int, int> &;
    using pointer = std::tuple<int, int> *;
    using iterator_category = std::random_access_iterator_tag;

    ProductIterator(int xSize = 0, int ySize = 0, int index = 0);

    ProductIterator begin() const;
    ProductIterator end() const;
    value_type operator*();
    value_type operator[](difference_type i) const;

    difference_type operator-(const ProductIterator &it) const;
    ProductIterator &operator+=(difference_type forward);

    ProductIterator &operator-=(difference_type backward);

    // Operators : arithmetic
    ProductIterator &operator++();

    ProductIterator &operator--();

    ProductIterator operator++(int);

    ProductIterator operator--(int);

    ProductIterator operator+(difference_type forward) const;
    ProductIterator operator-(difference_type backward) const;

    // Operators : comparison
    bool operator==(const ProductIterator &other) const;
    bool operator!=(const ProductIterator &other) const;
    bool operator<(const ProductIterator &other) const;
    bool operator>(const ProductIterator &other) const;
    bool operator<=(const ProductIterator &other) const;
    bool operator>=(const ProductIterator &other) const;
};
