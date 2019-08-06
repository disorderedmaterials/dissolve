/*
	*** Expression Value
	*** src/expression/value.cpp
	Copyright T. Youngs 2010-2019

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

#include "expression/value.h"
#include "base/charstring.h"

// Constructors
ExpressionValue::ExpressionValue()
{
	valueI_ = 0;
	valueD_ = 0.0;
	type_ = IntegerType;
}

ExpressionValue::ExpressionValue(int value)
{
	valueI_ = value;
	valueD_ = 0.0;
	type_ = IntegerType;
}

ExpressionValue::ExpressionValue(double value)
{
	valueI_ = 0;
	valueD_ = value;
	type_ = DoubleType;
}

// Destructor
ExpressionValue::~ExpressionValue()
{
}

/*
 * Data
 */

// Assignment operator (integer)
void ExpressionValue::operator=(int i)
{
	valueI_ = i;
	type_ = IntegerType;
}

// Assignment operator (double)
void ExpressionValue::operator=(double d)
{
	valueD_ = d;
	type_ = DoubleType;
}

// Return result as integer (regardless of current type)
int ExpressionValue::asInteger() const
{
	return (type_ == IntegerType ? valueI_ : int(valueD_));
}

// Return result as double (regardless of current type)
double ExpressionValue::asDouble() const
{
	return (type_ == IntegerType ? double(valueI_) : valueD_);
}

// Return result as a string
const char* ExpressionValue::asString() const
{
	static CharString result;

	if (type_ == IntegerType) result.sprintf("%i",valueI_);
	else result.sprintf("%12.6e", valueD_);

	return result.get();
}

// Return pointer to integer value
int* ExpressionValue::integerPointer()
{
	return &valueI_;
}

// Return pointer to double value
double* ExpressionValue::doublePointer()
{
	return &valueD_;
}

/*
 * Tests
 */

// Return whether the contained type is an integer
bool ExpressionValue::isInteger() const
{
	return (type_ == IntegerType);
}

// Return whether the contained type is an double
bool ExpressionValue::isDouble() const
{
	return (type_ == DoubleType);
}

// Return the supplied ExpressionValues both contain integer types
bool ExpressionValue::bothIntegers(const ExpressionValue& a, const ExpressionValue& b)
{
	return ((a.type_ == IntegerType) && (b.type_ == IntegerType));
}

// Return the supplied ExpressionValues both contain double types
bool ExpressionValue::bothDoubles(const ExpressionValue& a, const ExpressionValue& b)
{
	return ((a.type_ == DoubleType) && (b.type_ == DoubleType));
}
