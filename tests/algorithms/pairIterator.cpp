// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/pairIterator.h"
#include "classes/fullPairIterator.h"
#include "templates/array2D.h"

#include <gtest/gtest.h>
#include <numeric>
#include <random>

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

TEST(PairIterTest, FullPairs)
{
    const int size = 100;
    FullPairIterator it(size);

    Array2D<int> store(size, size, false);

    std::random_device rd;                           // a seed source for the random number engine
    std::mt19937 gen(rd());                          // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(1, 100); // Generate a random number in [1, 100]

    int total = 0;
    for (auto &item : store)
    {
        item = distrib(gen);
        total += item;
    }

    auto sum =
        std::accumulate(it.begin(), it.end(), 0, [&store](const auto acc, const auto index) { return acc + store[index]; });

    EXPECT_EQ(total, sum);
}
} // namespace UnitTest
