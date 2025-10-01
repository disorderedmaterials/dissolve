// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "math/data1D.h"
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
    for (auto x = 0.0; x < M_PI*2.0; x += 0.1)
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
    for (auto x = 0.0; x < M_PI*2.0; x += 0.1)
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

TEST(SerialisableTest, Vector3Vector)
{
    std::vector<Vector3> a = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 1.0}}, b;

    for (auto n = 0; n < 2; ++n)
    {
        EXPECT_NO_THROW(roundTrip(a, b));
        EXPECT_EQ(a.size(), b.size());
        EXPECT_EQ(a, b);

        // Test clearing
        b = {{1.0, 1.0, 1.0},  {2.0, 2.0, 2.0}};
    }
}

} // namespace UnitTest
