// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "templates/algorithms.h"
#include "templates/arrayIndex2D.h"
#include "templates/arrayIndex3D.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(ArrayIterationTest, IndexIterator)
{
    int x = 3;
    int y = 4;
    int z = 5;
    Array2D<double> array2(x, y);
    for (auto i = 0; i < array2.nRows(); ++i)
        for (auto j = 0; j < array2.nColumns(); ++j)
            array2[{i, j}] = i * array2.nColumns() + j;

    Array3D<double> array3(x, y, z);
    for (auto i = 0; i < array3.nX(); ++i)
        for (auto j = 0; j < array3.nY(); ++j)
            for (auto k = 0; k < array3.nZ(); ++k)
                array3[std::tuple{i, j, k}] = i * array3.nY() + j * array3.nZ() + k;

    EXPECT_EQ(*(ArrayIndex2D(array2).begin()), std::make_tuple(0, 0));
    EXPECT_EQ(*(ArrayIndex2D(array2).end()), std::make_tuple(x, 0));

    EXPECT_EQ(*(ArrayIndex3D(array3).begin()), std::make_tuple(0, 0, 0));
    EXPECT_EQ(*(ArrayIndex3D(array3).end()), std::make_tuple(x, 0, 0));

    int i = 0;
    int j = 0;
    for (auto index : ArrayIndex2D(array2))
    {
        EXPECT_EQ(index, std::make_tuple(i, j));
        if (j < y - 1)
            j++;
        else
        {
            j = 0;
            i++;
        }
    }

    auto index2 = ArrayIndex2D(array2);
    for (auto i = 0; i < array2.nRows(); ++i)
        for (auto j = 0; j < array2.nColumns(); ++j)
            EXPECT_EQ(*index2++, std::make_tuple(i, j));

    i = 0;
    j = 0;
    int k = 0;
    for (auto index2 : ArrayIndex3D(array3))
    {
        EXPECT_EQ(index2, std::make_tuple(i, j, k));
        if (k < z - 1)
            k++;
        else if (j < y - 1)
        {
            k = 0;
            j++;
        }
        else
        {
            k = 0;
            j = 0;
            i++;
        }
    }

    auto index3 = ArrayIndex3D(array3);
    for (auto i = 0; i < array3.nX(); ++i)
        for (auto j = 0; j < array3.nY(); ++j)
            for (auto k = 0; k < array3.nZ(); ++k)
                EXPECT_EQ(*index3++, std::make_tuple(i, j, k));
}

} // namespace UnitTest
