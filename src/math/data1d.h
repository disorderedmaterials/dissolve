/*
	*** 1-Dimensional Data With Statistics
	*** src/math/data1d.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_DATA1D_H
#define DISSOLVE_DATA1D_H

#include "math/sampleddouble.h"
#include "templates/array.h"
#include "templates/objectstore.h"

// Forward Declarations
class Histogram1D;

// One-Dimensional Data
class Data1D : public ListItem<Data1D>, public ObjectStore<Data1D>, public GenericItemBase
{
	public:
	// Constructor
	Data1D();
	// Destructor
	~Data1D();
	// Copy Constructor
	Data1D(const Data1D& source);
	// Clear data
	void clear();
	

	/*
	 * Data
	 */
	private:
	// X axis array
	Array<double> xAxis_;
	// Accumulated values along x
	Array<SampledDouble> values_;

	public:
	// Initialise to be consistent with supplied Histogram1D object
	void initialise(const Histogram1D& source);
	// Zero values array
	void zero();
	// Accumulate specified histogram data
	void accumulate(const Histogram1D& source);
	// Return number of values along x
	int nX() const;
	// Return x axis Array
	const Array<double>& xAxis() const;
	// Return Array of accumulated x values
	const Array<SampledDouble>& values() const;


	/*
	 * Operators
	 */
	public:
	// Assignment Operator
	void operator=(const Data1D& source);


	/*
	 * GenericItemBase Implementations
	 */
	public:
	// Return class name
	static const char* itemClassName();
	// Write data through specified LineParser
	bool write(LineParser& parser);
	// Read data through specified LineParser
	bool read(LineParser& parser);


	/*
	 * Parallel Comms
	 */
	public:
	// Sum histogram data onto all processes
	bool allSum(ProcessPool& procPool);
	// Broadcast data
	bool broadcast(ProcessPool& procPool, int rootRank = 0);
	// Check item equality
	bool equality(ProcessPool& procPool);
};

#endif
