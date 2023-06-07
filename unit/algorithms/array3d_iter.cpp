// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/array3d_iter.h"
#include "templates/algorithms.h"
#include "templates/array3d.h"
#include "templates/vector3.h"
#include <gtest/gtest.h>

#include "iostream"
namespace UnitTest
{

TEST(Array3DIteratorTest, Array3D)
{
    int x, y, z, sizeX = 100, sizeY = 100, sizeZ = 100;
    Array3DIterator it(sizeX, sizeY, sizeZ);
    for (x = 0; x < sizeX; ++x)
    {
        for (y = 0; y < sizeY; ++y)
        {
            for (z = 0; z < sizeZ; ++z)
            {
                auto [i, j, k] = *it;
                EXPECT_EQ(i, x);
                EXPECT_EQ(j, y);
                EXPECT_EQ(k, z);
                ++it;
            }
        }
    }
}

TEST(Array3DIteratorTest, SegFault)
{
    int x, y, z, sizeX = 10, sizeY = 10, sizeZ = 10;
    Array3D<std::pair<Vec3<int>, bool>> arr;
    arr.initialise(sizeX, sizeY, sizeZ);

    Array3DIterator it(sizeX, sizeY, sizeZ);
    EXPECT_EQ(it.end(), it.begin()+ 999);
    dissolve::for_each_triplet(ParallelPolicies::seq, it.begin(), it.end(),
                                [&](auto x1, auto y1, auto z1)
                                {
                                    arr[{x1,y1,z1}] = {Vec3<int>{x1,y1,z1}, true};
                                });
    for (x = 0; x < sizeX; ++x)
        for (y=0; y< sizeY; ++y)
            for (z=0; z<sizeZ; ++z)
            {
                Vec3<int> vec = arr[{x,y,z}].first;
                bool flag = arr[{x,y,z}].second;
                EXPECT_EQ(x, vec.x);
                EXPECT_EQ(y, vec.y);
                EXPECT_EQ(z, vec.z);
                EXPECT_EQ(true, flag);
            }
}
} // namespace UnitTest
