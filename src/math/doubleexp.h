/*
    *** Double with Exponent class
    *** src/math/doubleexp.h
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

#include <string>

// Mantissa/exponent class
class DoubleExp
{
    public:
    DoubleExp();
    DoubleExp(double value);
    DoubleExp(double mantissa, int exponent);
    // Conversion (double)
    operator double();

    /*
     * Operators
     */
    public:
    void operator=(double d);
    bool operator==(const double other);
    bool operator!=(const double other);

    /*
     * Data
     */
    private:
    // Mantissa
    double mantissa_;
    // Exponent
    int exponent_;
    // Value
    double value_;

    /*
     * Functions
     */
    private:
    // Recalculate value from stored mantissa and exponent
    void recalculate();

    public:
    // Retrieve full, real value
    double value() const;
    // Set mantissa and exponent
    void set(double mantissa, int exponent);
    // Set from normal value
    void set(double value);
    // Set mantissa
    void setMantissa(double mantissa);
    // Return mantissa
    double mantissa() const;
    // Set exponent
    void setExponent(int exponent);
    // Return exponent
    int exponent() const;
    // Return value as string
    std::string asString(const int exponentThreshold = 3, const int maxDecimals = 4) const;
};
