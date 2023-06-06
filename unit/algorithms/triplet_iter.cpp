// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/triplet_iterator.h"

#include <gtest/gtest.h>

namespace UnitTest
{

TEST(TripletIteratorTest, Triplets)
{
    int x, y, z, sizeX = 100, sizeY = 100, sizeZ = 100;
    TripletIterator it(sizeX, sizeY, sizeZ);
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
} // namespace UnitTest

