// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/array3d_iter.h"
#include <cmath>
#include <iostream>

Array3DIterator::Array3DIterator(int sizeX, int sizeY, int sizeZ, int index) : sizeX_(sizeX), sizeY_(sizeY), sizeZ_(sizeZ), index_(index)
{
    if (index == 0)
        x_ = y_ = z_ = 0;
    else
        fromIndex();
}
void Array3DIterator::fromIndex()
{
    x_ = index_ % sizeX_;
    y_ = (index_ / sizeX_) % sizeY_;
    z_ = index_ / (sizeX_ * sizeY_);
}

bool Array3DIterator::operator<(const Array3DIterator &other) const
{
    return index_ < other.index_;
}

bool Array3DIterator::operator==(const Array3DIterator &other) const
{
    return index_ == other.index_;
}

bool Array3DIterator::operator!=(const Array3DIterator &other) const
{
    return index_ != other.index_;
}

Array3DIterator::value_type Array3DIterator::operator*() { return {x_, y_, z_}; }

Array3DIterator &Array3DIterator::operator++()
{
    ++index_;
    fromIndex();
    return *this;
}

Array3DIterator::difference_type Array3DIterator::operator-(const Array3DIterator &other) const
{
    return index_ - other.index_;
}

Array3DIterator Array3DIterator::operator+(Array3DIterator::difference_type diff) const
{
    return Array3DIterator(sizeX_, sizeY_, sizeZ_, diff + index_);
}

Array3DIterator &Array3DIterator::operator+=(difference_type forward)
{
    index_ += forward;
    fromIndex();
    return *this;
}

Array3DIterator::value_type Array3DIterator::operator[](difference_type i) const { return *(*this + i); }

Array3DIterator Array3DIterator::begin() const { return Array3DIterator(sizeX_, sizeY_, sizeZ_, 0); }
Array3DIterator Array3DIterator::end() const { return Array3DIterator(sizeX_, sizeY_, sizeZ_, (sizeX_ * sizeY_ * sizeZ_)); }
