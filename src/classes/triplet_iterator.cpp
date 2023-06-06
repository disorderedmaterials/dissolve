// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/triplet_iterator.h"
#include <cmath>
#include <iostream>

TripletIterator::TripletIterator(int size, int index) : size_(size)
{
    if (index == 0)
        x_ = y_ = z_ = 0;
    else
        fromIndex(index);
}

void TripletIterator::fromIndex(int index)
{
    double Np = size_ + 0.5;
    x_ = floor(Np - sqrt(Np * Np - 2 * index));
    y_ = x_ + index - (x_ * Np - 0.5 * x_ * x_);
    z_ = y_ + index - (y_ * Np - 0.5 * y_ * y_);    
}

int TripletIterator::toIndex() const
{
    int front = ((2 * size_ + 1) - x_) * x_ / 2;
    int side = ((2 * size_ + 1) - y_) * y_ / 2;
    return front + side + z_-y_-x_;
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
    if (z_ >= size_)
    {
        y_+=1;
        
        if (y_ >= size_)
        {
            x_+=1;
            y_ = x_;
        }
        z_ = y_;
    }
    return *this;
}

TripletIterator::difference_type TripletIterator::operator-(const TripletIterator &other) const { return toIndex() - other.toIndex(); }

TripletIterator TripletIterator::operator+(TripletIterator::difference_type diff) const { return TripletIterator(size_, diff + toIndex()); } 

TripletIterator &TripletIterator::operator+=(difference_type forward)
{
    fromIndex(forward + toIndex());
    return *this;
}

TripletIterator::value_type TripletIterator::operator[](difference_type i) const {return *(*this + i); }

TripletIterator TripletIterator::begin() const {return TripletIterator(size_, 0);}
TripletIterator TripletIterator::end() const {return TripletIterator(size_, size_ * (size_+1) / 3);}
