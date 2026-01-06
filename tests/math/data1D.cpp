// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/data1D.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(Data1DTest, Serialisation)
{
    Data1D a, b;
    for (auto x = 0.0; x < M_PI * 2.0; x += 0.1)
        a.addPoint(x, sin(x));

    for (auto n = 0; n < 2; ++n)
    {
        EXPECT_NO_THROW(tomlRoundTrip(a, b));
        ASSERT_EQ(a.nValues(), b.nValues());
        for (auto &&[ax, ay, bx, by] : zip(a.xAxis(), a.values(), b.xAxis(), b.values()))
        {
            EXPECT_EQ(ax, bx);
            EXPECT_EQ(ay, by);
        }

        // Add an extra point to test clearing of the old data
        b.addPoint(100.0, 0.0);
    }
}

TEST(Data1DTest, SerialisationWithErrors)
{
    Data1D a, b;
    a.initialise(1, true);
    b.initialise(1, true);
    for (auto x = 0.0; x < M_PI * 2.0; x += 0.1)
        a.addPoint(x, sin(x), pow(x, 1.5));

    for (auto n = 0; n < 2; ++n)
    {
        EXPECT_NO_THROW(tomlRoundTrip(a, b));
        ASSERT_EQ(a.nValues(), b.nValues());
        for (auto &&[ax, ay, ae, bx, by, be] : zip(a.xAxis(), a.values(), a.errors(), b.xAxis(), b.values(), b.errors()))
        {
            EXPECT_EQ(ax, bx);
            EXPECT_EQ(ay, by);
            EXPECT_EQ(ae, be);
        }

        // Add an extra point to test clearing of the old data
        b.addPoint(100.0, 0.0, 0.0);
    }
}

} // namespace UnitTest
