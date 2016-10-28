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
	// Variable Type
	enum VariableType { IntegerType, DoubleType, StringType };


	/*
	 * Definition
	 */
	private:
	// Name of variable
	Dnchar name_;
	// Type of variable
	VariableType type_;
	// Integer value of variable (if integer type)

	public:
	// Setup variable (int)
	void setup(const char* name, int defaultValue);
	// Setup variable (double)
	void setup(const char* name, double defaultValue);
	// Setup variable (string)
	void setup(const char* name, const char* defaultValue);
	// Return name of variable
	const char* name();
	// Return type of variable
	VariableType type();


	/*
	 * Value
	 */
	private:
	// Temporary working string
	Dnchar conversionStringTemp_;
	// Value (integer)
	int valueI_;
	// Value (double)
	double valueD_;
	// Value (string)
	Dnchar valueC_;

	public:
	// Set variable value (int)
	bool set(int value);
	// Set variable value (double)
	bool set(double value);
	// Set variable value (string)
	bool set(const char* value);
	// Return variable (as int)
	int asInt();
	// Return variable (as double)
	double asDouble();
	// Return variable (as string)
	const char* asChar();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data
	bool broadcast(ProcessPool& procPool);
};

#endif

