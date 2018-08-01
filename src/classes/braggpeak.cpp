/*
	*** BraggPeak
	*** src/classes/braggpeak.cpp
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
double BraggPeak::q() const
{
	return q_;
}

// Set index 
void BraggPeak::setIndex(int index)
{
	index_ = index;
}

// Return index
int BraggPeak::index() const
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
	intensities_.at(typeI, typeJ) += intensity;
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
	return intensities_.at(typeI, typeJ);
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
 * GenericItemBase Implementations
 */

// Return class name
const char* BraggPeak::itemClassName()
{
	return "BraggPeak";
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

// Check item equality
bool BraggPeak::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!procPool.equality(q_)) return Messenger::error("BraggPeak Q value is not equivalent (process %i has %e).\n", procPool.poolRank(), q_);
	if (!procPool.equality(index_)) return Messenger::error("BraggPeak index is not equivalent (process %i has %i).\n", procPool.poolRank(), index_);
	if (!procPool.equality(nKVectors_)) return Messenger::error("BraggPeak nKVectors is not equivalent (process %i has %i).\n", procPool.poolRank(), nKVectors_);
	if (!procPool.equality(intensities_)) return Messenger::error("BraggPeak intensities are not equivalent.\n");
#endif
	return true;
}
