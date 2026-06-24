// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "applicative.h"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace parsers
{

// A parser that expects an exact string
Parser<std::string_view> literal(std::string_view constant) { return Parser<std::string_view>(constant); }

Parser<std::string> takeWhile(std::function<bool(char)> f)
{
    Parser<std::string> result(
        [f](auto &input) -> parser_output<std::string>
        {
            if (input.eof())
                return {};
            std::string result = "";
            while (f(input.peek()))
                result.push_back(input.get());
            if (result.empty())
                return {};
            return {{result, input}};
        });
    return result;
}

// A parser that accepts and amount of whitespace
Parser<std::string> spaces()
{
    return takeWhile([](const char c) { return isspace(c); });
}
// A parser that accepts space and tab
Parser<std::string> inline_spaces()
{
    return takeWhile([](const char c) { return c == ' ' || c == '\t'; });
}
// A parse that accepts any amount of visible characters
Parser<std::string> graphs()
{
    return takeWhile([](const char c) { return std::isgraph(c); });
}
// A parse that accepts any amount of alphanumeric characters
Parser<std::string> alphanums()
{
    return takeWhile([](const char c) { return std::isalnum(c); });
}
// A parse that accepts any amount of letters
Parser<std::string> alphas()
{
    return takeWhile([](const char c) { return std::isalpha(c); });
}
// A parse that accepts any amount of upper case letters
Parser<std::string> uppers()
{
    return takeWhile([](const char c) { return std::isupper(c); });
}
// A parse that accepts any amount of lower case letters
Parser<std::string> lowers()
{
    return takeWhile([](const char c) { return std::islower(c); });
}
// A parse that accepts any amount of punctuation case letters
Parser<std::string> punctuations()
{
    return takeWhile([](const char c) { return std::ispunct(c); });
}
// A parser that accepts any amount of digit characters
Parser<std::string> digits()
{
    return takeWhile([](const char c) { return std::isdigit(c); });
}

// A parser that continues until a newline
Parser<std::string> inlines()
{
    return takeWhile([](const auto c) { return c != '\r' && c != '\n'; });
}

// A parser that matches newline characters
Parser<std::string_view> newlines() { return "\r\n"_p | "\n"_p; }

// A quick wrapper for easily making parses from strings
Parser<std::string_view> operator""_p(const char *text, size_t size) { return literal(std::string_view(text, size)); }
} // namespace parsers
