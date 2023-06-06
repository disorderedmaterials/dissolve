// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/triplet_iterator.h"

#include <gtest/gtest.h>

namespace UnitTest
{

TEST(TripletIteratorTest, Triplets)
{
    int x, y, z, size = 100;
    TripletIterator it(100);
    for (x = 0; x < size; ++x)
    {
        for (y = x; y < size; ++y)
        {
            for (z = y; z < size; ++z)
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

