// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/triplet_iterator.h"
#include <cmath>
#include <iostream>

TripletIterator::TripletIterator(int sizeX, int sizeY, int sizeZ, int index) : sizeX_(sizeX), sizeY_(sizeY), sizeZ_(sizeZ)
{
    if (index == 0)
        x_ = y_ = z_ = 0;
    else
        fromIndex(index);
}

void TripletIterator::fromIndex(int index)
{
    x_ = index % sizeX_;
    index/= sizeX_;
    y_ = index % sizeY_;
    index/=sizeY_;
    z_ = index;
}

int TripletIterator::toIndex() const
{
    return x_ + sizeX_ * y_ + sizeX_ * sizeY_ * z_;
}

bool TripletIterator::operator<(const TripletIterator &other) const
{
    if (x_ != other.x_)
        return x_ < other.x_;
    if (y_ != other.y_)
        return y_ < other.y_;
    return z_ < other.z_;
}

bool TripletIterator::operator==(const TripletIterator &other) const {return x_ == other.x_ && y_ == other.y_ && z_ == other.z_;}
bool TripletIterator::operator!=(const TripletIterator &other) const {return x_ != other.x_ || y_ != other.y_ || z_ != other.z_;}
TripletIterator::value_type TripletIterator::operator*() { return {x_, y_, z_};}

TripletIterator &TripletIterator::operator++()
{
    z_ ++;
    if (z_ >= sizeZ_)
    {
        y_+=1;
        
        if (y_ >= sizeY_)
        {
            x_+=1;
            y_ = x_;
        }
        z_ = y_;
    }
    return *this;
}

TripletIterator::difference_type TripletIterator::operator-(const TripletIterator &other) const { return toIndex() - other.toIndex(); }

TripletIterator TripletIterator::operator+(TripletIterator::difference_type diff) const { return TripletIterator(sizeX_, sizeY_, sizeZ_, diff + toIndex()); } 

TripletIterator &TripletIterator::operator+=(difference_type forward)
{
    fromIndex(forward + toIndex());
    return *this;
}

TripletIterator::value_type TripletIterator::operator[](difference_type i) const {return *(*this + i); }

TripletIterator TripletIterator::begin() const {return TripletIterator(sizeX_, sizeY_, sizeZ_, 0);}
TripletIterator TripletIterator::end() const {return TripletIterator(sizeX_, sizeY_, sizeZ_, (sizeX_ * sizeY_ * sizeZ_) -1);}
