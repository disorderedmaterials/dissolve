/*
	*** VariableValue
	*** src/base/variablevalue.cpp
	Copyright T. Youngs 2012-2016

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/variablevalue.h"
#include "base/processpool.h"
#include "base/sysfunc.h"

// Constructor
VariableValue::VariableValue()
{
	type_ = VariableValue::IntegerType;
	valueI_ = 0;
}

// Constructor (bool)
VariableValue::VariableValue(bool value)
{
	type_ = VariableValue::BooleanType;
	valueB_ = value;
}

// Constructor (int)
VariableValue::VariableValue(int value)
{
	type_ = VariableValue::IntegerType;
	valueI_ = value;
}

// Constructor (double)
VariableValue::VariableValue(double value)
{
	type_ = VariableValue::DoubleType;
	valueD_ = value;
}

// Constructor (int)
VariableValue::VariableValue(const char* value)
{
	type_ = VariableValue::CharType;
	valueC_ = value;
}

// Constructor (Array<int>)
VariableValue::VariableValue(const Array<int>& value)
{
	type_ = VariableValue::IntegerArrayType;
	arrayI_ = value;
}

// Constructor (Array<double>)
VariableValue::VariableValue(const Array<double>& value)
{
	type_ = VariableValue::DoubleArrayType;
	arrayD_ = value;
}

// Constructor (VariableValue)
VariableValue::VariableValue(const VariableValue& value)
{
	(*this) = value;
}

// Assignment operator
void VariableValue::operator=(const VariableValue& value)
{
	type_ = value.type_;
	valueB_ = value.valueB_;
	valueI_ = value.valueI_;
	valueD_ = value.valueD_;
	valueC_ = value.valueC_;
	arrayI_ = value.arrayI_;
	arrayD_ = value.arrayD_;
}

// Destructor
VariableValue::~VariableValue()
{
}

// VariableType names
const char* ValueTypeNames[] = { "Boolean", "Integer", "Double", "Character", "IntegerArray", "DoubleArray" };

// Return VariableType name
const char* VariableValue::valueType(ValueType vt)
{
	return ValueTypeNames[vt];
}

/*
 * Definition
 */

// Return type of variable
VariableValue::ValueType VariableValue::type()
{
	return type_;
}

/*
 * Value
 */

// Set variable value
bool VariableValue::set(VariableValue newValue)
{
	if (type_ == VariableValue::BooleanType) valueB_ = newValue.asBool();
	else if (type_ == VariableValue::IntegerType) valueI_ = newValue.asInt();
	else if (type_ == VariableValue::DoubleType) valueD_ = newValue.asDouble();
	else if (type_ == VariableValue::CharType) valueC_ = newValue.asChar();
	else if (type_ == VariableValue::IntegerArrayType) arrayI_ = newValue.asIntArray();
	else if (type_ == VariableValue::DoubleArrayType) arrayD_ = newValue.asDoubleArray();
}

// Return variable (as bool)
bool VariableValue::asBool()
{
	if (type_ == VariableValue::BooleanType) return valueB_;
	else if (type_ == VariableValue::IntegerType) return valueI_;
	else if (type_ == VariableValue::DoubleType) return (valueD_ > 0.0);
	else if (type_ == VariableValue::CharType) return DUQSys::atob(valueC_.get());
	else
	{
		Messenger::error("Can't return a whole Array as a bool.\n");
		return false;
	}
}

// Return variable (as int)
int VariableValue::asInt()
{
	if (type_ == VariableValue::BooleanType) return int(valueB_);
	else if (type_ == VariableValue::IntegerType) return valueI_;
	else if (type_ == VariableValue::DoubleType) return int(valueD_);
	else if (type_ == VariableValue::CharType) return atoi(valueC_.get());
	else
	{
		Messenger::error("Can't return a whole Array as an int.\n");
		return 0;
	}
}

// Return variable (as double)
double VariableValue::asDouble()
{
	if (type_ == VariableValue::BooleanType) return (valueB_ ? 1.0 : 0.0);
	else if (type_ == VariableValue::IntegerType) return double(valueI_);
	else if (type_ == VariableValue::DoubleType) return valueD_;
	else if (type_ == VariableValue::CharType) return atof(valueC_.get());
	else
	{
		Messenger::error("Can't return a whole Array as a double.\n");
		return 0.0;
	}
}

// Return variable (as string)
const char* VariableValue::asChar()
{
	if (type_ == VariableValue::BooleanType)
	{
		return (valueB_ ? "True" : "False");
	}
	else if (type_ == VariableValue::IntegerType)
	{
		conversionStringTemp_.sprintf("%i", valueI_);
		return conversionStringTemp_.get();
	}
	else if (type_ == VariableValue::DoubleType)
	{
		conversionStringTemp_.sprintf("%f", valueD_);
		return conversionStringTemp_.get();
	}
	else if (type_ == VariableValue::CharType) return valueC_.get();
	else
	{
		Messenger::error("Can't return a whole Array as a const char*.\n");
		return "[ARRAY]";
	}
}

// Return variable (as integer array)
Array<int>& VariableValue::asIntArray()
{
	if (type_ == VariableValue::IntegerArrayType) return arrayI_;

	Messenger::error("Can't convert single values (or Array<double>) into an Array<int> in VariableValue::asIntArray().\n");
	static Array<int> dummyResult;
	return dummyResult;
}

// Return variable (as double array)
Array<double>& VariableValue::asDoubleArray()
{
	if (type_ == VariableValue::DoubleArrayType) return arrayD_;

	Messenger::error("Can't convert single values (or Array<int>) into an Array<double> in VariableValue::asDoubleArray().\n");
	static Array<double> dummyResult;
	return dummyResult;
}

/*
 * Parallel Comms
 */

// Broadcast data
bool VariableValue::broadcast(ProcessPool& procPool)
{
#ifdef PARALLEL
	// Broadcast type first, then value
	int tempType = type_;
	if (!procPool.broadcast(&tempType, 1)) return false;
	type_ = (VariableValue::ValueType) tempType;
	switch (type_)
	{
		case (VariableValue::BooleanType):
			if (!procPool.broadcast(&valueB_, 1)) return false;
			break;
		case (VariableValue::IntegerType):
			if (!procPool.broadcast(&valueI_, 1)) return false;
			break;
		case (VariableValue::DoubleType):
			if (!procPool.broadcast(&valueD_, 1)) return false;
			break;
		case (VariableValue::CharType):
			if (!procPool.broadcast(valueC_)) return false;
			break;
		case (VariableValue::IntegerArrayType):
			if (!procPool.broadcast(arrayI_)) return false;
			break;
		case (VariableValue::DoubleArrayType):
			if (!procPool.broadcast(arrayD_)) return false;
			break;
		default:
			Messenger::error("Broadcast of VariableValue failed - type_ %s not accounted for.\n", VariableValue::valueType(type_));
			return false;
	}
#endif
	return true;
}
