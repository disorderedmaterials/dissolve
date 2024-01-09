// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors
#pragma once

#include <iterator>
#include <tuple>

class Array3DIterator
{
    private:
    int sizeX_;
    int sizeY_;
    int sizeZ_;
    int x_;
    int y_;
    int z_;
    int toIndex() const;
    void fromIndex(int);

    public:
    using value = std::tuple<int, int, int>;
    using value_type = std::tuple<int, int, int>;
    using difference_type = int;
    using reference = std::tuple<int, int, int> &;
    using pointer = std::tuple<int, int, int> *;
    using iterator_category = std::random_access_iterator_tag;

    Array3DIterator(int sizeX = 0, int sizeY = 0, int sizeZ = 0, int index = 0);

    Array3DIterator begin() const;
    Array3DIterator end() const;
    value_type operator*();
    value_type operator[](difference_type i) const;

    difference_type operator-(const Array3DIterator &it) const;
    Array3DIterator &operator+=(difference_type forward);

    Array3DIterator &operator-=(difference_type backward);

    // Operators : arithmetic
    Array3DIterator &operator++();

    Array3DIterator &operator--();

    Array3DIterator operator++(int);

    Array3DIterator operator--(int);

    Array3DIterator operator+(difference_type forward) const;
    Array3DIterator operator-(difference_type backward) const;

    // Operators : comparison
    bool operator==(const Array3DIterator &other) const;
    bool operator!=(const Array3DIterator &other) const;
    bool operator<(const Array3DIterator &other) const;
    bool operator>(const Array3DIterator &other) const;
    bool operator<=(const Array3DIterator &other) const;
    bool operator>=(const Array3DIterator &other) const;
};
