// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "expression/expression.h"
#include "templates/optionalref.h"

// Node Value
class NodeValue : public Serialisable
{
    public:
    NodeValue();
    NodeValue(const int i);
    NodeValue(const double d);
    NodeValue(std::string_view expressionText,
              std::optional<std::vector<std::shared_ptr<ExpressionVariable>>> parameters = std::nullopt);
    ~NodeValue();
    void operator=(const int value);
    void operator=(const double value);
    bool operator==(const NodeValue &value) const;
    bool operator!=(const NodeValue &value) const;
    operator double();

    /*
     * Data
     */
    private:
    // Value Types
    enum NodeValueType
    {
        IntegerNodeValue,
        DoubleNodeValue,
        ExpressionNodeValue
    };
    // Type of contained data
    NodeValueType type_;
    // Integer value, if defined
    int valueI_;
    // Double value, if defined
    double valueD_;
    // Expression, if defined
    Expression expression_;

    public:
    // Set integer value
    bool set(int value);
    // Set double value
    bool set(double value);
    // Set from expression text
    bool set(std::string_view expressionText,
             std::optional<std::vector<std::shared_ptr<ExpressionVariable>>> parameters = std::nullopt);
    // Return whether value is currently valid
    bool isValid() const;

    /*
     * Value Retrieval
     */
    public:
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
    // Express as a tree node
    SerialisedValue serialise() const override;
    // Read values from a tree node
    void deserialise(const SerialisedValue &node) override;
};
