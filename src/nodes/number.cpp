// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/number.h"
#include <string>

Number::Number(const std::variant<int, double> &value) : value_(value) {}

Number::Number(int value, std::optional<int> min, std::optional<int> max) : value_(value), min_(min), max_(max) {}

Number::Number(double value, std::optional<double> min, std::optional<double> max) : value_(value), min_(min), max_(max) {}

Number &Number::operator=(const Number &other)
{
    set(other);
    return *this;
}

Number Number::operator+(const Number &other) const
{
    return std::visit([](auto a, auto b) -> Number { return {a + b}; }, value_, other.value_);
}

Number &Number::operator+=(const Number &other)
{
    set(std::visit([](auto &a, auto b) -> Number { return a + b; }, value_, other.value_));
    return *this;
}

Number Number::operator-(const Number &other) const
{
    return std::visit([](auto a, auto b) -> Number { return {a - b}; }, value_, other.value_);
}

Number &Number::operator-=(const Number &other)
{
    set(std::visit([](auto &a, auto b) -> Number { return a - b; }, value_, other.value_));
    return *this;
}

Number Number::operator*(const Number &other) const
{
    return std::visit([](auto a, auto b) -> Number { return {a * b}; }, value_, other.value_);
}

Number &Number::operator*=(const Number &other)
{
    set(std::visit([](auto &a, auto b) -> Number { return a * b; }, value_, other.value_));
    return *this;
}

Number Number::operator/(const Number &other) const
{
    return std::visit([](auto a, auto b) -> Number { return {a / b}; }, value_, other.value_);
}

Number &Number::operator/=(const Number &other)
{
    set(std::visit([](auto &a, auto b) -> Number { return a / b; }, value_, other.value_));
    return *this;
}

bool Number::operator==(const Number &other) const { return value_ == other.value_; }

bool Number::operator!=(const Number &other) const { return value_ != other.value_; }

bool Number::operator<(const Number &other) const
{
    return std::visit([](auto a, auto b) -> bool { return a < b; }, value_, other.value_);
}

bool Number::operator<=(const Number &other) const
{
    return std::visit([](auto a, auto b) -> bool { return a <= b; }, value_, other.value_);
}

bool Number::operator>(const Number &other) const
{
    return std::visit([](auto a, auto b) -> bool { return a > b; }, value_, other.value_);
}

bool Number::operator>=(const Number &other) const
{
    return std::visit([](auto a, auto b) -> bool { return a >= b; }, value_, other.value_);
}

/*
 * Data
 */

// Impose limit on Number
Number::NumberVariant Number::limit(Number n) const
{
    if (hasLowerBound() && n < Number(min_.value()))
        return min_.value();

    if (hasUpperBound() && n > Number(max_.value()))
        return max_.value();

    return n.value_;
}

// Return whether the contained value is an integer
bool Number::isInteger(NumberVariant n) const { return std::holds_alternative<int>(n); }

// Return whether the contained value is a double
bool Number::isDouble(NumberVariant n) const { return std::holds_alternative<double>(n); }

// Return contained value as integer
int Number::asInteger(NumberVariant n) const
{
    if (isInteger(n))
        return std::get<int>(n);
    else
        return int(std::get<double>(n));
}

// Return contained value as double
double Number::asDouble(NumberVariant n) const
{
    if (isInteger(n))
        return double(std::get<int>(n));
    else
        return std::get<double>(n);
}

// Set from other Number
void Number::set(const Number &other) { value_ = limit(other.value_); }

// Return whether the number has lower bound
bool Number::hasLowerBound() const { return min_.has_value(); }

// Return whether the number has upper bound
bool Number::hasUpperBound() const { return max_.has_value(); }

// Return optional lower bound
std::optional<Number::NumberVariant> Number::min() const { return min_; }

// Return optional upper bound
std::optional<Number::NumberVariant> Number::max() const { return max_; }

// Return whether the number has bounds
bool Number::isBounded() const { return hasLowerBound() && hasUpperBound(); }

// Return whether the contained value is an integer
bool Number::isInteger() const { return isInteger(value_); }

// Return whether the contained value is a double
bool Number::isDouble() const { return isDouble(value_); }

// Return contained value as integer
int Number::asInteger() const { return asInteger(value_); }

// Return contained value as double
double Number::asDouble() const { return asDouble(value_); }

// Return value represented as a string
std::string Number::asString(bool addQuotesIfRequired) const
{
    return std::visit([](auto &&arg) -> std::string { return std::format("{}", arg); }, value_);
}

// Express as a serialisable value
void Number::serialise(std::string name, SerialisedValue &target) const
{
    if (std::holds_alternative<int>(value_))
        target[name] = std::get<int>(value_);
    else
        target[name] = std::get<double>(value_);
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
