// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors
#pragma once

#include <iterator>
#include <tuple>

// Iterate over every ordered pair of values (i.e. (2, 3) and (3, 2)
// are both visited)
class FullPairIterator
{
    private:
    int size_;
    int x_;
    int y_;
    int toIndex() const;
    void fromIndex(int);

    public:
    using value = std::tuple<int, int>;
    using value_type = std::tuple<int, int>;
    using difference_type = int;
    using reference = std::tuple<int, int> &;
    using pointer = std::tuple<int, int> *;
    using iterator_category = std::random_access_iterator_tag;

    FullPairIterator(int size = 0, int index = 0);

    FullPairIterator begin() const;
    FullPairIterator end() const;
    value_type operator*();
    value_type operator[](difference_type i) const;

    difference_type operator-(const FullPairIterator &it) const;
    FullPairIterator &operator+=(difference_type forward);

    FullPairIterator &operator-=(difference_type backward);

    // Operators : arithmetic
    FullPairIterator &operator++();

    FullPairIterator &operator--();

    FullPairIterator operator++(int);

    FullPairIterator operator--(int);

    FullPairIterator operator+(difference_type forward) const;
    FullPairIterator operator-(difference_type backward) const;

    // Operators : comparison
    bool operator==(const FullPairIterator &other) const;
    bool operator!=(const FullPairIterator &other) const;
    bool operator<(const FullPairIterator &other) const;
    bool operator>(const FullPairIterator &other) const;
    bool operator<=(const FullPairIterator &other) const;
    bool operator>=(const FullPairIterator &other) const;
};
