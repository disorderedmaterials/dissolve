/*
	*** Expression Function Node
	*** src/expression/functionnode.h
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

#ifndef DISSOLVE_UCHROMAFUNCTIONNODE_H
#define DISSOLVE_UCHROMAFUNCTIONNODE_H

#include "expression/functions.h"
#include "expression/node.h"

// Forward Declarations
/* none */

// Function Node
class FunctionNode : public Node
{
	public:
	// Constructors / Destructor
	FunctionNode(Functions::Function func = Functions::NoFunction);
	FunctionNode(Node* source);
	~FunctionNode();


	/*
	 * Function Data
	 */
	protected:
	// Function that this node performs
	Functions::Function function_;
	
	public:
	// Get command function
	Functions::Function function();
	// Execute command
	bool execute(double& rv);
	// Print node contents
	void nodePrint(int offset, const char* prefix = "");
	// Set from returnvalue node
	bool set(double rv);
	// Initialise node
	bool initialise();
};

#endif
