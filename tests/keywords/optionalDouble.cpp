// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/optionalDouble.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(OptionalDoubleTest, Standard)
{
    // Minimum at zero, no maximum
    std::optional<double> value_;
    OptionalDoubleKeyword keyword(value_, 0.0, std::nullopt, 0.5, "Null Text");

    // Null value_ to start with
    EXPECT_TRUE(!value_);
    // Set ot an actual value
    EXPECT_TRUE(keyword.setData(1.0));
    EXPECT_TRUE(value_);
    EXPECT_NEAR(value_.value(), 1.0, 1.0e-8);
    // Set back to the value indicating null
    EXPECT_TRUE(keyword.setData(0.0));
    EXPECT_TRUE(!value_);
    // Set below the minimum value indicating null
    EXPECT_TRUE(keyword.setData(-100.0));
    EXPECT_TRUE(!value_);
}

TEST(OptionalDoubleTest, Clamped)
{
    // Minimum at 10.0, maximum at 100.0
    std::optional<double> value_;
    OptionalDoubleKeyword keyword(value_, 10.0, 100.0, 1.0, "Null Text");

    // Null value_ to start with
    EXPECT_TRUE(!value_);
    // Set the value representing null
    EXPECT_TRUE(keyword.setData(10.0));
    EXPECT_TRUE(!value_);
    // Set ot an actual value
    EXPECT_TRUE(keyword.setData(50.0));
    EXPECT_TRUE(value_);
    EXPECT_NEAR(value_.value(), 50.0, 1.0e-8);
    // Set the maximum value
    EXPECT_TRUE(keyword.setData(100.0));
    EXPECT_TRUE(value_);
    EXPECT_NEAR(value_.value(), 100.0, 1.0e-8);
    // Set above the maximum value (fail)
    EXPECT_FALSE(keyword.setData(105.0));
    EXPECT_TRUE(value_);
    EXPECT_NEAR(value_.value(), 100.0, 1.0e-8);
    // Set below the minimum value indicating null
    EXPECT_TRUE(keyword.setData(-100.0));
    EXPECT_TRUE(!value_);
}

} // namespace UnitTest
