// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <variant>

// Node Number
class Number : public Serialisable<>
{
    public:
    using NumberVariant = std::variant<int, double>;
    Number(const NumberVariant &value = {0});
    Number(int value);
    Number(double value);
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

    /*
     * Data
     */
    protected:
    // Value
    NumberVariant value_;

    public:
    // Set from other Number
    void set(const Number &other);
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
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
