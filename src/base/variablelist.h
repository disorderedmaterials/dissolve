/*
	*** Variable List
	*** src/base/variablelist.h
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

#ifndef DUQ_VARIABLELIST_H
#define DUQ_VARIABLELIST_H

#include "base/variable.h"

// Forward Declarations
/* none */

// VariableList
class VariableList
{
	public:
	// Constructor
	VariableList();
	// Destructor
	~VariableList();


	/*
	 * Variables
	 */
	private:
	// List of contained Variables
	List<Variable> variables_;

	public:
	// Return first Variable in list
	Variable* variables();
	// Add/set Variable
	void setVariable(const char* name, VariableValue value, const char* description = "", const char* source = "");
	// Retrieve named Variable (bool)
	bool variableAsBool(const char* name, const char* source = "");
	// Retrieve named Variable (int)
	int variableAsInt(const char* name, const char* source = "");
	// Retrieve named Variable (double)
	double variableAsDouble(const char* name, const char* source = "");
	// Retrieve named Variable (string)
	const char* variableAsChar(const char* name, const char* source = "");


	/*
	 * Search
	 */
	public:
	// Return named Variable
	Variable* variable(const char* name, const char* source = "");


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data
	bool broadcast(ProcessPool& procPool);
};

#endif
