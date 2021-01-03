// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "expression/value.h"
#include <fmt/core.h>

ExpressionValue::ExpressionValue()
{
    valueI_ = 0;
    valueD_ = 0.0;
    type_ = IntegerType;
    typeFixed_ = false;
}

ExpressionValue::ExpressionValue(int value, bool fixedType)
{
    valueI_ = value;
    valueD_ = 0.0;
    type_ = IntegerType;
    typeFixed_ = fixedType;
}

ExpressionValue::ExpressionValue(double value, bool fixedType)
{
    valueI_ = 0;
    valueD_ = value;
    type_ = DoubleType;
    typeFixed_ = fixedType;
}

ExpressionValue::~ExpressionValue() {}

ExpressionValue::ExpressionValue(const ExpressionValue &source)
{
    valueI_ = source.valueI_;
    valueD_ = source.valueD_;
    type_ = source.type_;
    typeFixed_ = source.typeFixed_;
}

void ExpressionValue::operator=(const ExpressionValue &source)
{
    if (typeFixed_)
    {
        if (type_ == IntegerType)
        {
            if (source.type_ == IntegerType)
                valueI_ = source.valueI_;
            else
                valueI_ = int(source.valueD_);
        }
        else
        {
            if (source.type_ == IntegerType)
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
}

/*
 * Data
 */

// Return the current result type
ExpressionValue::ValueType ExpressionValue::type() const { return type_; }

void ExpressionValue::operator=(int i)
{
    valueI_ = i;
    valueD_ = i;
    if (!typeFixed_)
        type_ = IntegerType;
}

void ExpressionValue::operator=(double d)
{
    valueI_ = int(d);
    valueD_ = d;
    if (!typeFixed_)
        type_ = DoubleType;
}

// Return result as integer (regardless of current type)
int ExpressionValue::asInteger() const { return (type_ == IntegerType ? valueI_ : int(valueD_)); }

// Return result as double (regardless of current type)
double ExpressionValue::asDouble() const { return (type_ == IntegerType ? double(valueI_) : valueD_); }

// Return result as a string
std::string ExpressionValue::asString() const
{
    return (type_ == IntegerType ? fmt::format("{}", valueI_) : fmt::format("{:12.6e}", valueD_));
}

// Return pointer to integer value
int *ExpressionValue::integerPointer() { return &valueI_; }

// Return pointer to double value
double *ExpressionValue::doublePointer() { return &valueD_; }

/*
 * Tests
 */

// Return whether the contained type is an integer
bool ExpressionValue::isInteger() const { return (type_ == IntegerType); }

// Return whether the contained type is an double
bool ExpressionValue::isDouble() const { return (type_ == DoubleType); }

// Return the supplied ExpressionValues both contain integer types
bool ExpressionValue::bothIntegers(const ExpressionValue &a, const ExpressionValue &b)
{
    return ((a.type_ == IntegerType) && (b.type_ == IntegerType));
}

// Return the supplied ExpressionValues both contain double types
bool ExpressionValue::bothDoubles(const ExpressionValue &a, const ExpressionValue &b)
{
    return ((a.type_ == DoubleType) && (b.type_ == DoubleType));
}
