// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <optional>
#include <variant>

// Node Number
class Number
{
    public:
    using NumberVariant = std::variant<int, double>;
    Number(const NumberVariant &value = {0});
    Number(int value, std::optional<int> min = {}, std::optional<int> max = {});
    Number(double value, std::optional<double> min = {}, std::optional<double> max = {});
    ~Number() = default;
    Number &operator=(const Number &other);
    Number operator+(const Number &other) const;
    Number &operator+=(const Number &other);
    Number operator-(const Number &other) const;
    Number &operator-=(const Number &other);
    Number operator*(const Number &other) const;
    Number &operator*=(const Number &other);
    Number operator/(const Number &other) const;
    Number &operator/=(const Number &other);
    Number &operator++();
    Number &operator--();
    bool operator==(const Number &value) const;
    bool operator!=(const Number &value) const;
    bool operator<(const Number &other) const;
    bool operator<=(const Number &other) const;
    bool operator>(const Number &other) const;
    bool operator>=(const Number &other) const;

    /*
     * Data
     */
    protected:
    // Value
    NumberVariant value_;
    // Lower bound
    std::optional<NumberVariant> min_;
    // Upper bound
    std::optional<NumberVariant> max_;

    private:
    // Impose limit on Number
    Number::NumberVariant limit(Number n) const;
    // Return whether the contained value is an integer
    bool isInteger(NumberVariant n) const;
    // Return whether the contained value is a double
    bool isDouble(NumberVariant n) const;
    // Return contained value as integer
    int asInteger(NumberVariant n) const;
    // Return contained value as double
    double asDouble(NumberVariant n) const;

    public:
    // Set from other Number
    void set(const Number &other);
    // Return whether the number has lower bound
    bool hasLowerBound() const;
    // Return whether the number has upper bound
    bool hasUpperBound() const;
    // Return whether the number has bounds
    bool isBounded() const;
    // Return optional min
    std::optional<Number::NumberVariant> min() const;
    // Return optional max
    std::optional<Number::NumberVariant> max() const;
    // Return whether the contained value is an integer
    bool isInteger() const;
    // Return whether the contained value is a double
    bool isDouble() const;
    // Return contained value as integer
    int asInteger() const;
    // Return contained value as double
    double asDouble() const;
    // Return value represented as a string
    std::string asString(bool addQuotesIfRequired = false) const;

    /*
     * Serialisable
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
};
