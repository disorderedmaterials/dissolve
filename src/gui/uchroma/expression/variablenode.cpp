/*
	*** Expression Variable Node
	*** src/gui/uchroma/expression/variablenode.cpp
	Copyright T. Youngs 2015

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/expression/variablenode.h"
#include "gui/uchroma/expression/variable.h"
#include <string.h>

// Constructor
VariableNode::VariableNode(Variable* var) : Node(), variable_(var)
{
	// Private variables
	readOnly_ = false;
	nodeType_ = Node::VarWrapperNode;
	if (variable_ != NULL) returnsNumber_ = var->returnsNumber();
}

// Destructor
VariableNode::~VariableNode()
{
}

// Set function
void VariableNode::setVariable(Variable* variable)
{
	variable_ = variable;
}

// Get function
Variable* VariableNode::variable()
{
	return variable_;
}

// Return name of variable target
QString VariableNode::name()
{
	if (variable_ == NULL)
	{
		printf("Internal Error: VariableNode contains a NULL Variable pointer.\n");
		return "NULL";
	}
	return variable_->name();
}

/*
 * Set / execute
 */

// Execute command
bool VariableNode::execute(double& rv)
{
	if (variable_ == NULL)
	{
		printf("Internal Error: VariableNode contains a NULL Variable pointer and can't be executed.\n");
		return false;
	}

	// Call the local variable's execute() function to get the base value
	bool result = variable_->execute(rv);
	if (!result) printf("Variable retrieval ('%s') failed.\n", qPrintable(variable_->name()));

	return result;
}

// Print node contents
void VariableNode::nodePrint(int offset, const char* prefix)
{
	if (variable_ == NULL)
	{
		printf("Internal Error: VariableNode contains a NULL Variable pointer and can't be printed.\n");
		return;
	}
	// Call the local variables nodePrint() function
	variable_->nodePrint(offset, prefix);
}

// Set from returnvalue node
bool VariableNode::set(double setrv)
{
	if (variable_ == NULL)
	{
		printf("Internal Error: VariableNode contains a NULL Variable pointer and can't be set.\n");
		return false;
	}
	bool result = true;

	// Call the local variable's set() function
	result = variable_->set(setrv);
	if (!result) printf("Variable set failed.\n");

	return result;
}

// Initialise node
bool VariableNode::initialise()
{
	if (variable_ == NULL)
	{
		printf("Internal Error: VariableNode contains a NULL Variable pointer and can't be initialised.\n");
		return false;
	}
	return variable_->initialise();
}
