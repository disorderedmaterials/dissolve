// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "base/cbor.h"
#include <filesystem>
#include <gtest/gtest.h>

namespace UnitTest
{
void compare_toml(std::string location, SerialisedValue toml, SerialisedValue toml2)
{
    if (toml.is_table())
    {
        ASSERT_TRUE(toml2.is_table()) << location;
        for (auto &[k, v] : toml.as_table())
        {
            ASSERT_TRUE(toml2.contains(k)) << location << "." << k << std::endl << "Expected:" << std::endl << toml[k];
            compare_toml(std::format("{}.{}", location, k), v, toml2.at(k));
        }
    }
    else if (toml.is_array())
    {
        auto arr = toml.as_array();
        auto arr2 = toml2.as_array();
        ASSERT_EQ(arr.size(), arr2.size()) << location << std::endl << "Expected" << std::endl << toml;
        for (int i = 0; i < arr.size(); ++i)
            compare_toml(std::format("{}[{}]", location, i), arr[i], arr2[i]);
    }
    else
    {
        EXPECT_EQ(toml, toml2) << location;
    }
}

void basic_test(SerialisedValue node)
{
    auto cbor = toCBOR(node);
    std::ranges::subrange sub{cbor.begin(), cbor.end()};
    auto [copy, remainder] = fromCBOR(sub);

    EXPECT_EQ(remainder.begin(), remainder.end());
    UnitTest::compare_toml("", node, copy);
}

} // namespace UnitTest

TEST(CBORTest, BasicInt)
{
    // Check Positive Int
    UnitTest::basic_test(37);
    // Check Negative Int
    UnitTest::basic_test(-917);
}
