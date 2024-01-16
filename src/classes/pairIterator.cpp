// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/pairIterator.h"
#include <cmath>

PairIterator::PairIterator(int size, int index) : size_(size)
{
    if (index == 0)
        x_ = y_ = 0;
    else
        fromIndex(index);
}

void PairIterator::fromIndex(int index)
{
    double Np = size_ + 0.5;
    x_ = floor(Np - sqrt(Np * Np - 2 * index));
    y_ = x_ + index - (x_ * Np - 0.5 * x_ * x_);
}

int PairIterator::toIndex() const
{
    int front = ((2 * size_ + 1) - x_) * x_ / 2;
    return front + y_ - x_;
}

bool PairIterator::operator<(const PairIterator &other) const
{
    if (x_ != other.x_)
        return x_ < other.x_;
    return y_ < other.y_;
}

bool PairIterator::operator==(const PairIterator &other) const { return x_ == other.x_ && y_ == other.y_; }

bool PairIterator::operator!=(const PairIterator &other) const { return x_ != other.x_ || y_ != other.y_; }

PairIterator::value_type PairIterator::operator*() { return {x_, y_}; }

PairIterator &PairIterator::operator++()
{
    y_++;
    if (y_ >= size_)
    {
        x_ += 1;
        y_ = x_;
    }
    return *this;
}

PairIterator::difference_type PairIterator::operator-(const PairIterator &other) const { return toIndex() - other.toIndex(); }

PairIterator PairIterator::operator+(PairIterator::difference_type diff) const { return PairIterator(size_, diff + toIndex()); }

PairIterator &PairIterator::operator+=(difference_type forward)
{
    fromIndex(forward + toIndex());
    return *this;
}

PairIterator::value_type PairIterator::operator[](difference_type i) const { return *(*this + i); }

PairIterator PairIterator::begin() const { return PairIterator(size_, 0); }
PairIterator PairIterator::end() const { return PairIterator(size_, size_ * (size_ + 1) / 2); }
