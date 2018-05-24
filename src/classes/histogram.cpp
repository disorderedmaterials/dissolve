/*
	*** Histogram Definition
	*** src/classes/histogram.cpp
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

#include "classes/histogram.h"
#include "base/processpool.h"
#include "base/lineparser.h"
#include "math/constants.h"
#include <math.h>

// Constructor
Histogram::Histogram() : ListItem<Histogram>()
{
	nBins_ = 0;
	binOffset_ = 0;
	delta_ = 0.1;
	rDelta_ = 1.0/delta_;
	nAdded_ = 0;
}

// Destructor
Histogram::~Histogram()
{
}

// Copy Constructor
Histogram::Histogram(const Histogram& source)
{
	copy(source);
}

// Assignment Operator
void Histogram::operator=(const Histogram& source)
{
	copy(source);
}

// Copy data
void Histogram::copy(const Histogram& source)
{
	nBins_ = source.nBins_;
	binOffset_ = source.binOffset_;
	delta_ = source.delta_;
	rDelta_ = 1.0/delta_;
	histogram_ = source.histogram_;
	nAdded_ = source.nAdded_;
}

/*
 * Data
 */

// Initialise arrays
void Histogram::initialise(double minValue, double maxValue, double binWidth)
{
	/*
	 * Initialise all values and arrays, ready for histogram calculation. Note that the normalisationType_ is reset to NoNormalisation by
	 * this routine, and so the correct normalisation type must be set manually afterwards.
	 */

	// Determine number of points from range and binwidth
	maximum_ = maxValue;
	minimum_ = minValue;
	range_ = maxValue - minValue;
	delta_ = binWidth;
	rDelta_ = 1.0 / delta_;

	// Set nBins and offset
	nBins_ = range_ / delta_;
	binOffset_ = -minimum_/delta_;

	// Create arrays
	histogram_.initialise(nBins_);
	bins_.initialise(nBins_);
	nAdded_ = 0;
	nMissed_ = 0;

	// Set up bin values
	for (int n=0; n<nBins_; ++n) bins_[n] = (n+0.5)*delta_;
}

// Return number of bins
int Histogram::nBins() const
{
	return nBins_;
}

// Return spacing between bins
double Histogram::delta() const
{
	return delta_;
}

// Return histogram data
Array<int>& Histogram::histogram()
{
	return histogram_;
}

// Return bin data
Array<double>& Histogram::bins()
{
	return bins_;
}

// Add source histogram data into local array
void Histogram::addHistogramData(Histogram& otherHistogram, int factor)
{
	if (nBins_ != otherHistogram.nBins_)
	{
		Messenger::print("BAD_USAGE - Can't add histogram data since arrays are not the same size (%i vs %i).\n", nBins_, otherHistogram.nBins_);
		return;
	}
	for (int n=0; n<nBins_; ++n) histogram_[n] += otherHistogram.histogram_[n] * factor;
}

/*
 * Calculation
 */

// Reset arrays ready for new calculation
void Histogram::reset()
{
	histogram_ = 0;
	nAdded_ = 0;
	nMissed_ = 0;
}

// Add distance specified to histogram
void Histogram::add(double x)
{
	int bin = x * rDelta_ - binOffset_;
#ifdef CHECKS
	if (bin < 0)
	{
		Messenger::print("BAD_NUMBER - Negative bin calculated in Histogram::add().\n");
		return;
	}
#endif
	if (bin < nBins_)
	{
		++histogram_[bin];
		++nAdded_;
	}
	else ++nMissed_;
}

// Normalise data, giving relative histogram
Array<double> Histogram::normalised()
{
	Array<double> normalised(nBins_);

	for (int n=0; n<nBins_; ++n) normalised[n] = double(histogram_[n]) / nAdded_;

	return normalised;
}

/*
 * Parallel Comms
 */

// Sum histogram data onto all processes
bool Histogram::allSum(ProcessPool& procPool)
{
	if (!procPool.allSum(histogram_, nBins_)) return false;
	return true;
}

// Broadcast data over pool processes
bool Histogram::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	if (!procPool.broadcast(nBins_, rootRank)) return false;
	if (!procPool.broadcast(binOffset_, rootRank)) return false;
	if (!procPool.broadcast(delta_, rootRank)) return false;
	if (!procPool.broadcast(rDelta_, rootRank)) return false;
	if (!procPool.broadcast(histogram_, rootRank)) return false;
	if (!procPool.broadcast(nAdded_, rootRank)) return false;
#endif
	return true;
}
