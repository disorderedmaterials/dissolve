/*
	*** Number Format
	*** src/gui/render/numberformat.cpp
	Copyright T. Youngs 2013-2020

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

#include "gui/render/numberformat.h"
#include <math/doubleexp.h>
#include <stdio.h>
#include <QString>

// FormatType Keywords
const char* FormatTypeKeywords[] = { "Decimal", "Integer", "Scientific" };

// Convert text string to FormatType
NumberFormat::FormatType NumberFormat::formatType(QString s)
{
	for (int n=0; n<NumberFormat::nNumberFormats; ++n) if (s == FormatTypeKeywords[n]) return (NumberFormat::FormatType) n;
	return NumberFormat::nNumberFormats;
}

// Convert FormatType to text string
const char* NumberFormat::formatType(NumberFormat::FormatType ft)
{
	return FormatTypeKeywords[ft];
}

// Constructor
NumberFormat::NumberFormat()
{
	type_ = NumberFormat::DecimalFormat;
	nDecimals_ = 1;
	forcePrecedingPlus_ = false;
	useUpperCaseExponent_ = true;
	useENotation_ = true;
}

// Destructor
NumberFormat::~NumberFormat()
{
}

/*
 * Definition
 */

// Set ndex type
void NumberFormat::setType(NumberFormat::FormatType type)
{
	type_ = type;
}

// Return X index type
NumberFormat::FormatType NumberFormat::type()
{
	return type_;
}

// Set number of decimals to use
void NumberFormat::setNDecimals(int n)
{
	nDecimals_ = n;
}

// Return number of decimals to use
int NumberFormat::nDecimals()
{
	return nDecimals_;
}

// Set whether to force display of preceding '+' as well as '-'
void NumberFormat::setForcePrecedingPlus(bool b)
{
	forcePrecedingPlus_ = b;
}

// Return whether to force display of preceding '+' as well as '-'
bool NumberFormat::forcePrecedingPlus()
{
	return forcePrecedingPlus_;
}

// Set whether or not use use uppercase exponent character (for ScientificFormat)
void NumberFormat::setUseUpperCaseExponent(bool b)
{
	useUpperCaseExponent_ = b;
}

// Return whether or not use use uppercase exponent character (for ScientificFormat)
bool NumberFormat::useUpperCaseExponent()
{
	return useUpperCaseExponent_;
}

// Set whether to use 'E' notation in preference to 'x10' notation
void NumberFormat::setUseENotation(bool b)
{
	useENotation_ = b;
}

// Return whether to use 'E' notation in preference to 'x10' notation
bool NumberFormat::useENotation()
{
	return useENotation_;
}

/*
 * Number Conversion
 */

// Return number formatted according to internal definition
QString NumberFormat::format(double number)
{
	QString result;

	DoubleExp x(number);

	// Add '+' onto string if requested and number is positive
	if (forcePrecedingPlus_ && (number >= 0.0)) result = "+";

	// Construct rest of string
	switch (type_)
	{
		case (NumberFormat::IntegerFormat):
			result += QString::number(int(number));
			break;
		case (NumberFormat::DecimalFormat):
			result += QString::number(number, 'f', nDecimals_);
			break;
		case (NumberFormat::ScientificFormat):
			if (!useENotation_) result += QString::number(x.mantissa(), 'f', nDecimals_) + QChar(0x00D7)+"10\\sup{" + QString::number(x.exponent()) + "}";
			else if (useUpperCaseExponent_) result += QString::number(x.mantissa(), 'f', nDecimals_) + "E" + QString::number(x.exponent());
			else result += QString::number(x.mantissa(), 'f', nDecimals_) + "e" + QString::number(x.exponent());
			break;
		default:
			result += QString::number(number);
			break;
	}

	return result;
}
