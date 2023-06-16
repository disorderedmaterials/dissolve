// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodevalue.h"
#include "base/sysfunc.h"
#include <string>

NodeValue::NodeValue()
{
    valueI_ = 0;
    valueD_ = 0.0;
    type_ = DoubleNodeValue;
}
NodeValue::NodeValue(const int i)
{
    valueI_ = i;
    valueD_ = 0.0;
    type_ = IntegerNodeValue;
}
NodeValue::NodeValue(const double d)
{
    valueI_ = 0;
    valueD_ = d;
    type_ = DoubleNodeValue;
}
NodeValue::NodeValue(std::string_view expressionText,
                     std::optional<std::vector<std::shared_ptr<ExpressionVariable>>> parameters)
{
    valueI_ = 0;
    valueD_ = 0.0;
    set(expressionText, parameters);
}

NodeValue::~NodeValue() = default;

// Assignment from integer
void NodeValue::operator=(const int value) { set(value); }

// Assignment from integer
void NodeValue::operator=(const double value) { set(value); }

// Conversion (to double)
NodeValue::operator double() { return asDouble(); }

/*
 * Data
 */

// Add local variable to expression
std::shared_ptr<ExpressionVariable> NodeValue::addLocalVariable(std::string_view name)
{
    return expression_.addLocalVariable(name);
}

// Set integer value
bool NodeValue::set(int value)
{
    valueI_ = value;
    type_ = IntegerNodeValue;

    return true;
}

// Set double value
bool NodeValue::set(double value)
{
    valueD_ = value;
    type_ = DoubleNodeValue;

    return true;
}

// Set from expression text
bool NodeValue::set(std::string_view expressionText, std::optional<std::vector<std::shared_ptr<ExpressionVariable>>> parameters)
{
    // Is this just a plain number, rather than an equation.
    bool isFloatingPoint;
    if (DissolveSys::isNumber(expressionText, isFloatingPoint))
    {
        type_ = isFloatingPoint ? DoubleNodeValue : IntegerNodeValue;
        if (isFloatingPoint)
            valueD_ = std::stod(std::string(expressionText));
        else
            valueI_ = std::stoi(std::string(expressionText));
    }
    else
    {
        // Parse the supplied expression
        type_ = ExpressionNodeValue;
        return expression_.create(expressionText, parameters);
    }

    return true;
}

// Return whether value is currently valid
bool NodeValue::isValid() const { return (type_ == ExpressionNodeValue ? expression_.isValid() : true); }

/*
 * Value Retrieval
 */

// Return contained value as integer
int NodeValue::asInteger() const
{
    if (type_ == IntegerNodeValue)
        return valueI_;
    else if (type_ == DoubleNodeValue)
        return (int)valueD_;
    else
        return expression_.asInteger();
}

// Return contained value as double
double NodeValue::asDouble() const
{
    if (type_ == IntegerNodeValue)
        return (double)valueI_;
    else if (type_ == DoubleNodeValue)
        return valueD_;
    else
        return expression_.asDouble();
}

// Return value represented as a string
std::string NodeValue::asString(bool addQuotesIfRequired) const
{
    if (type_ == IntegerNodeValue)
        return fmt::format("{}", valueI_);
    else if (type_ == DoubleNodeValue)
        return fmt::format("{}", valueD_);
    else
    {
        if (addQuotesIfRequired)
            return fmt::format("'{}'", expression_.expressionString());
        else
            return fmt::format("{}", expression_.expressionString());
    }
}
