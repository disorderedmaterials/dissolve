/*
	*** Expression Value Node
	*** src/expression/value.cpp
	Copyright T. Youngs 2015-2018

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

#include "expression/value.h"
#include "expression/variable.h"
#include <string.h>

// Constructor
ExpressionValue::ExpressionValue(ExpressionVariable* var) : ExpressionNode(), variable_(var)
{
	// Private variables
	readOnly_ = false;
	nodeType_ = ExpressionNode::ValueNode;
	if (variable_ != NULL) returnsNumber_ = var->returnsNumber();
}

// Destructor
ExpressionValue::~ExpressionValue()
{
}

// Set function
void ExpressionValue::setVariable(ExpressionVariable* variable)
{
	variable_ = variable;
}

// Get function
ExpressionVariable* ExpressionValue::variable() const
{
	return variable_;
}

// Return name of variable target
const char* ExpressionValue::name() const
{
	if (variable_ == NULL)
	{
		printf("Internal Error: ExpressionValue contains a NULL Variable pointer.\n");
		return "NULL";
	}
	return variable_->name();
}

/*
 * Set / execute
 */

// Execute command
bool ExpressionValue::execute(double& rv)
{
	if (variable_ == NULL)
	{
		printf("Internal Error: ExpressionValue contains a NULL Variable pointer and can't be executed.\n");
		return false;
	}

	// Call the local variable's execute() function to get the base value
	bool result = variable_->execute(rv);
	if (!result) printf("Variable retrieval ('%s') failed.\n", variable_->name());

	return result;
}

// Print node contents
void ExpressionValue::nodePrint(int offset, const char* prefix)
{
	if (variable_ == NULL)
	{
		printf("Internal Error: ExpressionValue contains a NULL Variable pointer and can't be printed.\n");
		return;
	}
	// Call the local variables nodePrint() function
	variable_->nodePrint(offset, prefix);
}

// Set from double value
bool ExpressionValue::set(double value)
{
	if (variable_ == NULL)
	{
		printf("Internal Error: ExpressionValue contains a NULL Variable pointer and can't be set.\n");
		return false;
	}
	bool result = true;

	// Call the local variable's set() function
	result = variable_->set(value);
	if (!result) printf("Variable set failed.\n");

	return result;
}

// Initialise node
bool ExpressionValue::initialise()
{
	if (variable_ == NULL)
	{
		printf("Internal Error: ExpressionValue contains a NULL Variable pointer and can't be initialised.\n");
		return false;
	}
	return variable_->initialise();
}
