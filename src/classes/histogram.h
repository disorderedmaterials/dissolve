/*
	*** Histogram
	*** src/classes/histogram.h
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

#ifndef DUQ_HISTOGRAM_H
#define DUQ_HISTOGRAM_H

#include "base/data2d.h"

// Forward Declarations
class Box;

/*
 * \brief Histogram
 * \details XXX TODO
 */
class Histogram : public ListItem<Histogram>
{
	public:
	// Constructor
	Histogram();
	// Destructor
	~Histogram();
	// Copy Constructor
	Histogram(const Histogram& source);
	// Assignment Operator
	void operator=(const Histogram& source);
	// Clear data
	void clear();
	// Normalisation Method
	enum NormalisationType
	{
		NoNormalisation,			/* No normalisation of histogram - just copy bin values (no params) */
		RelativeNormalisation,			/* Divide through by total number of counts (no params) */
		RadialNumberDensityNormalisation,	/* Divide through by radial number density (boxVolume, nCentres, nSurrounding, factor) */
	};


	/*
	 * Data
	 */
	protected:
	// Histogram value range
	double minimum_, maximum_, range_;
	// Bin width and reciprocal
	double delta_, rDelta_;
	// Bin offset 
	int binOffset_;
	// Number of points in histogram
	int nBins_;
	// Histogram data
	int* histogram_;
	// Normalisation method
	NormalisationType normalisationType_;
	// Normalisation array
	Array<double> normalisation_;
	// Normalised data
	Data2D normalisedData_;
	// Number of points added
	long int nAdded_;
	// Number of points missed (out of bin range)
	long int nMissed_;
	
	private:
	// Copy
	void copy(const Histogram& source);
	
	public:
	// Initialise arrays
	void initialise(double minValue, double maxValue, double binWidth);
	// Set relative normalisation
	void setRelativeNormalisation();
	// Set radial number density normalisation
	void setRadialNumberDensityNormalisation(double boxVolume, int nCentres, int nSurrounding, double factor, Data2D& boxNormalisation);
	// Return number of bins
	int nBins() const;
	// Return histogram data
	int* histogram();
	// Return normalised data
	Data2D& normalisedData();
	// Add source histogram data into local array
	void addHistogramData(Histogram& otherHistogram, int factor = 1);


	/*
	 * Calculation
	 */
	public:
	// Reset arrays ready for new calculation
	void reset();
	// Add one to bin of corresponding value
	void add(double x);
	// Merge (add) histogram data
	void merge(Histogram& otherHistogram);
	// Finalise data
	void finalise();


	/*
	 * Parallel Comms
	 */
	public:
	// Gather all data to target process
	bool allSum();
};

#endif
