// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "math/data1D.h"
#include "math/histogram1D.h"
#include "math/sampledData1D.h"
#include "math/sampledDouble.h"
#include "math/sampledVector.h"
#include "math/vector3.h"
#include "nodes/serialisableData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
// Serialise A and deserialise into B
template <class T> void roundTrip(T &a, T &b)
{
    // Serialise
    auto s = std::make_shared<SerialisableClass<T>>("data", a);
    auto serialised = s->serialise();

    // Deserialise
    auto d = std::make_shared<SerialisableClass<T>>("data", b);
    d->deserialise(serialised);
}

TEST(SerialisableTest, Data1D)
{
    Data1D a, b;
    for (auto x = 0.0; x < M_PI * 2.0; x += 0.1)
        a.addPoint(x, sin(x));

    for (auto n = 0; n < 2; ++n)
    {
        EXPECT_NO_THROW(roundTrip(a, b));
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

TEST(SerialisableTest, Data1DErrors)
{
    Data1D a, b;
    a.initialise(1, true);
    b.initialise(1, true);
    for (auto x = 0.0; x < M_PI * 2.0; x += 0.1)
        a.addPoint(x, sin(x), pow(x, 1.5));

    for (auto n = 0; n < 2; ++n)
    {
        EXPECT_NO_THROW(roundTrip(a, b));
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

TEST(SerialisableTest, SampledDouble)
{
    SampledDouble a, b;
    a += 1.0;
    a += 2.0;
    a += 5.0;

    for (auto n = 0; n < 2; ++n)
    {
        EXPECT_NO_THROW(roundTrip(a, b));
        ASSERT_EQ(a, b);
        ASSERT_EQ(a.count(), b.count());

        // Add an extra point to test clearing of the old data
        a += 10.0;
    }
}

TEST(SerialisableTest, SampledData1D)
{
    SampledData1D a, b;
    a.initialise(3);
    a += {1.0, 0.0, 3.0};
    a += {9.0, 0.0, 2.0};
    a += {2.0, 6.0, 1.0};

    for (auto n = 0; n < 2; ++n)
    {
        EXPECT_NO_THROW(roundTrip(a, b));
        ASSERT_EQ(a.nValues(), b.nValues());
        for (auto &&[aValue, aError, bValue, bError] : zip(a.values(), a.errors(), b.values(), b.errors()))
        {
            EXPECT_DOUBLE_EQ(aValue, bValue);
            EXPECT_DOUBLE_EQ(aError, bError);
        }

        // Add extra data to test clearing of the old data
        a += {5.0, 5.0, 5.0};
    }
}

TEST(SerialisableTest, Histogram1D)
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
        EXPECT_NO_THROW(roundTrip(a, b));
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

TEST(SerialisableTest, SampledVector)
{
    SampledVector a, b;
    a.initialise(3);
    a += {1.0, 0.0, 3.0};
    a += {9.0, 0.0, 2.0};
    a += {2.0, 6.0, 1.0};

    for (auto n = 0; n < 2; ++n)
    {
        EXPECT_NO_THROW(roundTrip(a, b));
        ASSERT_EQ(a.count(), b.count());
        for (auto &&[aValue, aStDev, am2, bValue, bStDev, bm2] :
             zip(a.values(), a.stDev(), a.m2(), b.values(), b.stDev(), b.m2()))
        {
            EXPECT_DOUBLE_EQ(aValue, bValue);
            EXPECT_DOUBLE_EQ(aStDev, bStDev);
            EXPECT_DOUBLE_EQ(am2, bm2);
        }

        // Add extra data to test clearing of the old data
        a += {5.0, 5.0, 5.0};
    }
}

TEST(SerialisableTest, Vector3Vector)
{
    std::vector<Vector3> a = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 1.0}}, b;

    for (auto n = 0; n < 2; ++n)
    {
        EXPECT_NO_THROW(roundTrip(a, b));
        EXPECT_EQ(a.size(), b.size());
        EXPECT_EQ(a, b);

        // Test clearing
        b = {{1.0, 1.0, 1.0}, {2.0, 2.0, 2.0}};
    }
}

} // namespace UnitTest
