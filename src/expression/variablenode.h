/*
	*** Expression Variable Node
	*** src/expression/variablenode.h
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

#ifndef DISSOLVE_UCHROMAVARIABLENODE_H
#define DISSOLVE_UCHROMAVARIABLENODE_H

#include "expression/node.h"
#include <QString>

// Forward Declarations
class Variable;

// Variable Node
class VariableNode : public Node
{
	public:
	// Constructor / Destructor
	VariableNode(Variable* var = 0);
	~VariableNode();


	/*
	 * Variable Data
	 */
	private:
	// Variable that this node links to
	Variable* variable_;
	
	public:
	// Set variable target
	void setVariable(Variable* v);
	// Get variable target
	Variable* variable();
	// Return name of variable target
	QString name();


	/*
	 * Inherited Virtuals
	 */
	public:
	// Execute node
	bool execute(double& rv);
	// Print node contents
	void nodePrint(int offset, const char* prefix = "");
	// Set from returnvalue node
	bool set(double value);
	// Initialise node
	bool initialise();
};

#endif
