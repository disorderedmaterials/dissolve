// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "templates/orderedMap.h"
#include "base/serialiser.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(OrderedMapTest, BasicOrderedMap)
{
    SerialisedValue example;
    std::vector<std::string> keys = {"foo", "bar", "quux", "xyzzy"};
    std::vector<int> values = {2, 7, 5, 3};
    for (auto i = 0; i < keys.size(); i++)
        example[keys[i]] = values[i];

    auto index = 0;
    for (auto [k, v] : example.as_table())
    {
        EXPECT_EQ(keys[index], k);
        EXPECT_EQ(values[index], v.as_integer());
        index++;
    }
    EXPECT_EQ(index, keys.size());
}

} // namespace UnitTest
