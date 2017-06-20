/*
	*** Plain Value List
	*** src/base/plainvaluelist.h
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

#ifndef DUQ_PLAINVALUELIST_H
#define DUQ_PLAINVALUELIST_H

#include "base/plainvalue.h"

// Forward Declarations
/* none */

// Plain Value List
class PlainValueList
{
	public:
	// Constructor
	PlainValueList();
	// Destructor
	~PlainValueList();


	/*
	 * Values
	 */
	private:
	// List of contained Values
	List<PlainValue> values_;

	public:
	// Return first Value in list
	PlainValue* values();
	// Add Value
	void add(const char* name, PlainValue value, const char* description = "", int genericItemFlags = -1);
	// Add/Append Value
	void appendValue(const char* name, PlainValue value, const char* description = "", int genericItemFlags = -1);
	// Retrieve named Value (bool)
	bool valueAsBool(const char* name);
	// Retrieve named Value (int)
	int valueAsInt(const char* name);
	// Retrieve named Value (double)
	double valueAsDouble(const char* name);
	// Retrieve named Value (string)
	const char* valueAsString(const char* name);


	/*
	 * Search
	 */
	public:
	// Return named Value
	PlainValue* value(const char* name);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data
	bool broadcast(ProcessPool& procPool);
};

#endif
