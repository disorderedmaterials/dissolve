/*
	*** Expression Function Node
	*** src/expression/functionnode.cpp
	Copyright T. Youngs 2015-2019

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

#include "expression/function.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include "base/charstring.h"
#include <string.h>
#include <stdarg.h>

// Constructor
ExpressionFunction::ExpressionFunction(ExpressionFunctions::Function func) : ExpressionNode(), function_(func)
{
	// Private variables
	nodeType_ = ExpressionNode::FunctionNode;
}

ExpressionFunction::ExpressionFunction(ExpressionNode* source)
{
	copy(source);	
}

// Destructor
ExpressionFunction::~ExpressionFunction()
{
}

// Get function
ExpressionFunctions::Function ExpressionFunction::function() const
{
	return function_;
}

// Execute command
bool ExpressionFunction::execute(ExpressionValue& result)
{
	// Execute the command
	//printf("Node function is %i (%s)\n", function_, commands.data[function_].keyword);
	return expressionFunctions.call(function_, this, result);
}

// Print node contents
void ExpressionFunction::nodePrint(int offset, const char* prefix)
{
	// Construct tabbed offset
	CharString tab;
	for (int n=0; n<offset-1; n++) tab += '\t';
	if (offset > 1) tab += "   |--> ";
	tab += prefix;

	// Output node data
// 	printf("Function id = %p\n", function_);
	printf("[CN]%s%s (Function) (%i arguments)\n", tab.get(), ExpressionFunctions::data[function_].keyword, args_.nItems());
	// Output Argument data
	for (RefListItem<ExpressionNode,int>* ri = args_.first(); ri != NULL; ri = ri->next) ri->item->nodePrint(offset+1);
}

// Set from ExpressionValue
bool ExpressionFunction::set(ExpressionValue value)
{
	printf("Internal Error: Trying to 'set' a FunctionNode.\n");
	return false;
}

// Initialise node
bool ExpressionFunction::initialise()
{
	printf("Internal Error: A FunctionNode cannot be initialised.\n");
	return false;
}
