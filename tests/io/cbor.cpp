// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/cbor.h"
#include <filesystem>
#include <gtest/gtest.h>

namespace UnitTest
{
class CBORTest : public ::testing::Test
{
    protected:
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
        auto cbor = CBOR::to(node);
        std::ranges::subrange sub{cbor.begin(), cbor.end()};
        auto copy = CBOR::from(sub);

        compare_toml("", node, copy);
    }
};

TEST_F(CBORTest, BasicTypes)
{
    // Check Positive Int
    basic_test(37);
    // Check Negative Int
    basic_test(-917);
    // Check Boolean True
    basic_test(true);
    // Check Boolean False
    basic_test(false);
    // Check Float
    basic_test(3.14159);
    // Check String
    basic_test("test string");
}

TEST_F(CBORTest, BasicArray)
{
    // Check Array
    toml::array array{12, 4.56, "in an array"};
    SerialisedValue value = array;
    ASSERT_EQ(value.type(), toml::value_t::array);
    ASSERT_EQ(value.as_array().size(), 3);
    basic_test(value);
}

TEST_F(CBORTest, BasicTable)
{
    // Check Array
    SerialisedValue value, inner;
    inner["foo"] = "bar";
    inner["baz"] = "quux";

    value["first"] = 12;
    value["second"] = 4.56;
    value["third"] = "in a map";
    value["last"] = inner;

    basic_test(value);
}

TEST_F(CBORTest, ComplexFile)
{
    SerialisedValue value = toml::parse("dissolve/input/simple_addition_graph.toml");
    basic_test(value);
}

TEST_F(CBORTest, CBORCompare)
{
    SerialisedValue tomlFile = toml::parse("dissolve/input/simple_addition_graph.toml");
    std::ifstream infile("dissolve/input/simple_addition_graph.cbor", std::ios::binary | std::ios::in);
    SerialisedValue cborFile = CBOR::from(std::move(infile));
    compare_toml("", tomlFile, cborFile);
}

TEST_F(CBORTest, CBORExtractSimple)
{
    std::ifstream infile("dissolve/input/simple_addition_graph.cbor", std::ios::binary | std::ios::in);
    auto type = CBOR::extract(std::move(infile), {"type"});
    EXPECT_TRUE(type.is_string());
    EXPECT_EQ(type.as_string(), "Dissolve");
    infile.close();
}

TEST_F(CBORTest, CBORExtractComplex)
{
    std::ifstream infile("dissolve/input/simple_addition_graph.cbor", std::ios::binary | std::ios::in);
    auto type = CBOR::extract(std::move(infile), {"edges", 1, "sourceNode"});
    EXPECT_TRUE(type.is_string());
    EXPECT_EQ(type.as_string(), "y");
    infile.close();
}
} // namespace UnitTest
