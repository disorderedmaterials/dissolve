/*
    *** Expression Value
    *** src/expression/value.h
    Copyright T. Youngs 2015-2020

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

#include <string>

// Forward declarations
/* none */

// Expression Value
class ExpressionValue
{
    public:
    ExpressionValue();
    ExpressionValue(int value);
    ExpressionValue(double value);
    ~ExpressionValue();
    ExpressionValue(const ExpressionValue &source);
    void operator=(const ExpressionValue &source);

    /*
     * Data
     */
    public:
    // Value Type
    enum ValueType
    {
        IntegerType,
        DoubleType
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

    public:
    // Return the current result type
    ValueType type() const;
    void operator=(int i);
    void operator=(double d);
    // Return as integer (regardless of current type)
    int asInteger() const;
    // Return as double (regardless of current type)
    double asDouble() const;
    // Return result as a string
    std::string asString() const;
    // Return pointer to integer value
    int *integerPointer();
    // Return pointer to double value
    double *doublePointer();

    /*
     * Tests
     */
    public:
    // Return whether the contained type is an integer
    bool isInteger() const;
    // Return whether the contained type is an double
    bool isDouble() const;
    // Return the supplied ExpressionValues both contain integer types
    static bool bothIntegers(const ExpressionValue &a, const ExpressionValue &b);
    // Return the supplied ExpressionValues both contain double types
    static bool bothDoubles(const ExpressionValue &a, const ExpressionValue &b);
};
