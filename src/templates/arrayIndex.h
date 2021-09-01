// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/messenger.h"
#include "templates/list.h"
#include "templates/vector3.h"
#include "templates/array3d.h"
#include <numeric>
#include <tuple>
#include <vector>


template <class A> class ArrayIndex
{
    public:
    ArrayIndex(Array3D& inArray)
    {
        nX = inArray.nX();
        nY = inArray.nY();
        nZ = inArray.nZ();
        ptr* = &inArray[{0,0,0}];
    }

    Arrayindex& operator++()
    {
        ++ptr;
        return *this;
    }
    Arrayindex& operator--()
    {
        --ptr;
        return *this;
    }
    std::tuple<int,int,int> operator*()
    {
        int rawIndex = ptr - &inArray[{0,0}];
        int x = rawIndex/(nY*nZ);
        int y = rawIndex%nY/nZ;
        int z = rawIndex%nY%nZ;
        std::tuple<int,int,int> arrayIndex = std::make_tuple(x,y,z);
        return arrayIndex;
    }
    
    std::tuple<int,int,int> begin()
    {
        return std::make_tuple(0,0,0);
    }

    std::tuple<int,int,int> end()
    {
        return std::make_tuple(nX,nY,nZ);
    }

    private:
        int nX;nY;nZ;
        int ptr*;
}