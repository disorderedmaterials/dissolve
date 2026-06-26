// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/rangedVector3.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(RangedVector3Test, Construction)
{
    // No limiting
    RangedVector3 v1({1.0, 2.0, 3.0});
    EXPECT_DOUBLE_EQ(v1.x(), 1.0);
    EXPECT_DOUBLE_EQ(v1.y(), 2.0);
    EXPECT_DOUBLE_EQ(v1.z(), 3.0);

    // Limiting on construction - minimum only
    RangedVector3 v2({1.0, 2.0, 3.0}, Vector3{2.0, 0.0, 0.0});
    EXPECT_DOUBLE_EQ(v2.x(), 2.0);
    EXPECT_DOUBLE_EQ(v2.y(), 2.0);
    EXPECT_DOUBLE_EQ(v2.z(), 3.0);

    // Limiting on construction - minimum and maximum
    RangedVector3 v3({1.0, 2.0, 3.0}, Vector3{2.0, 0.0, 0.0}, Vector3{10.0, 1.0, 0.0});
    EXPECT_DOUBLE_EQ(v3.x(), 2.0);
    EXPECT_DOUBLE_EQ(v3.y(), 1.0);
    EXPECT_DOUBLE_EQ(v3.z(), 0.0);
}

TEST(RangedVector3Test, Set)
{
    RangedVector3 v({}, Vector3(-1.0, -2.0, -3.0), Vector3(1.0, 2.0, 3.0));
    EXPECT_DOUBLE_EQ(v.x(), 0.0);
    EXPECT_DOUBLE_EQ(v.y(), 0.0);
    EXPECT_DOUBLE_EQ(v.z(), 0.0);

    v.set({-3.0, -3.0, -3.0});
    EXPECT_DOUBLE_EQ(v.x(), -1.0);
    EXPECT_DOUBLE_EQ(v.y(), -2.0);
    EXPECT_DOUBLE_EQ(v.z(), -3.0);

    v.set({2.0, 1.0, 6.0});
    EXPECT_DOUBLE_EQ(v.x(), 1.0);
    EXPECT_DOUBLE_EQ(v.y(), 1.0);
    EXPECT_DOUBLE_EQ(v.z(), 3.0);
}
} // namespace UnitTest
