// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/applicative.h"
#include <gtest/gtest.h>

namespace UnitTest
{

using namespace parsers;

template <typename T> void test_parser(std::string_view input, Parser<T> parser, parser_output<T> expected)
{
    auto result = parser(input);
    ASSERT_EQ(result, expected);
}
TEST(ApplicativeTest, BasicStrings)
{
    test_parser("Foo", "Fo"_p, {{"Fo", "o"}});
    test_parser("Foobar", "Foo"_p, {{"Foo", "bar"}});
}
TEST(ApplicativeTest, Ignoring)
{
    test_parser("Foobar", "Foo"_p << "bar"_p, {{"Foo", ""}});
    test_parser("Foobar", "Foo" >> "bar"_p, {{"bar", ""}});
}
TEST(ApplicativeTest, Joining)
{
    test_parser("Foobar", "Foo"_p & "bar", {{{"Foo", "bar"}, ""}});
    test_parser("Foobar", (pure(1) & pure(2)) & (pure(3) & pure(4)), {{{1, 2, 3, 4}, "Foobar"}});
    test_parser("Foobar", ("Fo"_p & "ob") & ("a" & "r"_p), {{{"Fo", "ob", "a", "r"}, ""}});
    test_parser("Foobar", ("Fo"_p & "ob") & "ar", {{{"Fo", "ob", "ar"}, ""}});
    test_parser("Foobar", "Fo" & ("ob"_p & "ar"), {{{"Fo", "ob", "ar"}, ""}});
    test_parser("Foobar", (pure(1) & pure(2)) & pure(3), {{{1, 2, 3}, "Foobar"}});
    test_parser("Foobar", pure(1) & (pure(2) & pure(3)), {{{1, 2, 3}, "Foobar"}});
}

TEST(ApplicativeTest, Choices)
{
    test_parser("Foo", "Foo"_p | "Bar", {{"Foo", ""}});
    test_parser("Bar", "Foo"_p | "Bar", {{"Bar", ""}});
    test_parser("Quux", "Foo"_p | "Bar", {});
}

TEST(ApplicativeTest, NaturalNumbers)
{
    test_parser("123foo", natural(), {{123, "foo"}});
    auto triplet = natural() & "," >> natural() & "," >> natural();
    test_parser("123,456,789", triplet, {{{123, 456, 789}, ""}});
    auto vecsum = triplet.apply([](const auto x, const auto y, const auto z) -> int { return x + y + z; });
    test_parser("123,456,789", vecsum, {{123 + 456 + 789, ""}});
}

TEST(ApplicativeTest, Optionals)
{
    test_parser("123,456", maybe(natural() << ",") & natural(), {{{{123}, 456}, ""}});
    test_parser("456", maybe(natural() << ",") & natural(), {{{std::nullopt, 456}, ""}});
    test_parser("-456", integer(), {{-456, ""}});
    test_parser("456", integer(), {{456, ""}});
}

TEST(ApplicativeTest, MultipleTerms)
{
    // Parse multiple terms
    test_parser("123, 456,   789,012", some(integer() << maybe(","_p << maybe(spaces()))), {{{123, 456, 789, 12}, ""}});
    // Completely fail the parse if no copies are present
    test_parser("789", some(integer() << ","), {});
}

TEST(ApplicativeTest, RealNumbers)
{
    test_parser("-12.0543", real(), {{-12.0543, ""}});
    test_parser("1.02E-3", real(), {{1.02e-3, ""}});
    test_parser("-71.2e3", real(), {{-71.2e3, ""}});
}

TEST(ApplicativeTest, BasicParser)
{
    test_parser("  \t foo", spaces(), {{"  \t ", "foo"}});
    test_parser("1qaz.QAZ foo", graphs(), {{"1qaz.QAZ", " foo"}});
    test_parser("1qaz.QAZ foo", digits() & lowers() & punctuations() & uppers() & spaces(),
                {{{"1", "qaz", ".", "QAZ", " "}, "foo"}});

    test_parser("\"Foo\"", "\"" >> alphas() << "\"", {{"Foo", ""}});
}
} // namespace UnitTest
