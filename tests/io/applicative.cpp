// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/applicative.h"
#include "base/parserLibrary.h"
#include <gtest/gtest.h>
#include <string_view>

namespace UnitTest
{

using namespace parsers;
using namespace std::literals;

template <typename T> void test_parser(std::string_view input, Parser<T> parser, parser_output<T> expected)
{
    auto result = parser(input);
    EXPECT_EQ(result, expected);
}
template <typename T> void test_exact(std::string_view input, Parser<T> parser, T expected)
{
    auto result = parser.exact(input);
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, expected);
}

TEST(ApplicativeTest, BasicStrings)
{
    test_parser("Foo", "Fo"_p, {{"Fo", "o"}});
    test_parser("Foobar", "Foo"_p, {{"Foo", "bar"}});
}
TEST(ApplicativeTest, Ignoring)
{
    test_exact("Foobar", "Foo"_p << "bar"_p, "Foo"sv);
    test_exact("Foobar", "Foo" >> "bar"_p, "bar"sv);
}
TEST(ApplicativeTest, Joining)
{
    test_exact("Foobar", "Foo"_p & "bar", {"Foo", "bar"});
    test_parser("Foobar", (pure(1) & pure(2)) & (pure(3) & pure(4)), {{{1, 2, 3, 4}, "Foobar"}});
    test_exact("Foobar", ("Fo"_p & "ob") & ("a" & "r"_p), {"Fo", "ob", "a", "r"});
    test_exact("Foobar", ("Fo"_p & "ob") & "ar", {"Fo", "ob", "ar"});
    test_exact("Foobar", "Fo" & ("ob"_p & "ar"), {"Fo", "ob", "ar"});
    test_parser("Foobar", (pure(1) & pure(2)) & pure(3), {{{1, 2, 3}, "Foobar"}});
    test_parser("Foobar", pure(1) & (pure(2) & pure(3)), {{{1, 2, 3}, "Foobar"}});
}

TEST(ApplicativeTest, Choices)
{
    test_exact("Foo", "Foo"_p | "Bar", "Foo"sv);
    test_exact("Bar", "Foo"_p | "Bar", "Bar"sv);
    test_parser("Quux", "Foo"_p | "Bar", {});
}

TEST(ApplicativeTest, NaturalNumbers)
{
    test_parser("123foo", natural(), {{123, "foo"}});
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
    test_parser("  \t foo", spaces(), {{"  \t ", "foo"}});
    test_parser("1qaz.QAZ foo", graphs(), {{"1qaz.QAZ", " foo"}});
    test_parser("1qaz.QAZ foo", digits() & lowers() & punctuations() & uppers() & spaces(),
                {{{"1", "qaz", ".", "QAZ", " "}, "foo"}});

    test_exact("\"Foo\"", "\"" >> alphas() << "\"", "Foo"sv);
}

TEST(ApplicativeTest, Vector) { test_exact("1 2.5 -3e-1", vector3(), Vector3(1, 2.5, -3e-1)); }

TEST(ApplicativeTest, StructureAtom) { test_exact("HW 1 2.5 -3e-4 5", structureAtom(), {"HW", Vector3(1, 2.5, -3e-4), 5}); }

} // namespace UnitTest
