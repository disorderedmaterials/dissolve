// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "math/history.h"
#include "nodes/number.h"
#include <gtest/gtest.h>
#include <tests/testData.h>

namespace UnitTest
{

TEST(History, SimpleIntegerNumbers)
{
    History<Number> i;
    const auto value = 12345;
    for (auto n = 0; n < 10; ++n)
        EXPECT_EQ(i.push(value, 5).asInteger(), value);
}

TEST(History, SimpleDoubleNumbers)
{
    History<Number> d;
    const auto avgLength = 3;
    EXPECT_DOUBLE_EQ(d.push(1.0, avgLength).asDouble(), 1.0);
    EXPECT_DOUBLE_EQ(d.push(2.0, avgLength).asDouble(), (1 + 2) / 2.0);
    EXPECT_DOUBLE_EQ(d.push(3.0, avgLength).asDouble(), (1 + 2 + 3) / 3.0);
    EXPECT_DOUBLE_EQ(d.push(4.0, avgLength).asDouble(), (2 + 3 + 4) / 3.0);
    EXPECT_DOUBLE_EQ(d.push(5.0, avgLength).asDouble(), (3 + 4 + 5) / 3.0);
}

TEST(History, SimpleDeserialisation)
{
    History<Number> a, b;

    const auto avgLength = 5;
    a.push(1.0, avgLength).asDouble();
    a.push(2.0, avgLength).asDouble();
    a.push(3.0, avgLength).asDouble();
    a.push(4.0, avgLength).asDouble();
    a.push(5.0, avgLength).asDouble();

    tomlRoundTrip(a, b);
    EXPECT_EQ(a.average(), b.average());

    // Push another value to b to test clearing.
    b.push(100.0, avgLength);
    tomlRoundTrip(a, b);
    EXPECT_EQ(a.average(), b.average());
}

} // namespace UnitTest