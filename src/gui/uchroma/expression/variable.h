/*
	*** Expression Variable
	*** src/gui/uchroma/expression/variable.h
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

#ifndef DISSOLVE_UCHROMAVARIABLE_H
#define DISSOLVE_UCHROMAVARIABLE_H

#include "gui/uchroma/expression/node.h"
#include "base/charstring.h"

// Variable
class Variable : public Node
{
	public:
	// Constructor / Destructor
	Variable(double value = 0.0, bool readOnly = false);
	~Variable();


	/*
	 * Variable Data
	 */
	protected:
	// Name of the variable
	CharString name_;
	// Value of variable
	double value_;
	// Initial value of new variable
	Node* initialValue_;

	public:
	// Set name of variable
	void setName(const char* s);
	// Get name of variable
	const char* name() const;
	// Set initial value expression
	bool setInitialValue(Node* node);
	// Return Node corresponding to initial value
	Node* initialValue() const;


	/*
	 * Set / Get
	 */
	public:
	// Return value of node
	bool execute(double& rv);
	// Set value of node
	bool set(double rv);


	/*
	 * Inherited Virtuals
	 */
	public:
	// Print node contents
	void nodePrint(int offset, const char* prefix = "");
	// Initialise node
	bool initialise();
};

#endif
