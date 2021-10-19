// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/pair_iter.h"

#include <gtest/gtest.h>

namespace UnitTest
{

TEST(PairIterTest, Pairs)
{
    int x, y, size = 100;
    PairIterator it(100);
    for (x = 0; x < size; ++x)
    {
        for (y = x; y < size; ++y)
        {
            auto [i, j] = *it;
            EXPECT_EQ(i, x);
            EXPECT_EQ(j, y);
            ++it;
        }
    }
}
} // namespace UnitTest
