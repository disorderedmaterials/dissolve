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
	// Add/set Variable (int)
	void setVariable(const char* name, int value);
	// Add/set Variable (double)
	void setVariable(const char* name, double value);
	// Add/set Variable (string)
	void setVariable(const char* name, const char* value);
	// Retrieve named Variable (int)
	int variableAsInt(const char* name);
	// Retrieve named Variable (double)
	double variableAsDouble(const char* name);
	// Retrieve named Variable (string)
	const char* variableAsChar(const char* name);


	/*
	 * Search
	 */
	public:
	// Return named Variable
	Variable* variable(const char* name);
	// Return named Variable (with prefix)
	Variable* variable(const char* prefix, const char* name);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data to all processes
	bool broadcastVariables();
};

#endif
