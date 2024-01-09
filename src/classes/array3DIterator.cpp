// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/array3DIterator.h"
#include <cmath>
#include <iostream>

Array3DIterator::Array3DIterator(int sizeX, int sizeY, int sizeZ, int index) : sizeX_(sizeX), sizeY_(sizeY), sizeZ_(sizeZ)
{
    if (index == 0)
        x_ = y_ = z_ = 0;
    else
        fromIndex(index);
}
void Array3DIterator::fromIndex(int index)
{
    x_ = index % sizeX_;
    y_ = (index / sizeX_) % sizeY_;
    z_ = index / (sizeX_ * sizeY_);
}

int Array3DIterator::toIndex() const { return x_ + sizeX_ * y_ + sizeX_ * sizeY_ * z_; }

bool Array3DIterator::operator<(const Array3DIterator &other) const
{
    if (x_ != other.x_)
        return x_ < other.x_;
    if (y_ != other.y_)
        return y_ < other.y_;
    return z_ < other.z_;
}
bool Array3DIterator::operator==(const Array3DIterator &other) const
{
    return x_ == other.x_ && y_ == other.y_ && z_ == other.z_;
}

bool Array3DIterator::operator!=(const Array3DIterator &other) const
{
    return x_ != other.x_ || y_ != other.y_ || z_ != other.z_;
}
Array3DIterator::value_type Array3DIterator::operator*() { return {x_, y_, z_}; }

Array3DIterator &Array3DIterator::operator++()
{
    ++x_;
    if (x_ >= sizeX_)
    {
        y_ += 1;
        x_ = 0;

        if (y_ >= sizeY_)
        {
            z_ += 1;
            y_ = 0;
        }
    }
    return *this;
}

Array3DIterator::difference_type Array3DIterator::operator-(const Array3DIterator &other) const
{
    return toIndex() - other.toIndex();
}

Array3DIterator Array3DIterator::operator+(Array3DIterator::difference_type diff) const
{
    return Array3DIterator(sizeX_, sizeY_, sizeZ_, diff + toIndex());
}

Array3DIterator &Array3DIterator::operator+=(difference_type forward)
{
    fromIndex(forward + toIndex());
    return *this;
}

Array3DIterator::value_type Array3DIterator::operator[](difference_type i) const { return *(*this + i); }

Array3DIterator Array3DIterator::begin() const { return Array3DIterator(sizeX_, sizeY_, sizeZ_, 0); }
Array3DIterator Array3DIterator::end() const { return Array3DIterator(sizeX_, sizeY_, sizeZ_, (sizeX_ * sizeY_ * sizeZ_)); }
