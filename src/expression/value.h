/*
	*** Expression Value Node
	*** src/expression/value.h
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

#ifndef DISSOLVE_EXPRESSIONVALUE_H
#define DISSOLVE_EXPRESSIONVALUE_H

#include "expression/node.h"

// Forward Declarations
class ExpressionVariable;

// Value Node (retrieves value of an ExpressionVariable)
class ExpressionValue : public ExpressionNode
{
	public:
	// Constructor / Destructor
	ExpressionValue(ExpressionVariable* var = 0);
	~ExpressionValue();


	/*
	 * Variable Data
	 */
	private:
	// Variable that this node links to
	ExpressionVariable* variable_;
	
	public:
	// Set variable target
	void setVariable(ExpressionVariable* v);
	// Get variable target
	ExpressionVariable* variable() const;
	// Return name of variable target
	const char* name() const;


	/*
	 * Inherited Virtuals
	 */
	public:
	// Execute node
	bool execute(double& rv);
	// Print node contents
	void nodePrint(int offset, const char* prefix = "");
	// Set from double value
	bool set(double value);
	// Initialise node
	bool initialise();
};

#endif
