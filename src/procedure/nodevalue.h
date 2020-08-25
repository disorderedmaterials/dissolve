/*
    *** Node Value
    *** src/procedure/nodevalue.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "expression/expression.h"
#include "templates/reflist.h"

// Forward Declarations
class Configuration;
class LineParser;

// Node Value
class NodeValue
{
    public:
    NodeValue();
    NodeValue(const int i);
    NodeValue(const double d);
    NodeValue(std::string_view expressionText, RefList<ExpressionVariable> parameters);
    ~NodeValue();
    void operator=(const int value);
    void operator=(const double value);
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
    bool set(std::string_view expressionText, RefList<ExpressionVariable> parameters);
    // Return whether value is currently valid
    bool isValid() const;

    /*
     * Value Retrieval
     */
    public:
    // Return contained value as integer
    int asInteger();
    // Return contained value as double
    double asDouble();
    // Return value represented as a string
    std::string asString(bool addQuotesIfRequired = false) const;
};
