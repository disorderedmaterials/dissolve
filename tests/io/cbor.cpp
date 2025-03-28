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

} // namespace UnitTest

TEST(CBORTest, BasicInt)
{
    int baseline = 37;
    SerialisedValue node = baseline;

    auto cbor = toCBOR(node);
    auto copy = fromCBOR(cbor);

    UnitTest::compare_toml("", node, copy);

    // Check negative numbers
    node = -917;
    copy = fromCBOR(toCBOR(node));
    UnitTest::compare_toml("", node, copy);
}
