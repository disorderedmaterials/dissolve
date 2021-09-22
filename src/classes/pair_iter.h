// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#pragma once

#include <iterator>
#include <tuple>

class PairIterator
{
    private:
    int size_;
    int x_;
    int y_;
    int toIndex_();
    void fromIndex_(int);

    public:
    using value_type = std::tuple<int, int>;
    using difference_type = int;
    using reference = std::tuple<int, int> &;
    using pointer = std::tuple<int, int> *;
    using iterator_category = std::random_access_iterator_tag;

    PairIterator(int size, int index = 0);
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
