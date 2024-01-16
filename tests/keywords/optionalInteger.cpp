// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/optionalInt.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(OptionalIntegerTest, Standard)
{
    // Minimum at zero, no maximum
    std::optional<int> value_;
    OptionalIntegerKeyword keyword(value_, 0, std::nullopt, 1, "Null Text");

    // Null value_ to start with
    EXPECT_TRUE(!value_);
    // Set an actual value
    EXPECT_TRUE(keyword.setData(1));
    EXPECT_TRUE(value_);
    EXPECT_EQ(value_.value(), 1);
    // Set back to the value indicating null
    EXPECT_TRUE(keyword.setData(0));
    EXPECT_TRUE(!value_);
    // Set below the minimum value indicating null
    EXPECT_TRUE(keyword.setData(-100));
    EXPECT_TRUE(!value_);
}

TEST(OptionalIntegerTest, Clamped)
{
    // Minimum at 10, maximum at 100
    std::optional<int> value_;
    OptionalIntegerKeyword keyword(value_, 10, 100, 1, "Null Text");

    // Null value_ to start with
    EXPECT_TRUE(!value_);
    // Set the value representing null
    EXPECT_TRUE(keyword.setData(10));
    EXPECT_TRUE(!value_);
    // Set an actual value
    EXPECT_TRUE(keyword.setData(50));
    EXPECT_TRUE(value_);
    EXPECT_EQ(value_.value(), 50);
    // Set the maximum value
    EXPECT_TRUE(keyword.setData(100));
    EXPECT_TRUE(value_);
    EXPECT_EQ(value_.value(), 100);
    // Set above the maximum value (fail)
    EXPECT_FALSE(keyword.setData(105));
    EXPECT_TRUE(value_);
    EXPECT_EQ(value_.value(), 100);
    // Set below the minimum value indicating null
    EXPECT_TRUE(keyword.setData(-100));
    EXPECT_TRUE(!value_);
}

} // namespace UnitTest
