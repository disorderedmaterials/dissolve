/*
	*** 3-Dimensional Histogram
	*** src/math/histogram3d.h
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

#ifndef DISSOLVE_HISTOGRAM3D_H
#define DISSOLVE_HISTOGRAM3D_H

#include "math/data3d.h"
#include "math/sampleddouble.h"
#include "base/genericitembase.h"
#include "templates/array3d.h"
#include "templates/objectstore.h"

// Forward Declarations
class ProcessPool;

// One-Dimensional Histogram
class Histogram3D : public ListItem<Histogram3D>, public ObjectStore<Histogram3D>, public GenericItemBase
{
	public:
	// Constructor
	Histogram3D();
	// Destructor
	~Histogram3D();
	// Copy Constructor
	Histogram3D(const Histogram3D& source);
	// Clear data
	void clear();
	

	/*
	 * Histogram Data
	 */
	private:
	// Minimum x value for data (hard left-edge of first bin along x)
	Vec3<double> minima_;
	// Maximum value for data (hard right-edge of last bin along x, adjusted to match bin width if necessary)
	Vec3<double> maxima_;
	// Bin widths
	Vec3<double> binWidths_;
	// Number of bins
	Vec3<int> nBins_;
	// Histogram bins
	Array3D<long int> bins_;
	// Array of bin centres along x
	Array<double> xBinCentres_;
	// Array of bin centres along y
	Array<double> yBinCentres_;
	// Array of bin centres along z
	Array<double> zBinCentres_;
	// Accumulated averages
	Array3D<SampledDouble> averages_;
	// Number of values binned over all bins
	long int nBinned_;
	// Number of points missed (out of bin range)
	long int nMissed_;
	// Accumulated data
	Data3D accumulatedData_;

	private:
	// Update accumulated data
	void updateAccumulatedData();

	public:
	// Initialise with specified bin range
	void initialise(double minimum, double maximum, double binWidth);
	// Zero histogram bins
	void zeroBins();
	// Return minimum value for data (hard left-edge of first bin)
	double minimum() const;
	// Return maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
	double maximum() const;
	// Return bin width
	double binWidth() const;
	// Return number of bins
	int nBins() const;
	// Bin specified value
	void bin(double x);
	// Accumulate current histogram bins into averages
	void accumulate();
	// Return Array of x centre-bin values
	const Array<double>& binCentres() const;
	// Return histogram data
	Array<long int>& bins();
	// Add source histogram data into local array
	void add(Histogram3D& other, int factor = 1);
	// Return accumulated (averaged) data
	const Data3D& accumulatedData() const;


	/*
	 * Operators
	 */
	public:
	// Assignment Operator
	void operator=(const Histogram3D& source);


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
