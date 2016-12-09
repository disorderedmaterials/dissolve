/*
	*** VariableValue
	*** src/base/variablevalue.h
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

#ifndef DUQ_VARIABLEVALUE_H
#define DUQ_VARIABLEVALUE_H

#include "base/dnchar.h"
#include "templates/array.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// VariableValue
class VariableValue
{
	public:
	// Constructor
	VariableValue();
	// Destructor
	~VariableValue();
	// Constructor (bool)
	VariableValue(bool value);
	// Constructor (int)
	VariableValue(int value);
	// Constructor (double)
	VariableValue(double value);
	// Constructor (const char*)
	VariableValue(const char* value);
	// Constructor (Dnchar)
	VariableValue(const Dnchar& value);
	// Constructor (Array<int>)
	VariableValue(const Array<int>& value);
	// Constructor (Array<double>)
	VariableValue(const Array<double>& value);
	// Constructor (VariableValue)
	VariableValue(const VariableValue& value);
	// Assignment Operator
	void operator=(const VariableValue& value);
	// Value Type
	enum ValueType { BooleanType, IntegerType, DoubleType, CharType, IntegerArrayType, DoubleArrayType };
	// Return ValueType name
	static const char* valueType(ValueType vt);


	/*
	 * Value Storage
	 */
	private:
	// Type of variable
	ValueType type_;
	// Value (bool)
	bool valueB_;
	// Value (integer)
	int valueI_;
	// Value (double)
	double valueD_;
	// Value (string)
	Dnchar valueC_;
	// Value (integer array)
	Array<int> arrayI_;
	// Value (double array)
	Array<double> arrayD_;

	public:
	// Return type of variable
	ValueType type();


	/*
	 * Conversion
	 */
	private:
	// Temporary working string
	Dnchar conversionStringTemp_;

	public:
	// Set variable value
	bool set(VariableValue newValue);
	// Return variable (as bool)
	bool asBool();
	// Return variable (as int)
	int asInt();
	// Return variable (as double)
	double asDouble();
	// Return variable (as string)
	const char* asChar();
	// Return variable (as integer array)
	Array<int>& asIntArray();
	// Return variable (as double array)
	Array<double>& asDoubleArray();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data
	bool broadcast(ProcessPool& procPool);
};

#endif

