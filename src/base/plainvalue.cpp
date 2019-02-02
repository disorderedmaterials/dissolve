/*
	*** PlainValue
	*** src/base/plainvalue.cpp
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

#include "base/plainvalue.h"
#include "base/processpool.h"
#include "base/sysfunc.h"

// Constructor
PlainValue::PlainValue()
{
	clear();

	type_ = PlainValue::IntegerType;
	valueI_ = 0;
}

// Constructor (bool)
PlainValue::PlainValue(bool value)
{
	clear();

	type_ = PlainValue::BooleanType;
	valueB_ = value;
}

// Constructor (int)
PlainValue::PlainValue(int value)
{
	clear();

	type_ = PlainValue::IntegerType;
	valueI_ = value;
}

// Constructor (double)
PlainValue::PlainValue(double value)
{
	clear();

	type_ = PlainValue::DoubleType;
	valueD_ = value;
}

// Constructor (string)
PlainValue::PlainValue(const char* value)
{
	clear();

	type_ = PlainValue::StringType;
	valueC_ = value;
}

// Constructor (CharString)
PlainValue::PlainValue(const CharString& value)
{
	clear();

	type_ = PlainValue::StringType;
	valueC_ = value;
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
	hasValidation_ = false;
	minI_ = value.minI_;
	maxI_ = value.maxI_;
	minD_ = value.minD_;
	maxD_ = value.maxD_;
	nValidC_ = value.nValidC_;
	validC_ = value.validC_;

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

// Clear all data
void PlainValue::clear()
{
	genericItemFlags_ = -1;
	hasValidation_ = false;
	minimumLimit_ = false;
	maximumLimit_ = false;
	minI_ = 0;
	maxI_ = 0;
	minD_ = 0.0;
	maxD_ = 0.0;
	nValidC_ = 0;
	validC_ = NULL;
}

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
 * Validation
 */

// Set integer validation range
void PlainValue::setValidationRange(int minValue, int maxValue)
{
	if (type_ != PlainValue::IntegerType) Messenger::warn("Setting integer validation for a non-integer PlainValue.\n");

	hasValidation_ = true;
	minimumLimit_ = true;
	maximumLimit_ = true;
	minI_ = minValue;
	maxI_ = maxValue;
}

// Set minimum validation limit
void PlainValue::setValidationMin(int minValue)
{
	if (type_ != PlainValue::IntegerType) Messenger::warn("Setting integer validation maximum limit for a non-integer PlainValue.\n");

	minimumLimit_ = true;
	minI_ = minValue;
}

// Set maximum validation limit
void PlainValue::setValidationMax(int maxValue)
{
	if (type_ != PlainValue::IntegerType) Messenger::warn("Setting integer validation minimum limit for a non-integer PlainValue.\n");

	maximumLimit_ = true;
	maxI_ = maxValue;
}

// Return minimum integer value allowed
int PlainValue::minI()
{
	return minI_;
}

// Return maximum integer value allowed
int PlainValue::maxI()
{
	return maxI_;
}

// Set double validation range
void PlainValue::setValidationRange(double minValue, double maxValue)
{
	if (type_ != PlainValue::DoubleType) Messenger::warn("Setting double validation for a non-double PlainValue.\n");

	hasValidation_ = true;
	minD_ = minValue;
	maxD_ = maxValue;
}

// Set minimum validation limit
void PlainValue::setValidationMin(double minValue)
{
	if (type_ != PlainValue::DoubleType) Messenger::warn("Setting double validation minimum limit for a non-double PlainValue.\n");
	
	minimumLimit_ = true;
	minD_ = minValue;
}

// Set maximum validation limit
void PlainValue::setValidationMax(double maxValue)
{
	if (type_ != PlainValue::DoubleType) Messenger::warn("Setting double validation maximum limit for a non-double PlainValue.\n");

	maximumLimit_ = true;
	maxD_ = maxValue;
}

// Return minimum double value allowed
double PlainValue::minD()
{
	return minD_.value();
}

// Return maximum double value allowed
double PlainValue::maxD()
{
	return maxD_.value();
}

// Return minimum double value allowed (as string)
const char* PlainValue::minDAsString()
{
	return minD_.asString();
}

// Return maximum double value allowed (as string)
const char* PlainValue::maxDAsString()
{
	return maxD_.asString();
}

// Set string validation
void PlainValue::setValidation(int nValues, const char** values)
{
	if (type_ != PlainValue::StringType) Messenger::warn("Setting string validation for a non-string PlainValue.\n");

	hasValidation_ = true;
	nValidC_ = nValues;
	validC_ = values;
}

// Return valid string values
CharString PlainValue::validC()
{
	CharString result;
	for (int n=0; n<nValidC_; ++n)
	{
		if (n > 0) result += ", ";
		result += validC_[n];
	}

	return result;
}

// Validate integer value
bool PlainValue::isValid(int i)
{
	if (!hasValidation_) return true;

	if (minimumLimit_ && (i < minI_)) return false;
	else if (maximumLimit_ && (i > maxI_)) return false;

	return true;
}

// Valudate double value
bool PlainValue::isValid(double d)
{
	if (!hasValidation_) return true;

	if (minimumLimit_ && (d < minD_)) return false;
	else if (maximumLimit_ && (d > maxD_)) return false;

	return true;
}

// Validate string value
bool PlainValue::isValid(const char* s)
{
	if (!hasValidation_) return true;

	// Loop over list of values
	for (int n=0; n<nValidC_; ++n) if (DissolveSys::sameString(s, validC_[n])) return true;

	return false;
}

// Return valid range
CharString PlainValue::validRange()
{
	CharString result;
	if (!hasValidation_) result = "< No Range >";
	else if (type_ == PlainValue::IntegerType)
	{
		if (minimumLimit_ && maximumLimit_) result.sprintf("%i <= n <= %i\n", minI_, maxI_);
		else if (maximumLimit_) result.sprintf("n <= %i", maxI_);
		else result.sprintf("%i <= n", minI_);
	}
	else if (type_ == PlainValue::DoubleType)
	{
		if (minimumLimit_ && maximumLimit_) result.sprintf("%s <= n <= %s\n", minD_.asString().get(), maxD_.asString().get());
		else if (maximumLimit_) result.sprintf("%s <= n", minD_.asString().get());
		else result.sprintf("n <= %s", maxD_.asString().get());
	}
	else result = validC();
	return result;
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
	else if (type_ == PlainValue::StringType) return DissolveSys::atob(valueC_.get());

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

