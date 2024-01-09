// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
