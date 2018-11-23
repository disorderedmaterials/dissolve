/*
	*** Expression Variable
	*** src/expression/variable.cpp
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

#include "expression/variable.h"
#include <string.h>

// Constructor
Variable::Variable(double value, bool readOnly) : Node()
{
	// Private variables
	name_ = "unnamedvariable";
	initialValue_ = NULL;
	nodeType_ = Node::VarNode;
	value_ = value;
	returnsNumber_ = true;
	readOnly_ = readOnly;
}

// Destructor (virtual)
Variable::~Variable()
{
}

// Set name of variable
void Variable::setName(const char* s)
{
	name_ = s;
}

// Get name of variable
const char* Variable::name() const
{
	return name_.get();
}

// Initialise variable
bool Variable::initialise()
{
	if (initialValue_ == NULL) value_ = 0.0;
	else
	{
		double rv;
		if (initialValue_->execute(rv))
		{
			if (set(rv)) return true;
			else
			{
				printf("Error: Failed to initialise variable '%s'.\n", name_.get());
				return false;
			}
		}
		return false;
	}
	return true;
}

// Set initial value expression
bool Variable::setInitialValue(Node* node)
{
	initialValue_ = node;
	if (initialValue_ == NULL) return true;

	// Check return type
	if (!node->returnsNumber())
	{
		printf("Error: Initial value for '%s' does not return a number.\n", name_.get());
		return false;
	}

	return true;
}

// Return Node corresponding to initial value
Node* Variable::initialValue() const
{
	return initialValue_;
}

/*
 * Set / Get
 */

// Set value of variable (real)
bool Variable::set(double rv)
{
	if (readOnly_)
	{
		printf("A constant value (in this case a double) cannot be assigned to.\n");
		return false;
	}
	value_ = rv;
	return true;
}

// Return value of node
bool Variable::execute(double& rv)
{
	rv = value_;
	return true;
}

// Print node contents
void Variable::nodePrint(int offset, const char* prefix)
{
	// Construct tabbed offset
	CharString tab;
	for (int n=0; n<offset-1; n++) tab += '\t';
	if (offset > 1) tab += "   |--> ";
	tab += prefix;

	// Output node data
	if (readOnly_) printf("[C]%s%f (constant value)\n", tab.get(), value_);
	else printf("[V]%s%f (variable, name=%s)\n", tab.get(), value_, name_.get());
}
