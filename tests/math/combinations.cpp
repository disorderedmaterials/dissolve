// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/combinations.h"
#include "templates/algorithms.h"

#include <gtest/gtest.h>
#include <vector>

namespace
{
bool combinationInVector(std::pair<int, int> &combination, std::vector<std::pair<int, int>> &vec)
{
    auto result = std::find(std::begin(vec), std::end(vec), combination);
    return result != std::end(vec);
}
} // namespace

namespace UnitTest
{

TEST(CombinationTest, CheckNumberOfCombinations)
{
    int numTests = 500;

    // Get the expected combinations if were just doing a double loop
    std::vector<std::pair<int, int>> expectedCombinations;
    for (int i = 0; i < numTests; ++i)
    {
        for (int j = i + 1; j < numTests; ++j)
        {
            expectedCombinations.emplace_back(i, j);
        }
    }
    std::vector<std::pair<int, int>> combinations;

    Combinations comb(numTests);
    for (auto i = 0; i < comb.getNumCombinations(); ++i)
    {
        auto [m, n] = comb.nthCombination(i);

        combinations.emplace_back(m, n);
    }

    EXPECT_EQ(combinations.size(), expectedCombinations.size());
    EXPECT_EQ(combinations.size(), 0.5 * numTests * (numTests - 1));
}
TEST(CombinationTest, CheckCombinations)
{
    int numTests = 500;

    // Get the expected combinations if were just doing a double loop
    std::vector<std::pair<int, int>> expectedCombinations;
    for (int i = 0; i < numTests; ++i)
    {
        for (int j = i + 1; j < numTests; ++j)
        {
            expectedCombinations.emplace_back(i, j);
        }
    }
    std::vector<std::pair<int, int>> combinations;

    Combinations comb(numTests);
    for (auto i = 0; i < comb.getNumCombinations(); ++i)
    {
        auto [m, n] = comb.nthCombination(i);

        combinations.emplace_back(m, n);
    }

    for (auto [computed, expected] : zip(combinations, expectedCombinations))
    {
        EXPECT_EQ(computed, expected);
    }
    auto it = std::unique(combinations.begin(), combinations.end());
    EXPECT_TRUE(it == std::end(combinations));
}

} // namespace UnitTest
