/*
	*** Double with Exponent value
	*** src/math/doubleexp.cpp
	Copyright T. Youngs 2012-2019

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
#include <math.h>
#include <stdio.h>
#include <limits>
#include <string.h>

// Constructors
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
void DoubleExp::operator=(double d)
{
	set(d);
}

// Equality Operator
bool DoubleExp::operator==(const double other)
{
	return value_ == other;
}

// Inequality Operator
bool DoubleExp::operator!=(const double other)
{
	return value_ != other;
}

// Conversion operators
DoubleExp::operator double()
{
	return value();
}

/*
 * Functions
 */

// Recalculate value
void DoubleExp::recalculate()
{
	value_ = mantissa_ * pow(10.0,exponent_);
}

// Retrieve full, real value
double DoubleExp::value() const
{
	return value_;
}

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
	exponent_ = floor(log10(fabs(value)+std::numeric_limits<double>::min()));
	if (exponent_ == floor(log10(std::numeric_limits<double>::min()))) exponent_ = 0;
	mantissa_ = value / pow(10.0,exponent_);
	recalculate();
// 	printf("  -- Input value %f gives mantissa of %f and exponent of %i, giving value of %e\n", value, mantissa_, exponent_, value_);
}

// Set from supplied text
void DoubleExp::set(const char* text)
{
	// Copy the string
	static char s[128];
	strcpy(s, text);

// 	printf("DoubleExp::set(const char*) - Original string is '%s'\n", text);
	// Use strtok to get first part of string, before any exponent
	char* mant = strtok(s, "Ee");
	mantissa_ = atof(mant);
// 	printf("DoubleExp::set(const char*) - Mantissa is %f (%s)\n", mantissa_, mant);

	// Call strtok a second time to see if we have an exponent
	char* expo = strtok(NULL, "Ee");
	if (expo) exponent_ = atoi(expo);
	else exponent_ = 0;
// 	printf("DoubleExp::set(const char*) - Exponent is %i (%s)\n", exponent_, expo ? expo : "NULL");

	recalculate();
}

// Set mantissa
void DoubleExp::setMantissa(double mantissa)
{
	mantissa_ = mantissa;
	recalculate();
}

// Return mantissa
double DoubleExp::mantissa() const
{
	return mantissa_;
}

// Set exponent alone
void DoubleExp::setExponent(int exponent)
{
	exponent_ = exponent;
	recalculate();
}

// Return exponent
int DoubleExp::exponent() const
{
	return exponent_;
}

// Return value as string
CharString DoubleExp::asString(const int exponentThreshold, const int maxDecimals) const
{
	/*
	 * Check the absolute value against the provided threshold, and decide whether to use scientific or normal formatting.
	 *
	 * If using scientific notation, print the mantissa to a formatted string with maxDecimals. Otherwise, print the 
	 * actual value to a formatted string.
	 *
	 * Strip any unnecessary trailing zeroes from the formatted string (we always keep one after the decimal point, if present).
	 * 
	 * Then, return the final formatted string, adding the exponent on if using scientificNotation.
	 */

	bool scientificNotation = abs(exponent_) > exponentThreshold;
	char formatString[32];
	sprintf(formatString, "%%.%if", maxDecimals);

	// Print the mantissa or full value to a formatted string, and strip any trailing zeroes
	CharString mantissaString(formatString, scientificNotation ? mantissa_ : value_);
	int dot = mantissaString.find('.');
	if (dot != -1)
	{
		int nZeroesAtEnd = 0;
		int nDecimals = 0;

		// Start the search at [dot+2], skipping the dot and the first char after it - we will always allow one lone zero after the decimal point
		for (int n=dot+2; n<mantissaString.length(); ++n)
		{
			// If this character is '0', increase our counter
			// If anything else, reset the counter
			if (mantissaString[n] == '0') ++nZeroesAtEnd;
			else nZeroesAtEnd = 0;

			// Increase the number of decimal places
			++nDecimals;
		}
		mantissaString.eraseEnd(nZeroesAtEnd);
	}

	/*
	 * If using normal notation, the mantissaString is our result.
	 * If not, add on the exponent (unless it is zero).
	 */
	if ((!scientificNotation) || (exponent_ == 0)) return mantissaString;
	else return CharString("%sE%i", mantissaString.get(), exponent_);
}
