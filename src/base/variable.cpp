/*
	*** Variable
	*** src/base/variable.cpp
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

#include "base/variable.h"
#include "base/processpool.h"
#include "sysfunc.h"

// Constructor
Variable::Variable()
{
}

// Destructor
Variable::~Variable()
{
}

/*
 * Definition
 */

// Set up variable
void Variable::setUp(const char* name, VariableValue defaultValue, const char* description, const char* source)
{
	name_ = name;
	source_ = source;
	description_ = description;
	value_ = defaultValue;
}

// Return name of variable
const char* Variable::name()
{
	return name_.get();
}

// Return source of variable
const char* Variable::source()
{
	return source_;
}

// Return type of variable
VariableValue::ValueType Variable::type()
{
	return value_.type();
}

// Return description of variable
const char* Variable::description()
{
	return description_.get();
}

/*
 * Value
 */

// Set variable value
void Variable::set(VariableValue value)
{
	value_ = value;
}

// Return value (as bool)
bool Variable::asBool()
{
	return value_.asBool();
}

// Return value (as int)
int Variable::asInt()
{
	return value_.asInt();
}

// Return value (as double)
double Variable::asDouble()
{
	return value_.asDouble();
}

// Return value (as string)
const char* Variable::asChar()
{
	return value_.asChar();
}

// Return value (as Array<int>)
Array<int>& Variable::asIntArray()
{
	return value_.asIntArray();
}

// Return value (as Array<double>)
Array<double>& Variable::asDoubleArray()
{
	return value_.asDoubleArray();
}

/*
 * Parallel Comms
 */

// Broadcast data
bool Variable::broadcast(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!procPool.broadcast(name_)) return false;
	if (!procPool.broadcast(description_)) return false;
	if (!procPool.broadcast(source_)) return false;
	if (!value_.broadcast(procPool)) return false;
#endif
	return true;
}
