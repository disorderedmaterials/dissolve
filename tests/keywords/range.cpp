// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/range.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(RangeKeywordTest, Basic)
{
    // Unlimited range
    Range value_;
    RangeKeyword keyword(value_);

    // Set an actual value
    EXPECT_TRUE(keyword.setData(1.5, 4.5));
    EXPECT_NEAR(value_.minimum(), 1.5, 1.0e-8);
    EXPECT_NEAR(value_.maximum(), 4.5, 1.0e-8);

    // Set the same numbers but with max > min
    EXPECT_TRUE(keyword.setData(4.5, 1.5));
    EXPECT_NEAR(value_.minimum(), 1.5, 1.0e-8);
    EXPECT_NEAR(value_.maximum(), 4.5, 1.0e-8);
}

TEST(RangeKeywordTest, Clamped)
{
    // Minimum at 0.0, maximum at 100.0
    Range value_;
    RangeKeyword keyword(value_, 0.0, 100.0);

    // Set an acceptable range within the limits
    EXPECT_TRUE(keyword.setData(40.0, 60.0));
    EXPECT_NEAR(value_.minimum(), 40.0, 1.0e-8);
    EXPECT_NEAR(value_.maximum(), 60.0, 1.0e-8);

    // Set range to the allowed limits
    EXPECT_TRUE(keyword.setData(0.0, 100.0));
    EXPECT_NEAR(value_.minimum(), 0.0, 1.0e-8);
    EXPECT_NEAR(value_.maximum(), 100.0, 1.0e-8);

    // Try to set outside the allowed limits
    EXPECT_FALSE(keyword.setMinimum(-1.0));
    EXPECT_NEAR(value_.minimum(), 0.0, 1.0e-8);
    EXPECT_FALSE(keyword.setMaximum(101.0));
    EXPECT_NEAR(value_.maximum(), 100.0, 1.0e-8);
    EXPECT_FALSE(keyword.setData(-1.0, 50.0));
    EXPECT_NEAR(value_.minimum(), 0.0, 1.0e-8);
    EXPECT_NEAR(value_.maximum(), 100.0, 1.0e-8);
    EXPECT_FALSE(keyword.setData(0.0, 101.0));
    EXPECT_NEAR(value_.minimum(), 0.0, 1.0e-8);
    EXPECT_NEAR(value_.maximum(), 100.0, 1.0e-8);
}

} // namespace UnitTest
