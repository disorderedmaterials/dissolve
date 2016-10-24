/*
	*** Histogram Definition
	*** src/classes/histogram.cpp
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

#include "classes/histogram.h"
#include "base/comms.h"
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
	histogram_ = NULL;
	nAdded_ = 0;
	normalisationType_ = Histogram::NoNormalisation;
}

// Destructor
Histogram::~Histogram()
{
	clear();
}

// Copy Constructor
Histogram::Histogram(const Histogram& source)
{
	histogram_ = NULL;
	copy(source);
}

// Assignment Operator
void Histogram::operator=(const Histogram& source)
{
	copy(source);
}

// Clear data
void Histogram::clear()
{
	if (histogram_ != NULL) delete[] histogram_;
	histogram_ = NULL;
}

// Copy data
void Histogram::copy(const Histogram& source)
{
	clear();
	nBins_ = source.nBins_;
	binOffset_ = source.binOffset_;
	delta_ = source.delta_;
	rDelta_ = 1.0/delta_;
	histogram_ = new int[nBins_];
	for (int n=0; n<nBins_; ++n) histogram_[n] = source.histogram_[n];
	nAdded_ = source.nAdded_;
	normalisedData_ = source.normalisedData_;
	normalisationType_ = source.normalisationType_;
	normalisation_ = source.normalisation_;
}

/*
 * Data
 */

/*
 * \brief Initialise arrays
 * \details Initialise all values and arrays, ready for histogram calculation. Note that the normalisationType_ is reset to NoNormalisation by
 * this routine, and so the correct normalisation type must be set manually afterwards.
 */
void Histogram::initialise(double minValue, double maxValue, double binWidth)
{
	clear();
	
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
	histogram_ = new int[nBins_];
	normalisation_.initialise(nBins_);
	normalisation_ = 1.0;
	normalisationType_ = Histogram::NoNormalisation;
	normalisedData_.initialise(nBins_);
	nAdded_ = 0;
	nMissed_ = 0;

	// Set up X axis values
	for (int n=0; n<nBins_; ++n) normalisedData_.setX(n, (n+0.5)*delta_);
}

// Set relative normalisation
void Histogram::setRelativeNormalisation()
{
	normalisationType_ = Histogram::RelativeNormalisation;
}

// Set radial number density normalisation
void Histogram::setRadialNumberDensityNormalisation(double boxVolume, int nCentres, int nSurrounding, double factor, Data2D& boxNormalisation)
{
	normalisationType_ = Histogram::RadialNumberDensityNormalisation;
	double shellVolume, norm, r = 0.0, numberDensity = nSurrounding / boxVolume;
	for (int n=0; n<nBins_; ++n)
	{
		shellVolume = (4.0/3.0)*PI*(pow(r+delta_,3.0) - pow(r,3.0));
		norm = nCentres * (shellVolume * numberDensity);
		normalisation_[n] = (factor / norm) * boxNormalisation.interpolated(r+delta_*0.5);
		r += delta_;
	}
}

// Return number of bins
int Histogram::nBins() const
{
	return nBins_;
}

// Return histogram data
int* Histogram::histogram()
{
	return histogram_;
}

// Return normalised data
Data2D& Histogram::normalisedData()
{
	return normalisedData_;
}

// Add source histogram data into local array
void Histogram::addHistogramData(Histogram& otherHistogram, int factor)
{
	if (nBins_ != otherHistogram.nBins_)
	{
		msg.print("BAD_USAGE - Can't add histogram data since arrays are not the same size (%i vs %i).\n", nBins_, otherHistogram.nBins_);
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
	for (int n=0; n<nBins_; ++n)
	{
		normalisedData_.setY(n, 0.0);
		histogram_[n] = 0;
	}
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
		msg.print("BAD_NUMBER - Negative bin calculated in Histogram::add().\n");
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

// Merge histogram data with other RDF
void Histogram::merge(Histogram& otherHistogram)
{
	if (nBins_ != otherHistogram.nBins_)
	{
		msg.print("BAD_USAGE - Can't merge RDF data since arrays are not the same size.\n");
		return;
	}
	for (int n=0; n<nBins_; ++n)
	{
		histogram_[n] += otherHistogram.histogram_[n];
		otherHistogram.histogram_[n] = histogram_[n];
	}
}

/*
 * \brief Finalise data
 * \details Finalise the data by normalising according to the previously-selected method, creating the normalisedData_ array 
 * in the process. If not called, the normalisedData_ array will contain either garbage or old data.
 */
void Histogram::finalise()
{
	// Normalise according to specified method
	int n;
	switch (normalisationType_)
	{
		case (Histogram::NoNormalisation):
			for (n=0; n<nBins_; ++n) normalisedData_.setY(n, histogram_[n]);
			break;
		case (Histogram::RelativeNormalisation):
			for (n=0; n<nBins_; ++n) normalisedData_.setY(n, double(histogram_[n]) / nAdded_);
			break;
		case (Histogram::RadialNumberDensityNormalisation):
			for (n=0; n<nBins_; ++n) normalisedData_.setY(n, (histogram_[n] * normalisation_[n]));
			break;
	}
}

/*
 * Parallel Comms
 */

// Sum histogram data onto all processes
bool Histogram::allSum()
{
	if (!Comm.allSum(histogram_, nBins_)) return false;
	return true;
}
