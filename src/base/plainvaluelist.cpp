/*
	*** Plain Value List
	*** src/base/plainvaluelist.cpp
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

#include "base/plainvaluelist.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "base/processpool.h"

// Constructor
PlainValueList::PlainValueList()
{
}

// Destructor
PlainValueList::~PlainValueList()
{
}

/*
 * Values
 */

// Return first Value in list
PlainValue* PlainValueList::values()
{
	return values_.first();
}

// Add/set value
PlainValue* PlainValueList::add(const char* name, PlainValue newValue, const char* description, int genericItemFlags)
{
	// Does the value already exist?
	PlainValue* var = value(name);
	if (!var)
	{
		var = values_.add();
		var->initialise(name, newValue, description, genericItemFlags);
		Messenger::printVerbose("Added new %s value '%s' with value '%s'.\n", PlainValue::valueType(var->type()), name, newValue.asString());
	}
	else
	{
		Messenger::printVerbose("Set existing %s value '%s' to value '%s' (previous value was '%s').\n", PlainValue::valueType(var->type()), name, newValue.asString(), var->asString());
		var->set(newValue);
	}

	return var;
}

// Retrieve named value (bool)
bool PlainValueList::valueAsBool(const char* name)
{
	// Does the value already exist?
	PlainValue* oldValue = value(name);
	if (!oldValue)
	{
		Messenger::error("PlainValueList::valueAsBool - Can't find value '%s'.\n", name);
		return 0;
	}

	return oldValue->asBool();
}

// Retrieve named value (int)
int PlainValueList::valueAsInt(const char* name)
{
	// Does the value already exist?
	PlainValue* oldValue = value(name);
	if (!oldValue)
	{
		Messenger::error("PlainValueList::valueAsInt - Can't find value '%s'.\n", name);
		return 0;
	}

	return oldValue->asInt();
}

// Retrieve named value (double)
double PlainValueList::valueAsDouble(const char* name)
{
	// Does the value already exist?
	PlainValue* oldValue = value(name);
	if (!oldValue)
	{
		Messenger::error("PlainValueList::valueAsDouble - Can't find value '%s'.\n", name);
		return 0.0;
	}

	return oldValue->asDouble();
}

// Retrieve named value (string)
const char* PlainValueList::valueAsString(const char* name)
{
	// Does the value already exist?
	PlainValue* oldValue = value(name);
	if (!oldValue)
	{
		Messenger::error("PlainValueList::valueAsString - Can't find value '%s'.\n", name);
		return "NULL";
	}

	return oldValue->asString();
}

/*
 * Search
 */

// Return named value
PlainValue* PlainValueList::value(const char* name)
{
	for (PlainValue* oldValue = values_.first(); oldValue != NULL; oldValue = oldValue->next()) if (DissolveSys::sameString(name, oldValue->name())) return oldValue;

	return NULL;
}

/*
 * Parallel Comms
 */

// Broadcast data
bool PlainValueList::broadcast(ProcessPool& procPool)
{
#ifdef PARALLEL
	// Clear list on slaves
	if (procPool.isSlave()) values_.clear();

	// Broadcast number of values to expect
	int varCount = values_.nItems();
	if (!procPool.broadcast(&varCount, 1)) return false;
	for (int n=0; n<varCount; ++n)
	{
		PlainValue* value;
		if (procPool.isMaster()) value = values_[n];
		else value = values_.add();
		value->broadcast(procPool);
	}
#endif
	return true;
}
