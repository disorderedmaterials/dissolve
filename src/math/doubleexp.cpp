/*
    *** Double with Exponent value
    *** src/math/doubleexp.cpp
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

#include "math/doubleexp.h"
#include "base/sysfunc.h"
#include <fmt/core.h>
#include <limits>
#include <math.h>

DoubleExp::DoubleExp()
{
    mantissa_ = 0.0;
    exponent_ = 0.0;
    recalculate();
}

DoubleExp::DoubleExp(double value)
{
    set(value);
    recalculate();
}

DoubleExp::DoubleExp(double mantissa, int exponent)
{
    // Private variables
    mantissa_ = mantissa;
    exponent_ = exponent;
    recalculate();
}

// Assignment from single value
void DoubleExp::operator=(double d) { set(d); }

bool DoubleExp::operator==(const double other) { return value_ == other; }

bool DoubleExp::operator!=(const double other) { return value_ != other; }

DoubleExp::operator double() { return value(); }

/*
 * Functions
 */

// Recalculate value
void DoubleExp::recalculate() { value_ = mantissa_ * pow(10.0, exponent_); }

// Retrieve full, real value
double DoubleExp::value() const { return value_; }

// Set mantissa and exponent
void DoubleExp::set(double mantissa, int exponent)
{
    mantissa_ = mantissa;
    exponent_ = exponent;
    recalculate();
}

// Set from normal value
void DoubleExp::set(double value)
{
    exponent_ = floor(log10(fabs(value) + std::numeric_limits<double>::min()));
    if (exponent_ == floor(log10(std::numeric_limits<double>::min())))
        exponent_ = 0;
    mantissa_ = value / pow(10.0, exponent_);

    if (mantissa_ >= 10.0)
    {
        mantissa_ /= 10.0;
        ++exponent_;
    }

    recalculate();
}

// Set mantissa
void DoubleExp::setMantissa(double mantissa)
{
    mantissa_ = mantissa;
    recalculate();
}

// Return mantissa
double DoubleExp::mantissa() const { return mantissa_; }

// Set exponent alone
void DoubleExp::setExponent(int exponent)
{
    exponent_ = exponent;
    recalculate();
}

// Return exponent
int DoubleExp::exponent() const { return exponent_; }

// Return value as string
std::string DoubleExp::asString(const int exponentThreshold, const int maxDecimals) const
{
    /*
     * Check the absolute value against the provided threshold, and decide whether to use scientific or normal formatting.
     *
     * If using scientific notation, print the mantissa to a formatted string with maxDecimals. Otherwise, print the
     * actual value to a formatted string.
     *
     * Strip any unnecessary trailing zeroes from the formatted string (we always keep one after the decimal point, if
     * present).
     *
     * Then, return the final formatted string, adding the exponent on if using scientificNotation.
     */

    auto scientificNotation = abs(exponent_) > exponentThreshold;

    // Print the mantissa or full value to a formatted string, and strip any trailing zeroes
    std::string mantissaString = fmt::format("{:.{}f}", scientificNotation ? mantissa_ : value_, maxDecimals);
    auto dot = mantissaString.find('.');
    if (dot != std::string::npos)
    {
        auto nZeroesAtEnd = 0;
        auto nDecimals = 0;

        // Start the search at [dot+2], skipping the dot and the first char after it - we will always allow one lone
        // zero after the decimal point
        for (auto n = dot + 2; n < mantissaString.length(); ++n)
        {
            // If this character is '0', increase our counter
            // If anything else, reset the counter
            if (mantissaString[n] == '0')
                ++nZeroesAtEnd;
            else
                nZeroesAtEnd = 0;

            // Increase the number of decimal places
            ++nDecimals;
        }
        mantissaString.erase(mantissaString.length() - nZeroesAtEnd);
    }

    /*
     * If using normal notation, the mantissaString is our result.
     * If not, add on the exponent (unless it is zero).
     */
    if ((!scientificNotation) || (exponent_ == 0))
        return mantissaString;
    else
        return fmt::format("{}E{}", mantissaString, exponent_);
}
