// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include <gtest/gtest.h>
#include <toml11/toml.hpp>
#include "templates/ordered_map.h"

namespace UnitTest
{

using SerialisedValue = toml::basic_value<toml::discard_comments, dissolve::ordered_map, std::vector>;
TEST(OrderedMapTest, BasicOrderedMap)
{
  SerialisedValue example;
  std::vector<std::string> keys = {"foo", "bar", "quux", "xyzzy"};
  std::vector<int> values = {2, 7, 5, 3};
  for (int i = 0; i < keys.size(); i++)
    example[keys[i]] = values[i];

  int index = 0;
  for (auto [k, v] : example.as_table()) {
    EXPECT_EQ(keys[index], k);
    EXPECT_EQ(values[index], v.as_integer());
    index++;
  }
  EXPECT_EQ(index, keys.size());


}

} // namespace UnitTest
