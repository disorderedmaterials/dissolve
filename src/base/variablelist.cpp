/*
	*** Variable List
	*** src/base/variablelist.cpp
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

#include "base/variablelist.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "base/processpool.h"

// Constructor
VariableList::VariableList()
{
}

// Destructor
VariableList::~VariableList()
{
}

/*
 * Variables
 */

// Return first Variable in list
Variable* VariableList::variables()
{
	return variables_.first();
}

// Add/set variable
void VariableList::setVariable(const char* name, VariableValue value, const char* description, const char* source)
{
	// Does the variable already exist?
	Variable* var  = variable(name, source);
	if (!var)
	{
		var = variables_.add();
		var->setup(name, value, description, source);
		Messenger::printVerbose("Added new %s variable '%s' with value '%s'.\n", VariableValue::valueType(var->type()), name, value.asChar());
	}
	else
	{
		Messenger::printVerbose("Set existing %s variable '%s' to value '%s' (previous value was '%s').\n", VariableValue::valueType(var->type()), name, value.asChar(), var->asChar());
		var->set(value);
	}
}

// Retrieve named variable (bool)
bool VariableList::variableAsBool(const char* name, const char* source)
{
	// Does the variable already exist?
	Variable* var = variable(name, source);
	if (!var)
	{
		Messenger::error("Can't find boolean variable '%s' (source = '%s').\n", name, source);
		return 0;
	}

	return var->asBool();
}

// Retrieve named variable (int)
int VariableList::variableAsInt(const char* name, const char* source)
{
	// Does the variable already exist?
	Variable* var = variable(name, source);
	if (!var)
	{
		Messenger::error("Can't find integer variable '%s' (source = '%s').\n", name, source);
		return 0;
	}

	return var->asInt();
}

// Retrieve named variable (double)
double VariableList::variableAsDouble(const char* name, const char* source)
{
	// Does the variable already exist?
	Variable* var = variable(name, source);
	if (!var)
	{
		Messenger::error("Can't find double variable '%s' (source = '%s').\n", name, source);
		return 0.0;
	}

	return var->asDouble();
}

// Retrieve named variable (string)
const char* VariableList::variableAsChar(const char* name, const char* source)
{
	// Does the variable already exist?
	Variable* var = variable(name, source);
	if (!var)
	{
		Messenger::error("Can't find string variable '%s' (source = '%s').\n", name, source);
		return "NULL";
	}

	return var->asChar();
}

/*
 * Search
 */

// Return named variable
Variable* VariableList::variable(const char* name, const char* source)
{
	if (DUQSys::isEmpty(source))
	{
		for (Variable* var = variables_.first(); var != NULL; var = var->next) if (DUQSys::sameString(name, var->name())) return var;
	}
	else
	{
		for (Variable* var = variables_.first(); var != NULL; var = var->next)
		{
			if ((DUQSys::sameString(name, var->name())) && (DUQSys::sameString(source, var->source()))) return var;
		}
	}
	return NULL;
}

/*
 * Parallel Comms
 */

// Broadcast data
bool VariableList::broadcast(ProcessPool& procPool)
{
#ifdef PARALLEL
	// Clear list on slaves
	if (procPool.isSlave()) variables_.clear();

	// Broadcast number of variables to expect
	int varCount = variables_.nItems();
	if (!procPool.broadcast(&varCount, 1)) return false;
	for (int n=0; n<varCount; ++n)
	{
		Variable* var;
		if (procPool.isMaster()) var = variables_[n];
		else var = variables_.add();
		var->broadcast(procPool);
	}
#endif
	return true;
}
