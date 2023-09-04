// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/array3DIterator.h"
#include "templates/algorithms.h"
#include "templates/array3D.h"
#include "templates/vector3.h"
#include <gtest/gtest.h>

#include "iostream"
namespace UnitTest
{

TEST(Array3DIteratorTest, ForEach)
{
    int x, y, z, sizeX = 10, sizeY = 10, sizeZ = 10;
    Array3D<std::pair<Vec3<int>, bool>> arr;
    arr.initialise(sizeX, sizeY, sizeZ);

    Array3DIterator it(sizeX, sizeY, sizeZ);

    dissolve::for_each_triplet(ParallelPolicies::par, it.begin(), it.end(),
                               [&](auto triplet, auto x1, auto y1, auto z1) {
                                   arr[triplet] = {Vec3<int>{x1, y1, z1}, true};
                               });
    for (x = 0; x < sizeX; ++x)
        for (y = 0; y < sizeY; ++y)
            for (z = 0; z < sizeZ; ++z)
            {
                Vec3<int> vec = arr[std::tuple{x, y, z}].first;
                bool flag = arr[std::tuple{x, y, z}].second;
                EXPECT_EQ(x, vec.x);
                EXPECT_EQ(y, vec.y);
                EXPECT_EQ(z, vec.z);
                EXPECT_EQ(true, flag);
            }
}

TEST(Array3DIteratorTest, Iterator)
{
    int x, y, z, sizeX = 10, sizeY = 10, sizeZ = 10;
    Array3D<std::pair<Vec3<int>, bool>> arr;
    arr.initialise(sizeX, sizeY, sizeZ);

    Array3DIterator it(sizeX, sizeY, sizeZ);

    for (auto i = it.begin(), end = it.end(); i != end; ++i)
    {
        auto [x, y, z] = *i;
        arr[std::tuple{x, y, z}] = {Vec3<int>{x, y, z}, true};
    }
    for (x = 0; x < sizeX; ++x)
        for (y = 0; y < sizeY; ++y)
            for (z = 0; z < sizeZ; ++z)
            {
                auto vec = arr[std::tuple{x, y, z}].first;
                auto flag = arr[std::tuple{x, y, z}].second;
                EXPECT_EQ(x, vec.x);
                EXPECT_EQ(y, vec.y);
                EXPECT_EQ(z, vec.z);
                EXPECT_EQ(flag, true);
            }
}
} // namespace UnitTest
