// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/applicative.h"
#include "base/parserLibrary.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string_view>

namespace UnitTest
{

using namespace Parsers;
using namespace std::literals;

template <typename T> void test_parser(std::string_view input, Parser<T> parser, std::optional<T> expected)
{
    std::cout << "Base string:\t" << input << std::endl;
    std::istringstream stream{std::string(input)};
    auto result = parser(stream);
    if (!result)
        EXPECT_FALSE(expected);
    else
        EXPECT_EQ(std::get<0>(*result), *expected);
}
template <typename T> void test_exact(std::string_view input, Parser<T> parser, T expected)
{
    std::cout << "Base string:\t" << input << std::endl;
    std::istringstream stream{std::string(input)};
    auto result = parser.exact(stream);
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, expected);
}

TEST(ApplicativeTest, BasicStrings)
{
    test_parser("Foo", "Fo"_p, {"Fo"});
    test_parser("Foobar", "Foo"_p, {"Foo"});
}
TEST(ApplicativeTest, Ignoring)
{
    test_exact("Foobar", "Foo"_p << "bar"_p, "Foo"sv);
    test_exact("Foobar", "Foo" >> "bar"_p, "bar"sv);
}
TEST(ApplicativeTest, Joining)
{
    test_exact("Foobar", "Foo"_p & "bar", {"Foo", "bar"});
    test_parser("Foobar", (pure(1) & pure(2)) & (pure(3) & pure(4)), {{1, 2, 3, 4}});
    test_exact("Foobar", ("Fo"_p & "ob") & ("a" & "r"_p), {"Fo", "ob", "a", "r"});
    test_exact("Foobar", ("Fo"_p & "ob") & "ar", {"Fo", "ob", "ar"});
    test_exact("Foobar", "Fo" & ("ob"_p & "ar"), {"Fo", "ob", "ar"});
    test_parser("Foobar", (pure(1) & pure(2)) & pure(3), {{1, 2, 3}});
    test_parser("Foobar", pure(1) & (pure(2) & pure(3)), {{1, 2, 3}});
}

TEST(ApplicativeTest, Choices)
{
    test_exact("Foo", "Foo"_p | "Bar", "Foo"sv);
    test_exact("Bar", "Foo"_p | "Bar", "Bar"sv);
    test_parser("Quux", "Foo"_p | "Bar", {});
}

TEST(ApplicativeTest, NaturalNumbers)
{
    test_parser("123foo", natural(), {123});
    auto triplet = natural() & "," >> natural() & "," >> natural();
    test_exact("123,456,789", triplet, {123, 456, 789});
    auto vecsum = triplet.apply([](const auto x, const auto y, const auto z) -> int { return x + y + z; });
    test_exact("123,456,789", vecsum, 123 + 456 + 789);
}

TEST(ApplicativeTest, Optionals)
{
    test_exact("123,456", maybe(natural() << ",") & natural(), {{123}, 456});
    test_exact("456", maybe(natural() << ",") & natural(), {std::nullopt, 456});
    test_exact("-456", integer(), -456);
    test_exact("456", integer(), 456);
}

TEST(ApplicativeTest, MultipleTerms)
{
    // Parse multiple terms
    test_exact("123, 456,   789,012", some(integer() << maybe(","_p << maybe(spaces()))), {123, 456, 789, 12});
    // Completely fail the parse if no copies are present
    test_parser("789", some(integer() << ","), {});
}

TEST(ApplicativeTest, RealNumbers)
{
    test_exact("-12.0543", real(), -12.0543);
    test_exact("1.02E-3", real(), 1.02e-3);
    test_exact("-3E-4", real(), -3e-4);
    test_exact("-71.2e3", real(), -71.2e3);
}

TEST(ApplicativeTest, BasicParser)
{
    test_parser("  \t foo", spaces(), {"  \t "});
    test_exact("HW", alphas(), "HW"s);
    test_parser("1qaz.QAZ foo", graphs(), {"1qaz.QAZ"});
    test_parser("1qaz.QAZ foo", digits() & lowers() & punctuations() & uppers() & spaces(), {{"1", "qaz", ".", "QAZ", " "}});

    test_exact("\"Foo\"", "\"" >> alphas() << "\"", "Foo"s);
}

TEST(ApplicativeTest, Vector) { test_exact("1 2.5 -3e-1", vector3(), Vector3(1, 2.5, -3e-1)); }

TEST(ApplicativeTest, StructureAtom)
{
    test_exact("HW 1 2.5 -3e-4 5.6", structureAtom(), {"HW", Vector3(1, 2.5, -3e-4), 5.6});
    test_exact("He 0.5 0.5 0.5", structureAtom(), {"He", Vector3(0.5, 0.5, 0.5), {}});
}

TEST(ApplicativeTest, XYZStructure)
{
    std::ifstream infile{"xyz/c2so3.xyz"};
    ASSERT_TRUE(infile);
    auto xyz =
        (maybe(spaces()) >> natural() << spaces() & inlines() >> newlines() >> some(structureAtom() << maybe(newlines())))
            .parse(infile);
    // auto xyz = (maybe(spaces()) >> natural() ).parse(oss.view());

    ASSERT_TRUE(xyz);
    auto &[value, rest] = *xyz;
    EXPECT_EQ(rest.get(), -1);
    auto &terms = std::get<1>(value);
    EXPECT_EQ(terms.size(), std::get<0>(value));

    std::vector<std::tuple<std::string_view, Vector3>> expected{

        {"S", {0.010001, 0.000000, -0.000012}},   {"O", {1.465001, 0.000000, -0.000012}},
        {"O", {-0.475688, -1.371543, -0.000012}}, {"O", {-0.475688, 0.685772, 1.187779}},
        {"C", {-0.588181, 0.844607, -1.462914}},  {"C", {-0.079239, 0.124469, -2.712002}},
        {"H", {-1.678181, 0.844607, -1.462914}},  {"H", {-0.225364, 1.872451, -1.463546}},
        {"H", {-0.442057, -0.903375, -2.712634}}, {"H", {-0.443088, 0.638209, -3.601825}},
        {"H", {1.000760, 0.124469, -2.713254}},

    };
    auto index = 0;
    for (auto &[elem, r] : expected)
    {
        EXPECT_EQ(elem, std::get<0>(terms[index]));
        EXPECT_EQ(r, std::get<1>(terms[index]));
        ++index;
    }
}

TEST(ApplicativeTest, Helium)
{
    std::ifstream infile{"xyz/voxelDensity-helium.xyz"};
    ASSERT_TRUE(infile);
    auto xyz =
        (maybe(spaces()) >> natural() << spaces() & inlines() >> newlines() >> some(structureAtom() << maybe(newlines())))
            .parse(infile);
    ASSERT_TRUE(xyz);
    auto &[value, rest] = *xyz;
    EXPECT_EQ(rest.get(), -1);
    auto &terms = std::get<1>(value);
    EXPECT_EQ(terms.size(), std::get<0>(value));
    int index = 0;
    for (auto &[elem, r, q] : terms)
        EXPECT_EQ(elem, "He");
}

} // namespace UnitTest
