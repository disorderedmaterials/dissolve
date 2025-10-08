// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "templates/doubleKeyedMap.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(DoubleKeyedMap, BasicMirrored)
{
    DoubleKeyedMap<int> map(true);

    // Add and retrieve an entry
    DoubleKeyedMapKey key("A", "B");
    map[key] = 10;
    EXPECT_TRUE(map.contains(key));
    EXPECT_TRUE(map.contains("A", "B"));
    EXPECT_EQ(map[key], 10);

    // Triangular mapping...
    EXPECT_TRUE(map.contains("B", "A"));

    // Remove key
    ASSERT_NO_THROW(map.erase("A", "B"));
    EXPECT_FALSE(map.contains("A", "B"));
}

TEST(DoubleKeyedMap, BasicFull)
{
    DoubleKeyedMap<int> map(false);

    // Add and retrieve an entry
    DoubleKeyedMapKey key("A", "B");
    map[key] = 10;
    EXPECT_TRUE(map.contains(key));
    EXPECT_TRUE(map.contains("A", "B"));
    EXPECT_EQ(map[key], 10);

    // Full mapping...
    EXPECT_FALSE(map.contains("B", "A"));
}

} // namespace UnitTest