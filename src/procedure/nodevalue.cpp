/*
	*** Node Value
	*** src/procedure/nodevalue.cpp
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

#include "procedure/nodevalue.h"
#include "base/sysfunc.h"

// Constructor
NodeValue::NodeValue()
{
	valueI_ = 0;
	valueD_ = 0.0;
	type_ = DoubleNodeValue;
}

// Destructor
NodeValue::~NodeValue()
{
}

// Assignment from integer
void NodeValue::operator=(const int value)
{
	set(value);
}

// Assignment from integer
void NodeValue::operator=(const double value)
{
	set(value);
}

// Conversion (to double)
NodeValue::operator double()
{
	return asDouble();
}

/*
 * Data
 */

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

// Set from expression test
bool NodeValue::set(const char* expressionText, RefList<ExpressionVariable,bool> parameters)
{
	type_ = ExpressionNodeValue;
	return expression_.generate(expressionText, parameters);
}

/*
 * Value Retrieval
 */

// Return contained value as integer
int NodeValue::asInteger()
{
	if (type_ == IntegerNodeValue) return valueI_;
	else if (type_ == DoubleNodeValue) return (int) valueD_;
	else
	{
		bool success;
		return (int) expression_.execute(success);
	}
}

// Return contained value as double
double NodeValue::asDouble()
{
	if (type_ == IntegerNodeValue) return (double) valueI_;
	else if (type_ == DoubleNodeValue) return valueD_;
	else
	{
		bool success;
		return expression_.execute(success);
	}
}

// Return value represented as a string
const char* NodeValue::asString()
{
	static CharString result;

	if (type_ == IntegerNodeValue) result = DissolveSys::itoa(valueI_);
	else if (type_ == DoubleNodeValue) result = DissolveSys::ftoa(valueD_, "%12.6e");
	else 
	{
		// TODO
		Messenger::error("Expression saving not implemented yet!\n");
		result.clear();
	}

	return result.get();
}
