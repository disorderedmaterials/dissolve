// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/integer.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(IntegerKeywordTest, Basic)
{
    // Unlimited value
    int value_{5};
    IntegerKeyword keyword(value_);

    // Check initial value
    EXPECT_EQ(keyword.data(), 5);

    // Set an actual value
    EXPECT_TRUE(keyword.setData(10));
    EXPECT_EQ(value_, 10);

    // And another
    EXPECT_TRUE(keyword.setData(-123));
    EXPECT_EQ(value_, -123);
}

TEST(IntegerKeywordTest, Clamped)
{
    // Minimum at 0.0, maximum at 100.0
    int value_{5};
    IntegerKeyword keyword(value_, 0, 100);

    // Set an acceptable value within the limits
    EXPECT_TRUE(keyword.setData(40));
    EXPECT_EQ(value_, 40);

    // Set to the allowed limits
    EXPECT_TRUE(keyword.setData(0));
    EXPECT_EQ(value_, 0);
    EXPECT_TRUE(keyword.setData(100));
    EXPECT_EQ(value_, 100);

    // Try to set outside the allowed limits
    value_ = 50.0;
    EXPECT_FALSE(keyword.setData(-1));
    EXPECT_EQ(value_, 50);
    EXPECT_FALSE(keyword.setData(101));
    EXPECT_EQ(value_, 50);
}

} // namespace UnitTest
