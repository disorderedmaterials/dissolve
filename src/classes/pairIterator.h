// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors
#pragma once

#include <iterator>
#include <tuple>

// Iterate over every unordered pair of values (i.e. (2, 3) and (3, 2)
// are considered the same value and only one is visited)
class PairIterator
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

    PairIterator(int size = 0, int index = 0);

    PairIterator begin() const;
    PairIterator end() const;
    value_type operator*();
    value_type operator[](difference_type i) const;

    difference_type operator-(const PairIterator &it) const;
    PairIterator &operator+=(difference_type forward);

    PairIterator &operator-=(difference_type backward);

    // Operators : arithmetic
    PairIterator &operator++();

    PairIterator &operator--();

    PairIterator operator++(int);

    PairIterator operator--(int);

    PairIterator operator+(difference_type forward) const;
    PairIterator operator-(difference_type backward) const;

    // Operators : comparison
    bool operator==(const PairIterator &other) const;
    bool operator!=(const PairIterator &other) const;
    bool operator<(const PairIterator &other) const;
    bool operator>(const PairIterator &other) const;
    bool operator<=(const PairIterator &other) const;
    bool operator>=(const PairIterator &other) const;
};
