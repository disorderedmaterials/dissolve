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
#include <string.h>

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

// Add/set variable (int)
void VariableList::setVariable(const char* name, int value)
{
	// Does the variable already exist?
	Variable* var = variable(name);
	if (!var)
	{
		msg.printVerbose("Added new integer variable '%s' with value '%i'.\n", name, value);
		var = variables_.add();
		var->setup(name, value);
	}
	else
	{
		msg.printVerbose("Set existing integer variable '%s' to value '%i' (previous value was '%i').\n", name, value, var->asInt());
		var->set(value);
	}
}

// Add/set variable (double)
void VariableList::setVariable(const char* name, double value)
{
	// Does the variable already exist?
	Variable* var= variable(name);
	if (!var)
	{
		msg.printVerbose("Added new double variable '%s' with value '%i'.\n", name, value);
		var = variables_.add();
		var->setup(name, value);
	}
	else
	{
		msg.printVerbose("Set existing double variable '%s' to value '%d' (previous value was '%d').\n", name, value, var->asDouble());
		var->set(value);
	}
}

// Add/set variable (string)
void VariableList::setVariable(const char* name, const char* value)
{
	// Does the variable already exist?
	Variable* var= variable(name);
	if (!var)
	{
		msg.printVerbose("Added new string variable '%s' with value '%i'.\n", name, value);
		var = variables_.add();
		var->setup(name, value);
	}
	else
	{
		msg.printVerbose("Set existing string variable '%s' to value '%s' (previous value was '%s').\n", name, value, var->asChar());
		var->set(value);
	}
}

// Add/set variable with prefix on name (adding underscore inbetween) (int)
void VariableList::setVariable(const char* prefix, const char* name, int value)
{
	// Construct variable name
	Dnchar varName;
	varName.sprintf("%s_%s", prefix, name);

	// Does the variable already exist?
	Variable* var= variable(varName);
	if (!var)
	{
		msg.printVerbose("Added new integer variable '%s' with value '%i'.\n", name, value);
		var = variables_.add();
		var->setup(varName, value);
	}
	else
	{
		msg.printVerbose("Set existing integer variable '%s' to value '%i' (previous value was '%i').\n", name, value, var->asInt());
		var->set(value);
	}
}

// Add/set variable with prefix on name (adding underscore inbetween) (double)
void VariableList::setVariable(const char* prefix, const char* name, double value)
{
	// Construct variable name
	Dnchar varName;
	varName.sprintf("%s_%s", prefix, name);

	// Does the variable already exist?
	Variable* var= variable(varName);
	if (!var)
	{
		msg.printVerbose("Added new double variable '%s' with value '%d'.\n", name, value);
		var = variables_.add();
		var->setup(varName, value);
	}
	else
	{
		msg.printVerbose("Set existing double variable '%s' to value '%d' (previous value was '%i').\n", name, value, var->asDouble());
		var->set(value);
	}
}

// Add/set variable with prefix on name (adding underscore inbetween) (string)
void VariableList::setVariable(const char* prefix, const char* name, const char* value)
{
	// Construct variable name
	Dnchar varName;
	varName.sprintf("%s_%s", prefix, name);

	// Does the variable already exist?
	Variable* var= variable(varName);
	if (!var)
	{
		msg.printVerbose("Added new string variable '%s' with value '%s'.\n", name, value);
		var = variables_.add();
		var->setup(varName, value);
	}
	else
	{
		msg.printVerbose("Set existing string variable '%s' to value '%s' (previous value was '%s').\n", name, value, var->asChar());
		var->set(value);
	}
}

/*
 * Search
 */

// Return named variable
Variable* VariableList::variable(const char* name)
{
	for (Variable* var = variables_.first(); var != NULL; var = var->next) if (strcmp(name, var->name()) == 0) return var;
	return NULL;
}

// Return named variable (with prefix)
Variable* VariableList::variable(const char* prefix, const char* name)
{
	// Construct variable name
	Dnchar varName;
	varName.sprintf("%s_%s", prefix, name);

	for (Variable* var = variables_.first(); var != NULL; var = var->next) if (strcmp(varName, var->name()) == 0) return var;
	return NULL;
}
