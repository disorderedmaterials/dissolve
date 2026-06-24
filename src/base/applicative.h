// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace parsers
{

// A concept to check if a type is a tuple.  This is built in in
// C++23, be we can do this for now.
template <typename T>
concept TupleLike = requires { std::tuple_size<T>::value; };

// The simplest defintion of an applicative parser is a function that
// takes a stream and, if the parse succeeds, returns the parsed
// value and the rest of the stream.  To make life simpler, we
// define the parser_output<T> for the return type of the function.
// This *could* have further implications because there are more
// complicated parsers we could create.
template <typename T> using parser_output = std::optional<std::tuple<T, std::istream &>>;

// This concept type checks that a given lambda matches the definition
// given in the paragraph above.
template <typename Lambda, typename T>
concept ApParse = requires(Lambda lam, std::istream input) {
    { lam(input) } -> std::convertible_to<parser_output<T>>;
};

// It's fully possible to just use the functions as the parsers, but,
// if we wrap them in a struct, we can use operator overloading to
// more easily combine smaller parsers into larger parsers.
template <typename T> class Parser
{

    public:
    // Wrap a parser lambda in a structure
    template <typename Lambda>
        requires ApParse<Lambda, T>
    Parser(Lambda lambda) : lambda_(lambda)
    {
    }

    // Create a parser that matches and exact string
    template <typename = std::enable_if<std::is_same<T, std::string_view>::value>>
    Parser(std::string_view constant)
        : lambda_(
              [constant](std::istream &input) -> parser_output<std::string_view>
              {
                  for (auto c : constant)
                      if (c != input.get())
                          return {};
                  return {{constant, input}};
              })
    {
    }

    private:
    // The actual function that we have wrapped.  We need to wrap this
    // in a std::function instead of using a lambda so that we don't
    // have to handle the exact type of the bound lambda
    std::function<parser_output<T>(std::istream &)> lambda_;

    public:
    // Parse a string and, if possible, return the value and the remainder
    parser_output<T> parse(std::istream &input) const { return lambda_(input); };
    // Parse a string and, if possible, return the value and the remainder
    parser_output<T> operator()(std::istream &input) const { return lambda_(input); }
    // Parse a string and enforce that it parsed the entire input
    std::optional<T> exact(std::istream &input) const
    {
        auto result = lambda_(input);
        if (result && input.get() == -1)
            return std::get<0>(*result);
        return {};
    }

    // Create a new parser that takes the output of the old parser and
    // passes it through a function
    template <typename Lambda> auto map(Lambda f) -> Parser<decltype(f(std::declval<T>()))>
    {
        auto &method = lambda_;
        Parser<decltype(f(std::declval<T>()))> result(
            [method, f](std::istream &input) -> parser_output<decltype(f(std::declval<T>()))>
            {
                auto first = method(input);
                if (first)
                {
                    auto &[body, remainder] = *first;
                    return {{f(body), remainder}};
                }
                else
                    return {};
            });
        return result;
    }

    // Same as map, but unpacks the tuple and passes the individual
    // parts as arguments.  This greatly simplifies creating mapping
    // functions
    template <typename Lambda>
        requires(TupleLike<T>)
    auto apply(Lambda f) -> Parser<decltype(std::apply(f, std::declval<T>()))>
    {
        return map([f](const T tup) { return std::apply(f, tup); });
    }

    // Insist that this parse is followed by another parse, but we
    // ignore the output of that other parser
    template <typename U> Parser<T> operator<<(Parser<U> other)
    {
        auto &method = lambda_;
        Parser<T> result(
            [method, other](std::istream &input) -> parser_output<T>
            {
                auto first = method(input);
                if (first)
                {
                    auto &[body, middle] = *first;
                    auto second = other(middle);
                    if (second)
                        return {{body, std::get<1>(*second)}};
                    else
                        return {};
                }
                else
                {
                    return {};
                }
            });
        return result;
    }

    // Confirm that this parser passes, but ignore its output and return the value of a subsequent parser
    template <typename U> Parser<U> operator>>(Parser<U> other)
    {
        auto &method = lambda_;
        Parser<U> result(
            [method, other](std::istream &input) -> parser_output<U>
            {
                auto first = method(input);
                if (first)
                {
                    auto &[body, middle] = *first;
                    return other(middle);
                }
                else
                    return {};
            });
        return result;
    }

    // If this parser fails, try an alternate parser instead of
    // immediately failing.
    Parser<T> operator|(Parser<T> other)
    {
        auto &method = lambda_;
        Parser<T> result(
            [method, other](std::istream &input) -> parser_output<T>
            {
                auto location = input.tellg();
                auto first = method(input);
                if (first)
                    return first;
                input.clear();
                input.seekg(location);
                return other(input);
            });
        return result;
    }

    // If this parser fails, try an alternate literal string parser
    // instead of immediately failing.
    template <typename = std::enable_if<std::is_same<T, std::string_view>::value>>
    Parser<std::string_view> operator|(std::string_view other)
    {
        return Parser<std::string_view>(other) | *this;
    }

    // After confirming that this parser passes, apply a second parser
    // on the remainder and collect both values.
    template <typename U>
        requires(!TupleLike<T> && !TupleLike<U>)
    auto operator&(Parser<U> other) -> Parser<std::tuple<T, U>>
    {
        auto &method = lambda_;
        Parser<std::tuple<T, U>> result(
            [method, other](std::istream &input) -> parser_output<std::tuple<T, U>>
            {
                auto first = method(input);
                if (first)
                {
                    auto &[fst, middle] = *first;
                    auto second = other(middle);
                    if (second)
                    {
                        auto &[snd, final] = *second;
                        return {{{fst, snd}, final}};
                    }
                    else
                        return {};
                }
                else
                    return {};
            });
        return result;
    }

    // After confirming that this parser passes, apply a second parser
    // on the remainder and collect both values.
    template <typename U>
        requires(TupleLike<T> && !TupleLike<U>)
    auto operator&(Parser<U> other) -> Parser<decltype(std::tuple_cat(std::declval<T>(), std::make_tuple(std::declval<U>())))>
    {
        auto &method = lambda_;
        Parser<decltype(std::tuple_cat(std::declval<T>(), std::make_tuple(std::declval<U>())))> result(
            [method, other](std::istream &input)
                -> parser_output<decltype(std::tuple_cat(std::declval<T>(), std::make_tuple(std::declval<U>())))>
            {
                auto first = method(input);
                if (first)
                {
                    auto &[fst, middle] = *first;
                    auto second = other(middle);
                    if (second)
                    {
                        auto &[snd, final] = *second;
                        return {{std::tuple_cat(fst, std::make_tuple(snd)), final}};
                    }
                    else
                        return {};
                }
                else
                    return {};
            });
        return result;
    }

    // After confirming that this parser passes, apply a second parser
    // on the remainder and collect both values.
    template <typename U>
        requires(!TupleLike<T> && TupleLike<U>)
    auto operator&(Parser<U> other) -> Parser<decltype(std::tuple_cat(std::make_tuple(std::declval<T>()), std::declval<U>()))>
    {
        auto &method = lambda_;
        Parser<decltype(std::tuple_cat(std::make_tuple(std::declval<T>()), std::declval<U>()))> result(
            [method, other](std::istream &input)
                -> parser_output<decltype(std::tuple_cat(std::make_tuple(std::declval<T>()), std::declval<U>()))>
            {
                auto first = method(input);
                if (first)
                {
                    auto &[fst, middle] = *first;
                    auto second = other(middle);
                    if (second)
                    {
                        auto &[snd, final] = *second;
                        return {{std::tuple_cat(std::make_tuple(fst), snd), final}};
                    }
                    else
                        return {};
                }
                else
                    return {};
            });
        return result;
    }

    // After confirming that this parser passes, apply a second parser
    // on the remainder and collect both values.
    template <typename U>
        requires(TupleLike<T> && TupleLike<U>)
    auto operator&(Parser<U> other) -> Parser<decltype(std::tuple_cat(std::declval<T>(), std::declval<U>()))>
    {
        auto &method = lambda_;
        Parser<decltype(std::tuple_cat(std::declval<T>(), std::declval<U>()))> result(
            [method,
             other](std::istream &input) -> parser_output<decltype(std::tuple_cat(std::declval<T>(), std::declval<U>()))>
            {
                auto first = method(input);
                if (first)
                {
                    auto &[fst, middle] = *first;
                    auto second = other(middle);
                    if (second)
                    {
                        auto &[snd, final] = *second;
                        return {{std::tuple_cat(fst, snd), final}};
                    }
                    else
                        return {};
                }
                else
                    return {};
            });
        return result;
    }
};

// Create a parser that always succeeds and returns a constant value
template <typename T> Parser<T> pure(T constant)
{
    Parser<T> result([constant](std::istream &input) -> parser_output<int> { return {{constant, input}}; });
    return result;
}

// A parser that always fails
template <typename T> Parser<T> null()
{
    Parser<T> result = ([](const auto x) -> parser_output<T> { return std::nullopt; });
    return result;
}

// Modify a parser so that the parsed value is wrapped in a
// std::optional.  If the parser would have failed, act as though
// the parse succeeded, but make the std::optional empty.
template <typename T> Parser<std::optional<T>> maybe(Parser<T> inner)
{
    Parser<std::optional<T>> result(
        [inner](std::istream &input) -> parser_output<std::optional<T>>
        {
            auto location = input.tellg();
            auto first = inner(input);
            if (first)
            {
                auto &[body, remainder] = *first;
                return {{{body}, remainder}};
            }
            else
            {
                input.clear();
                input.seekg(location);
                return {{{}, input}};
            }
        });
    return result;
}

// Insist that a parser passes at least once, but collect as many
// parsed values as possible.
template <typename T> Parser<std::vector<T>> some(Parser<T> inner)
{
    Parser<std::vector<T>> result(
        [inner](std::istream &input) -> parser_output<std::vector<T>>
        {
            std::vector<T> collection;
            auto location = input.tellg();
            while (!input.eof())
            {
                auto trial = inner(input);
                if (trial)
                {
                    auto &[body, _] = *trial;
                    collection.push_back(body);
                    location = input.tellg();
                }
                else
                {
                    input.clear();
                    input.seekg(location);
                    break;
                }
            }
            if (collection.empty())
                return {};
            return {{collection, input}};
        });
    return result;
}

// A parser that expects an exact string
Parser<std::string_view> literal(std::string_view constant);
// A parser that accepts any amount of digit characters
Parser<std::string> digits();

// A parser that accepts any amount of whitespace
Parser<std::string> spaces();
// A parser that accepts space and tab
Parser<std::string> inline_spaces();

// A parser that accepts any amount of visible characters
Parser<std::string> graphs();
// A parser that accepts any amount of alphanumeric characters
Parser<std::string> alphanums();
// A parser that accepts any amount of letters
Parser<std::string> alphas();
// A parser that accepts any amount of upper case letters
Parser<std::string> uppers();
// A parser that accepts any amount of lower case letters
Parser<std::string> lowers();
// A parser that accepts any amount of punctuation case letters
Parser<std::string> punctuations();
// A parser that continues until a newline
Parser<std::string> inlines();
// A parser that matches newline characters
Parser<std::string_view> newlines();

// A quick wrapper for easily making parses from strings
Parser<std::string_view> operator""_p(const char *text, size_t size);

// A quick overload to easily ignore strings around a parser
template <typename T> Parser<T> operator<<(Parser<T> body, std::string_view other)
{
    return body << Parser<std::string_view>(other);
}

// A quick overload to easily ignore strings around a parser
template <typename T> Parser<T> operator>>(std::string_view other, Parser<T> body)
{
    return Parser<std::string_view>(other) >> body;
}

// A quick overload to easily require strings around a parser
template <typename T> auto operator&(Parser<T> self, std::string_view other) -> decltype(self & Parser<std::string_view>(other))
{
    return self & Parser<std::string_view>(other);
}

// A quick overload to easily require strings around a parser
template <typename T> auto operator&(std::string_view other, Parser<T> self) -> decltype(Parser<std::string_view>(other) & self)
{
    return Parser<std::string_view>(other) & self;
}

}; // namespace parsers
