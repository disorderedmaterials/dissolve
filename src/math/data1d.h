/*
	*** 1-Dimensional Binned Data With Statistics
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

#include "base/charstring.h"
#include "math/sampleddouble.h"
#include "templates/array.h"
#include "templates/objectstore.h"

// Forward Declarations
class ProcessPool;

// Data1D
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
	// Minimum x value for data (hard left-edge of first bin)
	double xMin_;
	// Maximum x value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
	double xMax_;
	// Bin width
	double binWidth_;
	// Number of bins
	int nBins_;
	// Array of y values
	Array<SampledDouble> y_;
	// Number of values sampled over entire y array
	int nSamples_;
	// Normalisation factor
	double normalisationFactor_;

	public:
	// Initialise with specified bin range
	void initialise(double xMin, double xMax, double binWidth);
	// Reset all accumulated data
	void reset();
	// Return minimum x value
	double xMin() const;
	// Return maximum x value
	double xMax() const;
	// Return bin width
	double binWidth() const;
	// Return number of bins
	int nBins() const;
	// Accumulate value at specified x
	void add(double x, double y);
	// Return Array of x centre-bin values
	const Array<double>& x() const;
	// Return y Array
	Array<SampledDouble>& y();
	// Return y Array (const)
	const Array<SampledDouble>& constY() const;


	/*
	 * Operators
	 */
	public:
	// Assignment Operator
	void operator=(const Data1D& source);
	// Operator *=
	void operator*=(const double factor);
	// Operator /=
	void operator/=(const double factor);


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
	// Broadcast data
	bool broadcast(ProcessPool& procPool, int rootRank = 0);
	// Check item equality
	bool equality(ProcessPool& procPool);
};

#endif
