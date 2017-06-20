/*
	*** PlainValue
	*** src/base/plainvalue.cpp
	Copyright T. Youngs 2012-2017

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

#include "base/plainvalue.h"
#include "base/processpool.h"
#include "base/sysfunc.h"

// Constructor
PlainValue::PlainValue()
{
	type_ = PlainValue::IntegerType;
	valueI_ = 0;
	genericItemFlags_ = -1;
}

// Constructor (bool)
PlainValue::PlainValue(bool value)
{
	type_ = PlainValue::BooleanType;
	valueB_ = value;
	genericItemFlags_ = -1;
}

// Constructor (int)
PlainValue::PlainValue(int value)
{
	type_ = PlainValue::IntegerType;
	valueI_ = value;
	genericItemFlags_ = -1;
}

// Constructor (double)
PlainValue::PlainValue(double value)
{
	type_ = PlainValue::DoubleType;
	valueD_ = value;
	genericItemFlags_ = -1;
}

// Constructor (string)
PlainValue::PlainValue(const char* value)
{
	type_ = PlainValue::StringType;
	valueC_ = value;
	genericItemFlags_ = -1;
}

// Constructor (CharString)
PlainValue::PlainValue(const CharString& value)
{
	type_ = PlainValue::StringType;
	valueC_ = value;
	genericItemFlags_ = -1;
}

// Constructor (PlainValue)
PlainValue::PlainValue(const PlainValue& value)
{
	(*this) = value;
}

// Assignment operator
void PlainValue::operator=(const PlainValue& value)
{
	name_ = value.name_;
	description_ = value.description_;
	genericItemFlags_ = value.genericItemFlags_;

	set(value);
}

// Destructor
PlainValue::~PlainValue()
{
}

// VariableType names
const char* ValueTypeNames[] = { "Boolean", "Integer", "Double", "String" };

// Return VariableType name
const char* PlainValue::valueType(ValueType vt)
{
	return ValueTypeNames[vt];
}

/*
 * Definition
 */

// Set value, including name and description
void PlainValue::initialise(const char* name, PlainValue newValue, const char* description, int genericItemFlags)
{
	name_ = name;
	description_ = description;
	genericItemFlags_ = genericItemFlags;

	set(newValue);
}

// Set value
void PlainValue::set(const PlainValue& value)
{
	type_ = value.type_;
	valueB_ = value.valueB_;
	valueI_ = value.valueI_;
	valueD_ = value.valueD_;
	valueC_ = value.valueC_;
}

// Return name of value
const char* PlainValue::name()
{
	return name_.get();
}

// Return description for value
const char* PlainValue::description()
{
	return description_.get();
}

// Return type of variable
PlainValue::ValueType PlainValue::type()
{
	return type_;
}

// Return flags to apply if reinstated as a GenericListItem (i.e. in a Module)
int PlainValue::genericItemFlags()
{
	return genericItemFlags_;
}

/*
 * Conversion
 */

// Return variable (as bool)
bool PlainValue::asBool()
{
	if (type_ == PlainValue::BooleanType) return valueB_;
	else if (type_ == PlainValue::IntegerType) return valueI_;
	else if (type_ == PlainValue::DoubleType) return (valueD_ > 0.0);
	else if (type_ == PlainValue::StringType) return DUQSys::atob(valueC_.get());

	return false;
}

// Return variable (as int)
int PlainValue::asInt()
{
	if (type_ == PlainValue::BooleanType) return int(valueB_);
	else if (type_ == PlainValue::IntegerType) return valueI_;
	else if (type_ == PlainValue::DoubleType) return int(valueD_);
	else if (type_ == PlainValue::StringType) return atoi(valueC_.get());

	return 0;
}

// Return variable (as double)
double PlainValue::asDouble()
{
	if (type_ == PlainValue::BooleanType) return (valueB_ ? 1.0 : 0.0);
	else if (type_ == PlainValue::IntegerType) return double(valueI_);
	else if (type_ == PlainValue::DoubleType) return valueD_;
	else if (type_ == PlainValue::StringType) return atof(valueC_.get());

	return 0.0;
}

// Return variable (as string)
const char* PlainValue::asString()
{
	if (type_ == PlainValue::BooleanType)
	{
		return (valueB_ ? "True" : "False");
	}
	else if (type_ == PlainValue::IntegerType)
	{
		conversionStringTemp_.sprintf("%i", valueI_);
		return conversionStringTemp_.get();
	}
	else if (type_ == PlainValue::DoubleType)
	{
		conversionStringTemp_.sprintf("%f", valueD_);
		return conversionStringTemp_.get();
	}
	else if (type_ == PlainValue::StringType) return valueC_.get();

	return "[NULL]";
}

/*
 * Parallel Comms
 */

// Broadcast data
bool PlainValue::broadcast(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!procPool.broadcast(name_)) return false;
	if (!procPool.broadcast(description_)) return false;

	// Broadcast type first, then value
	int tempType = type_;
	if (!procPool.broadcast(&tempType, 1)) return false;
	type_ = (PlainValue::ValueType) tempType;
	switch (type_)
	{
		case (PlainValue::BooleanType):
			if (!procPool.broadcast(valueB_)) return false;
			break;
		case (PlainValue::IntegerType):
			if (!procPool.broadcast(&valueI_, 1)) return false;
			break;
		case (PlainValue::DoubleType):
			if (!procPool.broadcast(&valueD_, 1)) return false;
			break;
		case (PlainValue::StringType):
			if (!procPool.broadcast(valueC_)) return false;
			break;
		default:
			Messenger::error("Broadcast of PlainValue failed - type_ %s not accounted for.\n", PlainValue::valueType(type_));
			return false;
	}
#endif
	return true;
}

