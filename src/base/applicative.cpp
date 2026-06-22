// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "applicative.h"
#include <algorithm>
#include <cctype>
#include <cmath>

namespace parsers
{

// A parser that expects an exact string
Parser<std::string_view> literal(std::string_view constant) { return Parser<std::string_view>(constant); }

Parser<std::string_view> takeWhile(std::function<bool(char)> f)
{
    Parser<std::string_view> result(
        [f](const auto input) -> parser_output<std::string_view>
        {
            if (input.empty())
                return {};
            auto it = std::find_if(input.begin(), input.end(), [f](const auto x) { return !f(x); });
            if (it == input.begin())
                return {};
            return {{std::string_view(input.begin(), it), std::string_view(it, input.end())}};
        });
    return result;
}

// A parser that accepts an integer greater than or equal to zero
Parser<int> natural()
{
    auto result = digits();
    return result.map(
        [](const auto terms) -> int
        {
            int total = 0;
            for (auto term : terms)
                total = 10 * total + (term - '0');
            return total;
        });
}

// Take an optional minus sign and a whole number to create an integer
int nat2int(std::optional<std::string_view> minus, int number)
{
    if (minus)
        return -number;
    return number;
}

// A parser that accepts an integer
Parser<int> integer() { return (maybe("-"_p) & natural()).apply(nat2int); }

// Take the numbers before the decimal, some optional digits after the period, and an optional exponent, and return a double.
double nat2dbl(int before, std::optional<std::string_view> after, std::optional<int> exponent)
{
    double result = before;
    if (after)
    {
        double magnitude = 1;
        double total = 0;
        for (auto digit : *after)
        {
            total = total * 10 + (digit - '0');
            magnitude *= 10;
        }
        if (result < 0)
            total *= -1;
        result += total / magnitude;
    }
    if (exponent)
    {
        result *= pow(10, *exponent);
    }
    return result;
}

// A parser that accepts a real, floating point number
Parser<double> real()
{
    auto result = integer() & maybe("." >> digits()) & maybe(("e"_p | "E"_p) >> integer());
    return result.apply(nat2dbl);
}

// A parser that accepts and amount of whitespace
Parser<std::string_view> spaces() { return takeWhile(std::isspace); }
// A parse that accepts any amount of visible characters
Parser<std::string_view> graphs() { return takeWhile(std::isgraph); }
// A parse that accepts any amount of alphanumeric characters
Parser<std::string_view> alphanums() { return takeWhile(std::isalnum); }
// A parse that accepts any amount of letters
Parser<std::string_view> alphas() { return takeWhile(std::isalpha); }
// A parse that accepts any amount of upper case letters
Parser<std::string_view> uppers() { return takeWhile(std::isupper); }
// A parse that accepts any amount of lower case letters
Parser<std::string_view> lowers() { return takeWhile(std::islower); }
// A parse that accepts any amount of punctuation case letters
Parser<std::string_view> punctuations() { return takeWhile(std::ispunct); }
// A parser that accepts any amount of digit characters
Parser<std::string_view> digits() { return takeWhile(std::isdigit); }

// A quick wrapper for easily making parses from strings
Parser<std::string_view> operator""_p(const char *text, size_t size) { return literal(std::string_view(text, size)); }
} // namespace parsers
