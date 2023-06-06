// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors
#pragma once

#include <iterator>
#include <tuple>

class TripletIterator
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

    TripletIterator(int sizeX = 0, int sizeY=0, int sizeZ=0, int index = 0);

    TripletIterator begin() const;
    TripletIterator end() const;
    value_type operator*();
    value_type operator[](difference_type i) const;

    difference_type operator-(const TripletIterator &it) const;
    TripletIterator &operator+=(difference_type forward);

    TripletIterator &operator-=(difference_type backward);

    // Operators : arithmetic
    TripletIterator &operator++();

    TripletIterator &operator--();

    TripletIterator operator++(int);

    TripletIterator operator--(int);

    TripletIterator operator+(difference_type forward) const;
    TripletIterator operator-(difference_type backward) const;

    // Operators : comparison
    bool operator==(const TripletIterator &other) const;
    bool operator!=(const TripletIterator &other) const;
    bool operator<(const TripletIterator &other) const;
    bool operator>(const TripletIterator &other) const;
    bool operator<=(const TripletIterator &other) const;
    bool operator>=(const TripletIterator &other) const;
};

