/*
	*** Variable
	*** src/base/variable.cpp
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

#include "base/variable.h"
#include "base/comms.h"

// Constructor
Variable::Variable()
{
	type_ = Variable::IntegerType;
}

// Destructor
Variable::~Variable()
{
}

/*
 * Definition
 */

// Setup variable (int)
void Variable::setup(const char* name, int defaultValue)
{
	name_ = name;
	valueI_ = defaultValue;
	type_ = Variable::IntegerType;
}

// Setup variable (double)
void Variable::setup(const char* name, double defaultValue)
{
	name_ = name;
	valueD_ = defaultValue;
	type_ = Variable::DoubleType;
}

// Setup variable (string)
void Variable::setup(const char* name, const char* defaultValue)
{
	name_ = name;
	valueC_ = defaultValue;
	type_ = Variable::StringType;
}

// Return name of variable
const char* Variable::name()
{
	return name_.get();
}

// Return type of variable
Variable::VariableType Variable::type()
{
	return type_;
}

/*
 * Value
 */

// Set variable value (int)
bool Variable::set(int value)
{
	if (type_ == Variable::IntegerType) valueI_ = value;
	else if (type_ == Variable::DoubleType) valueD_ = double(value);
	else if (type_ == Variable::StringType) valueC_.sprintf("%i", value);
}

// Set variable value (double)
bool Variable::set(double value)
{
	if (type_ == Variable::IntegerType) valueI_ = int(value);
	else if (type_ == Variable::DoubleType) valueD_ = value;
	else if (type_ == Variable::StringType) valueC_.sprintf("%f", value);
}

// Set variable value (string)
bool Variable::set(const char* value)
{
	if (type_ == Variable::IntegerType) valueI_ = atoi(value);
	else if (type_ == Variable::DoubleType) valueD_ = atof(value);
	else if (type_ == Variable::StringType) valueC_ = value;
}

// Return variable (as int)
int Variable::asInt()
{
	if (type_ == Variable::IntegerType) return valueI_;
	else if (type_ == Variable::DoubleType) return int(valueD_);
	else if (type_ == Variable::StringType) return atoi(valueC_.get());
}

// Return variable (as double)
double Variable::asDouble()
{
	if (type_ == Variable::IntegerType) return double(valueI_);
	else if (type_ == Variable::DoubleType) return valueD_;
	else if (type_ == Variable::StringType) return atof(valueC_.get());
}

// Return variable (as string)
const char* Variable::asChar()
{
	if (type_ == Variable::IntegerType)
	{
		conversionStringTemp_.sprintf("%i", valueI_);
		return conversionStringTemp_.get();
	}
	else if (type_ == Variable::DoubleType)
	{
		conversionStringTemp_.sprintf("%f", valueD_);
		return conversionStringTemp_.get();
	}
	else if (type_ == Variable::StringType) return valueC_.get();
}

/*
 * Parallel Comms
 */

// Broadcast data to all processes
bool Variable::broadcast()
{
#ifdef PARALLEL
	if (!Comm.broadcast(name_)) return false;
	int tempType = type_;
	if (!Comm.broadcast(&tempType, 1)) return false;
	type_ = (Variable::VariableType) tempType;
	switch (type_)
	{
		case (Variable::IntegerType):
			if (!Comm.broadcast(&valueI_, 1)) return false;
			break;
		case (Variable::DoubleType):
			if (!Comm.broadcast(&valueD_, 1)) return false;
			break;
		case (Variable::StringType):
			if (!Comm.broadcast(valueC_)) return false;
			break;
		default:
			Messenger::error("Broadcast of Variable failed - type_ not accounted for.\n");
			return false;
	}
#endif
	return true;
}
