// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <string>

// Expression Value
class ExpressionValue : public Serialisable<>
{
    public:
    ExpressionValue();
    explicit ExpressionValue(int value);
    explicit ExpressionValue(double value);
    ExpressionValue(bool value);
    ~ExpressionValue() = default;
    ExpressionValue(const ExpressionValue &source);
    ExpressionValue &operator=(const ExpressionValue &source);
    ExpressionValue &operator=(int i);
    ExpressionValue &operator=(double d);
    ExpressionValue &operator=(bool b);

    /*
     * Data
     */
    public:
    // Value Type
    enum class ValueType
    {
        Integer,
        Double,
        Bool
    };

    private:
    // Current result type
    ValueType type_;
    // Whether current result type is fixed
    bool typeFixed_;
    // Integer value (if type_ == IntegerType)
    int valueI_;
    // Double value (if type_ == DoubleType)
    double valueD_;
    // Bool value (if type_ == BoolType)
    bool valueB_;

    public:
    // Return the current result type
    ValueType type() const;
    // Return as integer (regardless of current type)
    int asInteger() const;
    // Return as double (regardless of current type)
    double asDouble() const;
    // Return as bool (regardless of current type)
    bool asBool() const;
    // Return result as a string
    std::string asString() const;
    // Return pointer to integer value
    int *integerPointer();
    // Return pointer to double value
    double *doublePointer();
    // Return pointer to bool value
    bool *boolPointer();

    /*
     * Tests
     */
    public:
    // Return whether the contained type is an integer
    bool isInteger() const;
    // Return whether the contained type is a double
    bool isDouble() const;
    // Return whether the contained type is a bool
    bool isBool() const;
    // Return the supplied ExpressionValues both contain integer types
    static bool bothIntegers(const ExpressionValue &a, const ExpressionValue &b);
    // Return the supplied ExpressionValues both contain double types
    static bool bothDoubles(const ExpressionValue &a, const ExpressionValue &b);
    // Return the supplied ExpressionValues both contain bool types
    static bool bothBools(const ExpressionValue &a, const ExpressionValue &b);
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
