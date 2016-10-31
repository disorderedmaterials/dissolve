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

// Add/set variable (bool)
void VariableList::setVariable(const char* name, bool value, const char* description)
{
	// Does the variable already exist?
	Variable* var  = variable(name);
	if (!var)
	{
		Messenger::printVerbose("Added new boolean variable '%s' with value '%s'.\n", name, value ? "True" : "False");
		var = variables_.add();
		var->setup(name, value, description);
	}
	else
	{
		Messenger::printVerbose("Set existing boolean variable '%s' to value '%s' (previous value was '%s').\n", name, value ? "True" : "False", var->asBool() ? "True" : "False");
		var->set(value);
	}
}

// Add/set variable (int)
void VariableList::setVariable(const char* name, int value, const char* description)
{
	// Does the variable already exist?
	Variable* var  = variable(name);
	if (!var)
	{
		Messenger::printVerbose("Added new integer variable '%s' with value '%i'.\n", name, value);
		var = variables_.add();
		var->setup(name, value, description);
	}
	else
	{
		Messenger::printVerbose("Set existing integer variable '%s' to value '%i' (previous value was '%i').\n", name, value, var->asInt());
		var->set(value);
	}
}

// Add/set variable (double)
void VariableList::setVariable(const char* name, double value, const char* description)
{
	// Does the variable already exist?
	Variable* var = variable(name);
	if (!var)
	{
		Messenger::printVerbose("Added new double variable '%s' with value '%f'.\n", name, value);
		var = variables_.add();
		var->setup(name, value, description);
	}
	else
	{
		Messenger::printVerbose("Set existing double variable '%s' to value '%f' (previous value was '%f').\n", name, value, var->asDouble());
		var->set(value);
	}
}

// Add/set variable (string)
void VariableList::setVariable(const char* name, const char* value, const char* description)
{
	// Does the variable already exist?
	Variable* var = variable(name);
	if (!var)
	{
		Messenger::printVerbose("Added new string variable '%s' with value '%s'.\n", name, value);
		var = variables_.add();
		var->setup(name, value, description);
	}
	else
	{
		Messenger::printVerbose("Set existing string variable '%s' to value '%s' (previous value was '%s').\n", name, value, var->asChar());
		var->set(value);
	}
}

// Retrieve named variable (bool)
bool VariableList::variableAsBool(const char* name)
{
	// Does the variable already exist?
	Variable* var = variable(name);
	if (!var)
	{
		Messenger::error("Can't find boolean variable '%s'.\n", name);
		return 0;
	}

	return var->asBool();
}

// Retrieve named variable (int)
int VariableList::variableAsInt(const char* name)
{
	// Does the variable already exist?
	Variable* var = variable(name);
	if (!var)
	{
		Messenger::error("Can't find integer variable '%s'.\n", name);
		return 0;
	}

	return var->asInt();
}

// Retrieve named variable (double)
double VariableList::variableAsDouble(const char* name)
{
	// Does the variable already exist?
	Variable* var = variable(name);
	if (!var)
	{
		Messenger::error("Can't find double variable '%s'.\n", name);
		return 0.0;
	}

	return var->asDouble();
}

// Retrieve named variable (string)
const char* VariableList::variableAsChar(const char* name)
{
	// Does the variable already exist?
	Variable* var = variable(name);
	if (!var)
	{
		Messenger::error("Can't find string variable '%s'.\n", name);
		return "NULL";
	}

	return var->asChar();
}

/*
 * Search
 */

// Return named variable
Variable* VariableList::variable(const char* name)
{
	for (Variable* var = variables_.first(); var != NULL; var = var->next) if (DUQSys::sameString(name, var->name())) return var;
	return NULL;
}

// Return named variable (with prefix)
Variable* VariableList::variable(const char* prefix, const char* name)
{
	// Construct variable name
	Dnchar varName;
	varName.sprintf("%s_%s", prefix, name);

	for (Variable* var = variables_.first(); var != NULL; var = var->next) if (DUQSys::sameString(varName, var->name())) return var;
	return NULL;
}

/*
 * Parallel Comms
 */

// Broadcast data
bool VariableList::broadcastVariables(ProcessPool& procPool)
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
