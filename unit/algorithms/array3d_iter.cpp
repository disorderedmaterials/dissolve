// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "templates/algorithms.h"
#include "classes/array3d_iter.h"
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
        for (y = x; y < sizeY; ++y)
        {
            for (z = y; z < sizeZ; ++z)
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
/*
template <typename ParalellPolicy, class Lam>
void for_each_triplet(ParalellPolicy policy, int sizeX, int sizeY, int sizeZ, Lam lambda)
{
    TripletIterator u(sizeX, sizeY, sizeZ);
    for_each(policy, u.begin(), u.end(),
             [&lambda](const auto triplet)
             {
                auto &[x,y,z] = triplet;
                lambda(x,y,z);
             });
}
*/


} // namespace UnitTest

