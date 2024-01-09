// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/integerHistogram1D.h"
#include "templates/algorithms.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{

void testHistogram(const IntegerHistogram1D &h, const std::vector<int> &originalData)
{
    // Determine whether the accumulated data in the supplied histogram is consistent with the source data
    auto expectedMinimum = *std::min_element(originalData.begin(), originalData.end());
    if (h.minimum())
        expectedMinimum = std::min(*h.minimum(), expectedMinimum);
    auto expectedMaximum = *std::max_element(originalData.begin(), originalData.end());
    if (h.maximum())
        expectedMaximum = std::min(*h.maximum(), expectedMaximum);

    auto expectedNBins = (expectedMaximum - expectedMinimum) + 1;

    // Check expected number of bins
    auto &accData = h.accumulatedData();
    EXPECT_EQ(expectedNBins, accData.xAxis().size());

    // Check data within bins
    auto currentBin = expectedMinimum;
    for (auto &&[x, value] : zip(accData.xAxis(), accData.values()))
    {
        // Bin / x
        EXPECT_NEAR(x, currentBin, 1.0e-4);

        // Value
        EXPECT_NEAR(value, std::count(originalData.begin(), originalData.end(), currentBin), 1.0e-4);

        ++currentBin;
    }
}

TEST(IntegerHistogram1D, Basic)
{
    std::vector<int> data{2, 6, 7};
    IntegerHistogram1D h;

    // Add data to histogram and accumulate
    for (auto x : data)
        h.bin(x);
    h.accumulate();

    // Check accumulated data
    testHistogram(h, data);
}

TEST(IntegerHistogram1D, Negatives)
{
    std::vector<int> data{2, -5, 7, -1}, binnedData{2, 7};
    IntegerHistogram1D h;

    // Add data to histogram and accumulate
    for (auto x : data)
        h.bin(x);
    h.accumulate();

    // Check accumulated data
    testHistogram(h, binnedData);
}

TEST(IntegerHistogram1D, Limits1)
{
    std::vector<int> data{2, -5, 7, -1}, binnedData{-1, 2, 7};
    IntegerHistogram1D h;
    h.initialise(-3, {});

    // Add data to histogram and accumulate
    for (auto x : data)
        h.bin(x);
    h.accumulate();

    // Check accumulated data
    testHistogram(h, binnedData);
}

TEST(IntegerHistogram1D, Limits2)
{
    std::vector<int> data{2, -5, 7, -1}, binnedData{-1, 2};
    IntegerHistogram1D h;
    h.initialise(-3, 2);

    // Add data to histogram and accumulate
    for (auto x : data)
        h.bin(x);
    h.accumulate();

    // Check accumulated data
    testHistogram(h, binnedData);
}

TEST(IntegerHistogram1D, Duplicates)
{
    std::vector<int> data{2, -5, 7, -1, 7, 7, -1, 2, 2, 2}, binnedData{-1, -1, 2, 2, 2, 2};
    IntegerHistogram1D h;
    h.initialise(-3, 2);

    // Add data to histogram and accumulate
    for (auto x : data)
        h.bin(x);
    h.accumulate();

    // Check accumulated data
    testHistogram(h, binnedData);
}

} // namespace UnitTest