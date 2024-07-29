// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/nodeValue.h"
#include "base/sysFunc.h"
#include <string>

NodeValue::NodeValue(const int i) : type_(NodeValue::IntegerNodeValue), valueI_(i) {}
NodeValue::NodeValue(const double d) : valueD_(d) {}
NodeValue::NodeValue(std::string_view expressionText,
                     std::optional<std::vector<std::shared_ptr<ExpressionVariable>>> parameters)
    : type_(NodeValue::ExpressionNodeValue)
{
    set(expressionText, std::move(parameters));
}

// Assignment from integer
NodeValue &NodeValue::operator=(const int value)
{
    set(value);
    return *this;
}

// Assignment from integer
NodeValue &NodeValue::operator=(const double value)
{
    set(value);
    return *this;
}

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

// Express as a serialisable value
SerialisedValue NodeValue::serialise() const
{
    switch (type_)
    {
        case IntegerNodeValue:
            return valueI_;
        case DoubleNodeValue:
            return valueD_;
        case ExpressionNodeValue:
            return expression_.expressionString();
        default:
            throw(std::runtime_error("Unhandled NodeValue type in serialise().\n"));
    }
}

// Read values from a serialisable value
void NodeValue::deserialise(const SerialisedValue &node, std::vector<std::shared_ptr<ExpressionVariable>> params)
{
    toml::visit(
        [this, &params](auto &arg)
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
            else if constexpr (std::is_same_v<T, toml::string>)
            {
                set(std::string(arg), params);
            }
        },
        node);
}

bool NodeValue::operator==(const NodeValue &value) const
{
    if (type_ != value.type_)
        return false;
    switch (type_)
    {
        case IntegerNodeValue:
            return valueI_ == value.asInteger();
        case DoubleNodeValue:
            return valueD_ == value.asDouble();
        case ExpressionNodeValue:
            return expression_.expressionString() == value.expression_.expressionString();
    }
    return false;
}

bool NodeValue::operator!=(const NodeValue &value) const { return !(*this == value); }

/*
 * Node Value Proxy
 */

NodeValueProxy::NodeValueProxy(const int i)
{
    valueI_ = i;
    valueD_ = 0.0;
    type_ = IntegerNodeValue;
}
NodeValueProxy::NodeValueProxy(const double d)
{
    valueI_ = 0;
    valueD_ = d;
    type_ = DoubleNodeValue;
}
NodeValueProxy::NodeValueProxy(std::string_view expressionText)
{
    valueI_ = 0;
    valueD_ = 0.0;
    type_ = ExpressionNodeValue;
    expressionString_ = expressionText;
}

// Return value represented as a string
std::string NodeValueProxy::asString(bool addQuotesIfRequired) const
{
    if (type_ == IntegerNodeValue)
        return fmt::format("{}", valueI_);
    else if (type_ == DoubleNodeValue)
        return fmt::format("{}", valueD_);
    else
    {
        if (addQuotesIfRequired)
            return fmt::format("'{}'", expressionString_);
        else
            return fmt::format("{}", expressionString_);
    }
}
