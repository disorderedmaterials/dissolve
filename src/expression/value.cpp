// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "expression/value.h"
#include <fmt/format.h>

ExpressionValue::ExpressionValue()
{
    valueI_ = 0;
    valueD_ = 0.0;
    type_ = ValueType::Integer;
    typeFixed_ = false;
}

ExpressionValue::ExpressionValue(int value)
{
    valueI_ = value;
    valueD_ = 0.0;
    type_ = ValueType::Integer;
    typeFixed_ = false;
}

ExpressionValue::ExpressionValue(double value)
{
    valueI_ = 0;
    valueD_ = value;
    type_ = ValueType::Double;
    typeFixed_ = false;
}

ExpressionValue::ExpressionValue(const ExpressionValue &source)
{
    valueI_ = source.valueI_;
    valueD_ = source.valueD_;
    type_ = source.type_;
    typeFixed_ = source.typeFixed_;
}

ExpressionValue &ExpressionValue::operator=(const ExpressionValue &source)
{
    if (typeFixed_)
    {
        if (type_ == ValueType::Integer)
        {
            if (source.type_ == ValueType::Integer)
                valueI_ = source.valueI_;
            else
                valueI_ = int(source.valueD_);
        }
        else
        {
            if (source.type_ == ValueType::Integer)
                valueD_ = source.valueI_;
            else
                valueD_ = source.valueD_;
        }
    }
    else
    {
        // Take on the source type
        valueI_ = source.valueI_;
        valueD_ = source.valueD_;
        type_ = source.type_;
    }

    return *this;
}

ExpressionValue &ExpressionValue::operator=(int i)
{
    valueI_ = i;
    valueD_ = i;
    if (!typeFixed_)
        type_ = ValueType::Integer;

    return *this;
}

ExpressionValue &ExpressionValue::operator=(double d)
{
    valueI_ = int(d);
    valueD_ = d;
    if (!typeFixed_)
        type_ = ValueType::Double;

    return *this;
}

/*
 * Data
 */

// Return the current result type
ExpressionValue::ValueType ExpressionValue::type() const { return type_; }

// Return result as integer (regardless of current type)
int ExpressionValue::asInteger() const { return (type_ == ValueType::Integer ? valueI_ : int(valueD_)); }

// Return result as double (regardless of current type)
double ExpressionValue::asDouble() const { return (type_ == ValueType::Integer ? double(valueI_) : valueD_); }

// Return result as a string
std::string ExpressionValue::asString() const
{
    return (type_ == ValueType::Integer ? fmt::format("{}", valueI_) : fmt::format("{}", valueD_));
}

// Return pointer to integer value
int *ExpressionValue::integerPointer() { return &valueI_; }

// Return pointer to double value
double *ExpressionValue::doublePointer() { return &valueD_; }

/*
 * Tests
 */

// Return whether the contained type is an integer
bool ExpressionValue::isInteger() const { return (type_ == ValueType::Integer); }

// Return whether the contained type is a double
bool ExpressionValue::isDouble() const { return (type_ == ValueType::Double); }

// Return the supplied ExpressionValues both contain integer types
bool ExpressionValue::bothIntegers(const ExpressionValue &a, const ExpressionValue &b)
{
    return ((a.type_ == ValueType::Integer) && (b.type_ == ValueType::Integer));
}

// Return the supplied ExpressionValues both contain double types
bool ExpressionValue::bothDoubles(const ExpressionValue &a, const ExpressionValue &b)
{
    return ((a.type_ == ValueType::Double) && (b.type_ == ValueType::Double));
}

// Express as a serialisable value
SerialisedValue ExpressionValue::serialise() const
{
    if (type_ == ValueType::Integer)
        return valueI_;
    else
        return valueD_;
}

// Read values from a serialisable value
void ExpressionValue::deserialise(const SerialisedValue &node)
{
    if (node.is_integer())
    {
        type_ = ValueType::Integer;
        valueI_ = node.as_integer();
    }
    else
    {
        type_ = ValueType::Double;
        valueD_ = node.as_floating();
    }
}
