// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"

// Node Number
class NodeNumber : public Serialisable<>
{
    public:
    using NumberVariant = std::variant<int, double>;
    NodeNumber(const NumberVariant &value = {0});
    NodeNumber(int value);
    NodeNumber(double value);
    ~NodeNumber() = default;
    NodeNumber &operator=(const NodeNumber &other);
    NodeNumber operator+(const NodeNumber &other) const;
    NodeNumber &operator+=(const NodeNumber &other);
    NodeNumber operator-(const NodeNumber &other) const;
    NodeNumber &operator-=(const NodeNumber &other);
    NodeNumber operator*(const NodeNumber &other) const;
    NodeNumber &operator*=(const NodeNumber &other);
    NodeNumber operator/(const NodeNumber &other) const;
    NodeNumber &operator/=(const NodeNumber &other);
    NodeNumber &operator++();
    NodeNumber &operator--();
    bool operator==(const NodeNumber &value) const;
    bool operator!=(const NodeNumber &value) const;

    /*
     * Data
     */
    protected:
    // Value
    NumberVariant value_;

    public:
    // Set from other NodeNumber
    void set(const NodeNumber &other);
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
