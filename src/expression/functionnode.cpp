/*
	*** Expression Function Node
	*** src/expression/functionnode.cpp
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

#include "expression/functionnode.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include "base/charstring.h"
#include <string.h>
#include <stdarg.h>

// Constructor
FunctionNode::FunctionNode(Functions::Function func) : Node(), function_(func)
{
	// Private variables
	nodeType_ = Node::FuncNode;
}

FunctionNode::FunctionNode(Node* source)
{
	copy(source);	
}

// Destructor
FunctionNode::~FunctionNode()
{
}

// Get function
Functions::Function FunctionNode::function()
{
	return function_;
}

// Execute command
bool FunctionNode::execute(double& rv)
{
	// Execute the command
	//printf("Node function is %i (%s)\n", function_, commands.data[function_].keyword);
	return functions.call(function_, this, rv);
}

// Print node contents
void FunctionNode::nodePrint(int offset, const char* prefix)
{
	// Construct tabbed offset
	CharString tab;
	for (int n=0; n<offset-1; n++) tab += '\t';
	if (offset > 1) tab += "   |--> ";
	tab += prefix;

	// Output node data
// 	printf("Function id = %p\n", function_);
	printf("[CN]%s%s (Function) (%i arguments)\n", tab.get(), Functions::data[function_].keyword, args_.nItems());
	// Output Argument data
	for (RefListItem<Node,int> *ri = args_.first(); ri != NULL; ri = ri->next) ri->item->nodePrint(offset+1);
}

// Set from returnvalue node
bool FunctionNode::set(double rv)
{
	printf("Internal Error: Trying to 'set' a FunctionNode.\n");
	return false;
}

// Initialise node
bool FunctionNode::initialise()
{
	printf("Internal Error: A FunctionNode cannot be initialised.\n");
	return false;
}
