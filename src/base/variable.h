/*
	*** Variable
	*** src/base/variable.h
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

#ifndef DUQ_VARIABLE_H
#define DUQ_VARIABLE_H

#include "base/variablevalue.h"
#include "base/dnchar.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// Variable
class Variable : public ListItem<Variable>
{
	public:
	// Constructor
	Variable();
	// Destructor
	~Variable();


	/*
	 * Definition
	 */
	private:
	// Name of variable
	Dnchar name_;
	// Source of variable
	Dnchar source_;
	// Value
	VariableValue value_;
	// Description
	Dnchar description_;

	public:
	// Setup variable
	void setup(const char* name, VariableValue defaultValue, const char* description, const char* source = "");
	// Return name of variable
	const char* name();
	// Return source of variable
	const char* source();
	// Return type of data currently stored in variable
	VariableValue::ValueType type();
	// Return description
	const char* description();


	/*
	 * Conversion / Retrieval
	 */
	private:
	// Temporary working string
	Dnchar conversionStringTemp_;

	public:
	// Set variable value
	void set(VariableValue newValue);
	// Return value (as bool)
	bool asBool();
	// Return value (as int)
	int asInt();
	// Return value (as double)
	double asDouble();
	// Return value (as string)
	const char* asChar();
	// Return value (as Array<int>)
	Array<int>& asIntArray();
	// Return value (as Array<double>)
	Array<double>& asDoubleArray();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data
	bool broadcast(ProcessPool& procPool);
};

#endif

