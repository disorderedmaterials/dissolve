// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/messenger.h"
#include "templates/array3d.h"
#include "templates/list.h"
#include "templates/vector3.h"
#include <numeric>
#include <tuple>
#include <vector>

template <class A> class ArrayIndex3D
{

    public:
    ArrayIndex3D(const A &inArray) : arr_(inArray)
    {
        nX_ = inArray.nX();
        nY_ = inArray.nY();
        nZ_ = inArray.nZ();
        ptr_ = &inArray[{0, 0, 0}];
    }

    ArrayIndex3D &operator++()
    {
        ++ptr_;
        return *this;
    }
    ArrayIndex3D operator++(int)
    {
        ArrayIndex3D temp = *this;
        ++*this;
        return temp;
    }
    ArrayIndex3D &operator--()
    {
        --ptr_;
        return *this;
    }
    ArrayIndex3D operator--(int)
    {
        ArrayIndex3D temp = *this;
        --*this;
        return temp;
    }
    ArrayIndex3D &operator+=(int value)
    {
        ptr_ += value;
        return *this;
    }
    ArrayIndex3D &operator-=(int value)
    {
        ptr_ -= value;
        return *this;
    }

    std::tuple<int, int, int> operator*()
    {
        int rawIndex = ptr_ - &arr_[{0, 0, 0}];
        int x = rawIndex / (nY_ * nZ_);
        int y = rawIndex % nY_ / nZ_;
        int z = rawIndex % nY_ % nZ_;
        std::tuple<int, int, int> arrayIndex = std::make_tuple(x, y, z);
        return arrayIndex;
    }

    std::tuple<int, int, int> begin() { return std::make_tuple(0, 0, 0); }

    std::tuple<int, int, int> end() { return std::make_tuple(nX_, nY_, nZ_); }

    bool hasNext() { return (ptr_ <= &arr_[{nX_, nY_, nZ_}]); }

    private:
    int nX_;
    int nY_;
    int nZ_;
    const double *ptr_;
    const A &arr_;
};