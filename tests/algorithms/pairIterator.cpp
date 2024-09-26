// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/pairIterator.h"
#include "classes/fullPairIterator.h"
#include "templates/algorithms.h"
#include "templates/array2D.h"

#include <gtest/gtest.h>
#include <numeric>
#include <random>

namespace UnitTest
{

void for_each_test(bool unordered)
{
    const int size = 100;

    Array2D<int> store(size, size, unordered);

    std::random_device rd;                           // a seed source for the random number engine
    std::mt19937 gen(rd());                          // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(1, 100); // Generate a random number in [1, 100]

    int total = 0;
    for (auto &item : store)
    {
        item = distrib(gen);
        total += item;
    }

    int sum = 0;
    dissolve::for_each_pair(
        ParallelPolicies::seq, 0, size, [&sum, &store](const auto i, const auto j) { sum += store[i, j]; }, unordered);

    EXPECT_EQ(total, sum);
}

TEST(PairIterTest, Pairs)
{
    const int size = 100;
    PairIterator it(size);

    Array2D<int> store(size, size, true);

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

TEST(PairIterTest, for_each_pair) { for_each_test(true); }

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

TEST(PairIterTest, for_each_full_pair) { for_each_test(false); }
} // namespace UnitTest
