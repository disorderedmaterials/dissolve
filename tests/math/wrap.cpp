// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/mathFunc.h"
#include "math/vector3.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(WrapIntegers, WrapIntegers)
{
    EXPECT_EQ(DissolveMath::wrap(2, 0, 4), 2);
    EXPECT_EQ(DissolveMath::wrap(0, 0, 4), 0);
    EXPECT_EQ(DissolveMath::wrap(-5, 0, 4), 0);
    EXPECT_EQ(DissolveMath::wrap(-15, 0, 4), 0);
    EXPECT_EQ(DissolveMath::wrap(20, 0, 4), 0);
    EXPECT_EQ(DissolveMath::wrap(4, 0, 4), 4);
    EXPECT_EQ(DissolveMath::wrap(8, 0, 4), 3);
    EXPECT_EQ(DissolveMath::wrap(9, 0, 4), 4);

    EXPECT_EQ(DissolveMath::wrap(2, -5, 5), 2);
    EXPECT_EQ(DissolveMath::wrap(0, -5, 5), 0);
    EXPECT_EQ(DissolveMath::wrap(-5, -5, 5), -5);
    EXPECT_EQ(DissolveMath::wrap(-16, -5, 5), -5);
    EXPECT_EQ(DissolveMath::wrap(5, -5, 5), 5);
    EXPECT_EQ(DissolveMath::wrap(16, -5, 5), 5);
    EXPECT_EQ(DissolveMath::wrap(6, -5, 5), -5);
}

} // namespace UnitTest
