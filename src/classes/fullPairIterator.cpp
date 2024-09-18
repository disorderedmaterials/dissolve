// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/fullPairIterator.h"
#include <cmath>

FullPairIterator::FullPairIterator(int size, int index) : size_(size)
{
    if (index == 0)
        x_ = y_ = 0;
    else
        fromIndex(index);
}

void FullPairIterator::fromIndex(int index)
{
    x_ = index / size_;
    y_ = index % size_;
}

int FullPairIterator::toIndex() const { return x_ * size_ + y_; }

bool FullPairIterator::operator<(const FullPairIterator &other) const
{
    if (x_ != other.x_)
        return x_ < other.x_;
    return y_ < other.y_;
}

bool FullPairIterator::operator==(const FullPairIterator &other) const { return x_ == other.x_ && y_ == other.y_; }

bool FullPairIterator::operator!=(const FullPairIterator &other) const { return x_ != other.x_ || y_ != other.y_; }

FullPairIterator::value_type FullPairIterator::operator*() { return {x_, y_}; }

FullPairIterator &FullPairIterator::operator++()
{
    if (++y_ >= size_)
    {
        x_ += 1;
        y_ = 0;
    }
    return *this;
}

FullPairIterator::difference_type FullPairIterator::operator-(const FullPairIterator &other) const
{
    return toIndex() - other.toIndex();
}

FullPairIterator FullPairIterator::operator+(FullPairIterator::difference_type diff) const
{
    return FullPairIterator(size_, diff + toIndex());
}

FullPairIterator &FullPairIterator::operator+=(difference_type forward)
{
    fromIndex(forward + toIndex());
    return *this;
}

FullPairIterator::value_type FullPairIterator::operator[](difference_type i) const { return *(*this + i); }

FullPairIterator FullPairIterator::begin() const { return FullPairIterator(size_, 0); }
FullPairIterator FullPairIterator::end() const { return FullPairIterator(size_, size_ * size_); }
