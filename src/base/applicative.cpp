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

// A parser that accepts any amount of digit characters
Parser<std::string_view> inlines()
{
    return takeWhile([](const auto c) { return c != '\r' && c != '\n'; });
}

// A parser that accepts any amount of digit characters
Parser<std::string_view> newlines() { return "\r\n"_p | "\n"_p; }

// A quick wrapper for easily making parses from strings
Parser<std::string_view> operator""_p(const char *text, size_t size) { return literal(std::string_view(text, size)); }
} // namespace parsers
