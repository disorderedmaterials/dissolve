// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/double.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(DoubleKeywordTest, Basic)
{
    // Unlimited value
    double value_{5.0};
    DoubleKeyword keyword(value_);

    // Check initial value
    EXPECT_NEAR(keyword.data(), 5.0, 1.0e-8);

    // Set an actual value
    EXPECT_TRUE(keyword.setData(10.0));
    EXPECT_NEAR(value_, 10.0, 1.0e-8);

    // And another
    EXPECT_TRUE(keyword.setData(-123.0));
    EXPECT_NEAR(value_, -123.0, 1.0e-8);
}

TEST(DoubleKeywordTest, Clamped)
{
    // Minimum at 0.0, maximum at 100.0
    double value_{5.0};
    DoubleKeyword keyword(value_, 0.0, 100.0);

    // Set an acceptable value within the limits
    EXPECT_TRUE(keyword.setData(40.0));
    EXPECT_NEAR(value_, 40.0, 1.0e-8);

    // Set to the allowed limits
    EXPECT_TRUE(keyword.setData(0.0));
    EXPECT_NEAR(value_, 0.0, 1.0e-8);
    EXPECT_TRUE(keyword.setData(100.0));
    EXPECT_NEAR(value_, 100.0, 1.0e-8);

    // Try to set outside the allowed limits
    value_ = 50.0;
    EXPECT_FALSE(keyword.setData(-1.0));
    EXPECT_NEAR(value_, 50.0, 1.0e-8);
    EXPECT_FALSE(keyword.setData(101.0));
    EXPECT_NEAR(value_, 50.0, 1.0e-8);
}

} // namespace UnitTest
