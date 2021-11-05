// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/data1d.h"
#include "math/data2d.h"
#include "math/data3d.h"
#include "templates/algorithms.h"
#include "templates/arrayIndex2d.h"
#include "templates/arrayIndex3d.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(ArrayIterationTest, IndexIterator)
{
    int x = 3;
    int y = 3;
    int z = 3;
    Array2D<int> array(x, y);
    for (auto i = 0; i < array.nRows(); ++i)
        for (auto j = 0; j < array.nColumns(); ++j)
            array[{i, j}] = i * array.nColumns() + j;

    Array3D<int> array(x, y, z);
    for (auto i = 0; i < array.nX(); ++i)
        for (auto j = 0; j < array.nY(); ++j)
            for (auto k = 0; k < array.nZ(); ++k)
                array[{i, j, k}] = i * array.nY() + j * array.nZ() + k;

    EXPECT_EQ(*(ArrayIndex2D(array).begin()), {0, 0});
    EXPECT_EQ(*(ArrayIndex2D(array).end()), {x - 1, y - 1});

    EXPECT_EQ(*(ArrayIndex3D(array).begin()), {0, 0, 0});
    EXPECT_EQ(*(ArrayIndex3D(array).end()), {x - 1, y - 1, z - 1});

    int i = 0;
    int j = 0;
    for (auto index : ArrayIndex2D(array))
    {
        EXPECT_EQ(index, {i, j});
        if (j < y - 1)
            j++;
        else
        {
            j = 0;
            i++;
        }
    }

    auto index = ArrayIndex2D(array);
    for (auto i = 0; i < array.nRows(); ++i)
        for (auto j = 0; j < array.nColumns(); ++j)
            EXPECT_EQ(*index++, {i, j});

    int i = 0;
    int j = 0;
    int k = 0;
    for (auto index : ArrayIndex3D(array))
    {
        EXPECT_EQ(index, {i, j});
        if (k < z - 1)
            k++ else if (j < y - 1)
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

    auto index = ArrayIndex3D(array);
    for (auto i = 0; i < array.nX(); ++i)
        for (auto j = 0; j < array.nY(); ++j)
            for (auto k = 0; k < array.nZ(); ++k)
                EXPECT_EQ(*index++, {i, j, k});
}

} // namespace UnitTest