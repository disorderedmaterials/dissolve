// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/number.h"
#include <string>

Number::Number(const std::variant<int, double> &value) : value_(value) {}

Number::Number(int value) : value_(value) {}

Number::Number(double value) : value_(value) {}

Number &Number::operator=(const Number &other)
{
    value_ = other.value_;
    return *this;
}

Number Number::operator+(const Number &other) const
{
    return std::visit([](auto a, auto b) -> Number { return {a + b}; }, value_, other.value_);
}

Number &Number::operator+=(const Number &other)
{
    std::visit([](auto &a, auto b) { a += b; }, value_, other.value_);
    return *this;
}

Number Number::operator-(const Number &other) const
{
    return std::visit([](auto a, auto b) -> Number { return {a - b}; }, value_, other.value_);
}

Number &Number::operator-=(const Number &other)
{
    std::visit([](auto &a, auto b) { a -= b; }, value_, other.value_);
    return *this;
}

Number Number::operator*(const Number &other) const
{
    return std::visit([](auto a, auto b) -> Number { return {a * b}; }, value_, other.value_);
}

Number &Number::operator*=(const Number &other)
{
    std::visit([](auto &a, auto b) { a *= b; }, value_, other.value_);
    return *this;
}

Number Number::operator/(const Number &other) const
{
    return std::visit([](auto a, auto b) -> Number { return {a / b}; }, value_, other.value_);
}

Number &Number::operator/=(const Number &other)
{
    std::visit([](auto &a, auto b) { a /= b; }, value_, other.value_);
    return *this;
}

bool Number::operator==(const Number &other) const { return value_ == other.value_; }

bool Number::operator!=(const Number &other) const { return value_ != other.value_; }

/*
 * Data
 */

// Set from other Number
void Number::set(const Number &other) { value_ = other.value_; }

// Return whether the contained value is an integer
bool Number::isInteger() const { return std::holds_alternative<int>(value_); }

// Return whether the contained value is a double
bool Number::isDouble() const { return std::holds_alternative<double>(value_); }

// Return contained value as integer
int Number::asInteger() const
{
    if (std::holds_alternative<int>(value_))
        return std::get<int>(value_);
    else
        return int(std::get<double>(value_));
}

// Return contained value as double
double Number::asDouble() const
{
    if (std::holds_alternative<int>(value_))
        return double(std::get<int>(value_));
    else
        return std::get<double>(value_);
}

// Return value represented as a string
std::string Number::asString(bool addQuotesIfRequired) const
{
    return std::visit([](auto &&arg) -> std::string { return std::format("{}", arg); }, value_);
}

// Express as a serialisable value
SerialisedValue Number::serialise() const
{
    if (std::holds_alternative<int>(value_))
        return std::get<int>(value_);
    else
        return std::get<double>(value_);
}

// Read values from a serialisable value
void Number::deserialise(const SerialisedValue &node)
{
    toml::visit(
        [this](auto &arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, toml::integer>)
            {
                set((int)arg);
            }
            else if constexpr (std::is_same_v<T, toml::floating>)
            {
                set((double)arg);
            }
        },
        node);
}
