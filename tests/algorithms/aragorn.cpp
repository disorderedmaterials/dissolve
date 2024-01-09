// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "templates/algorithms.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>

namespace UnitTest
{

TEST(AragornTest, ArithmaticSeries)
{
    unsigned int size = 100000;
    std::vector<int> ints(size);
    std::iota(ints.begin(), ints.end(), 1);
    int total = std::accumulate(ints.begin(), ints.end(), 0);

    EXPECT_EQ(total, size * (size + 1) >> 1);

    // Try many different cut lengths
    for (int cuts = 1; cuts < 40; ++cuts)
    {
        int count = 0;
        // Sum each cut, then take the sum of the cuts
        for (int n = 0; n < cuts; ++n)
        {
            auto [begin, end] = chop_range(ints.begin(), ints.end(), cuts, n);
            count += std::accumulate(begin, end, 0);
        }
        // Make sure that the sum of the cuts in equal to the total
        EXPECT_EQ(total, count);
    }
}

} // namespace UnitTest
