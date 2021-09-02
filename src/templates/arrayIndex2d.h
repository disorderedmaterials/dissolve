// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/messenger.h"
#include "templates/array2d.h"
#include "templates/list.h"
#include "templates/vector3.h"
#include <numeric>
#include <tuple>
#include <vector>

template <class A> class ArrayIndex2D
{
    public:
    ArrayIndex2D(A &inArray) : arr_(inArray)
    {
        nX_ = inArray.nRows();
        nY_ = inArray.nColumns();
        ptr_ = &inArray[{0, 0}];
    }

    ArrayIndex2D &operator++()
    {
        ++ptr_;
        return *this;
    }
    ArrayIndex2D operator++(int)
    {
        ArrayIndex2D temp = *this;
        ++*this;
        return temp;
    }
    ArrayIndex2D &operator--()
    {
        --ptr_;
        return *this;
    }
    ArrayIndex2D operator--(int)
    {
        ArrayIndex2D temp = *this;
        --*this;
        return temp;
    }
    ArrayIndex2D &operator+=(int value)
    {
        ptr_ += value;
        return *this;
    }
    ArrayIndex2D &operator-=(int value)
    {
        ptr_ -= value;
        return *this;
    }

    std::tuple<int, int> operator*()
    {
        int rawIndex = ptr_ - &arr_[{0, 0}];
        int x = rawIndex / nY_;
        int y = rawIndex % nY_;
        std::tuple<int, int> arrayIndex = std::make_tuple(x, y);
        return arrayIndex;
    }

    std::tuple<int, int> begin() { return std::make_tuple(0, 0); }

    std::tuple<int, int> end() { return std::make_tuple(nX_, nY_); }

    bool hasNext() { return ptr_ <= &arr_[{nX_, nY_}]; }

    private:
    int nX_;
    int nY_;
    const double *ptr_;
    const A &arr_;
};