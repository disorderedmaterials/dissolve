// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/histogram1D.h"
#include "tests/testing.h"

namespace UnitTest
{
TEST(Histogram1DTest, Serialisation)
{
    Histogram1D a, b;
    a.initialise(1.0, 5.0, 1.0);
    a.bin(1.5);
    a.bin(0.2);
    a.bin(1.5);
    a.bin(4.0);
    a.accumulate();

    for (auto n = 0; n < 2; ++n)
    {
        EXPECT_NO_THROW(tomlRoundTrip(a, b));
        ASSERT_EQ(a.nBins(), b.nBins());
        ASSERT_EQ(a.minimum(), b.minimum());
        ASSERT_EQ(a.maximum(), b.maximum());
        ASSERT_EQ(a.binWidth(), b.binWidth());
        for (auto &&[aValue, bValue] : zip(a.accumulatedData().values(), b.accumulatedData().values()))
            EXPECT_DOUBLE_EQ(aValue, bValue);

        // Add an extra point to test clearing of the old data
        a.zeroBins();
        a.bin(2.0);
        a.accumulate();
    }
}

} // namespace UnitTest
