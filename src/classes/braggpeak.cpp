/*
	*** BraggPeak
	*** src/classes/braggpeak.cpp
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

#include "classes/braggpeak.h"
#include "base/processpool.h"

// Constructor
BraggPeak::BraggPeak()
{
	q_ = 0.0;
	index_ = -1;
	nKVectors_ = 0;
}

// Destructor
BraggPeak::~BraggPeak()
{
}

// Copy constructor
BraggPeak::BraggPeak(const BraggPeak& source)
{
	(*this) = source;
}

// Operator=
void BraggPeak::operator=(const BraggPeak& source)
{
	q_ = source.q_;
	index_ = source.index_;
	intensities_ = source.intensities_;
	nKVectors_ = source.nKVectors_;
}

/*
 * Data
 */

// Initialise arrays
void BraggPeak::initialise(double q, int index, int nTypes)
{
	q_ = q;
	index_ = index;
	intensities_.initialise(nTypes, nTypes, true);
	nKVectors_ = 0;
}

// Return Q value
const double BraggPeak::q() const
{
	return q_;
}

// Set index 
void BraggPeak::setIndex(int index)
{
	index_ = index;
}

// Return index
const int BraggPeak::index() const
{
	return index_;
}

// Reset stored intensities
void BraggPeak::resetIntensities()
{
	intensities_ = 0.0;
	nKVectors_ = 0;
}

// Add intensity between specified atomtypes
void BraggPeak::addIntensity(int typeI, int typeJ, double intensity)
{
	intensities_.ref(typeI, typeJ) += intensity;
}

// Scale intensities between all atom types by factor provided
void BraggPeak::scaleIntensity(double factor)
{
// 	printf("1  Q = %f M = %i  ", q_, nKVectors_);
// 	for (int n=0; n<intensities_.linearArraySize(); ++n) printf("%f ", intensities_.linearArray()[n]);
// 	printf("\n");
	for (int n=0; n<intensities_.linearArraySize(); ++n) intensities_.linearArray()[n] *= factor;
// 	printf("2  Q = %f M = %i  ", q_, nKVectors_);
// 	for (int n=0; n<intensities_.linearArraySize(); ++n) printf("%f ", intensities_.linearArray()[n]);
// 	printf("\n");
}

// Return literal intensity between specified atom types for this peak
double BraggPeak::intensity(int typeI, int typeJ)
{
	return intensities_.value(typeI, typeJ);
}

// Increment number of k-vectors by specified amount
void BraggPeak::addKVectors(int count)
{
	nKVectors_ += count;
}

// Return number of k-vectors contributing to this peak
int BraggPeak::nKVectors()
{
	return nKVectors_;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool BraggPeak::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	if (!procPool.broadcast(q_, rootRank)) return false;
	if (!procPool.broadcast(index_, rootRank)) return false;
	if (!procPool.broadcast(nKVectors_, rootRank)) return false;
	if (!procPool.broadcast(intensities_, rootRank)) return false;
#endif
	return true;
}
