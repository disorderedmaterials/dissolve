/*
	*** Plain (old datatype) Value
	*** src/base/plainvalue.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_PLAINVALUE_H
#define DUQ_PLAINVALUE_H

#include "base/charstring.h"
#include "templates/listitem.h"

// Forward Declarations
class ProcessPool;

// PlainValue
class PlainValue : public ListItem<PlainValue>
{
	public:
	// Constructor
	PlainValue();
	// Destructor
	~PlainValue();
	// Constructor (bool)
	PlainValue(bool value);
	// Constructor (int)
	PlainValue(int value);
	// Constructor (double)
	PlainValue(double value);
	// Constructor (const char*)
	PlainValue(const char* value);
	// Constructor (CharString)
	PlainValue(const CharString& value);
	// Constructor (PlainValue)
	PlainValue(const PlainValue& value);
	// Assignment Operator
	void operator=(const PlainValue& value);
	// Value Type
	enum ValueType { BooleanType, IntegerType, DoubleType, StringType };
	// Return ValueType name
	static const char* valueType(ValueType vt);


	/*
	 * Value Storage
	 */
	private:
	// Name of value
	CharString name_;
	// Description of value, if any
	CharString description_;
	// Type of value
	ValueType type_;
	// Flags to apply if reinstated as a GenericListItem (i.e. in a Module)
	int genericItemFlags_;
	// Value (bool)
	bool valueB_;
	// Value (integer)
	int valueI_;
	// Value (double)
	double valueD_;
	// Value (string)
	CharString valueC_;

	public:
	// Setup value, including name and description
	void initialise(const char* name, PlainValue newValue, const char* description, int genericItemFlags);
	// Set value
	void set(const PlainValue& newValue);
	// Return name of value
	const char* name();
	// Return description for value
	const char* description();
	// Return type of value
	ValueType type();
	// Return flags to apply if reinstated as a GenericListItem (i.e. in a Module)
	int genericItemFlags();

	/*
	 * Conversion
	 */
	private:
	// Temporary working string
	CharString conversionStringTemp_;

	public:
	// Return value (as bool)
	bool asBool();
	// Return value (as int)
	int asInt();
	// Return value (as double)
	double asDouble();
	// Return value (as string)
	const char* asString();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data
	bool broadcast(ProcessPool& procPool);
};

#endif

