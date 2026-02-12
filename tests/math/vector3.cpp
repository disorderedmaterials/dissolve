// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/vector3.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(SerialisableTest, Vector3Vector)
{
    std::vector<Vector3> a = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 1.0}}, b;

    for (auto n = 0; n < 2; ++n)
    {
        EXPECT_NO_THROW(tomlRoundTrip(a, b));
        EXPECT_EQ(a.size(), b.size());
        EXPECT_EQ(a, b);

        // Test clearing
        b = {{1.0, 1.0, 1.0}, {2.0, 2.0, 2.0}};
    }
}

} // namespace UnitTest
