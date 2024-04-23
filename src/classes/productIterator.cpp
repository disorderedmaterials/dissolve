// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/productIterator.h"
#include <cmath>

ProductIterator::ProductIterator(int xSize, int ySize, int index) : xSize_(xSize), ySize_(ySize)
{
    if (index == 0)
        x_ = y_ = 0;
    else
        fromIndex(index);
}

void ProductIterator::fromIndex(int index)
{
  x_ = index / ySize_;
  y_ = index % ySize_;
}

int ProductIterator::toIndex() const
{
  return x_ * ySize_ + y_;
}

bool ProductIterator::operator<(const ProductIterator &other) const
{
    if (x_ != other.x_)
        return x_ < other.x_;
    return y_ < other.y_;
}

bool ProductIterator::operator==(const ProductIterator &other) const { return x_ == other.x_ && y_ == other.y_; }

bool ProductIterator::operator!=(const ProductIterator &other) const { return x_ != other.x_ || y_ != other.y_; }

ProductIterator::value_type ProductIterator::operator*() { return {x_, y_}; }

ProductIterator &ProductIterator::operator++()
{
    y_++;
    if (y_ >= ySize_)
    {
        x_ += 1;
        y_ = 0;
    }
    return *this;
}

ProductIterator::difference_type ProductIterator::operator-(const ProductIterator &other) const { return toIndex() - other.toIndex(); }

ProductIterator ProductIterator::operator+(ProductIterator::difference_type diff) const { return ProductIterator(xSize_, ySize_, diff + toIndex()); }

ProductIterator &ProductIterator::operator+=(difference_type forward)
{
    fromIndex(forward + toIndex());
    return *this;
}

ProductIterator::value_type ProductIterator::operator[](difference_type i) const { return *(*this + i); }

ProductIterator ProductIterator::begin() const { return ProductIterator(xSize_, ySize_, 0); }
ProductIterator ProductIterator::end() const { return ProductIterator(xSize_, ySize_, xSize_ * ySize_); }
