/*
	*** KVector
	*** src/classes/kvector.cpp
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

#include "classes/kvector.h"
#include "classes/braggpeak.h"
#include "base/processpool.h"

// Constructor
KVector::KVector(int h, int k, int l, int peakIndex, int nAtomTypes) : MPIListItem<KVector>()
{
	hkl_.set(h, k, l);
	braggPeakIndex_ = peakIndex;

	// Create atomtype contribution arrays
	cosTerms_.initialise(nAtomTypes);
	sinTerms_.initialise(nAtomTypes);
}

// Destructor
KVector::~KVector()
{
}

// Copy constructor
KVector::KVector(const KVector& source)
{
	(*this) = source;
}

// Operator=
void KVector::operator=(const KVector& source)
{
	hkl_ = source.hkl_;
	braggPeakIndex_ = source.braggPeakIndex_;
	cosTerms_ = source.cosTerms_;
	sinTerms_ = source.sinTerms_;
}

/*
 * Data
 */

// Initialise
void KVector::initialise(int h, int k, int l, int peakIndex, int nAtomTypes)
{
	hkl_.set(h, k, l);
	braggPeakIndex_ = peakIndex;

	// Create atomtype contribution arrays
	cosTerms_.initialise(nAtomTypes);
	sinTerms_.initialise(nAtomTypes);
}

// Return integer hkl indices
const Vec3<int>& KVector::hkl() const
{
	return hkl_;
}

// Return h index
const int KVector::h() const
{
	return hkl_.x;
}

// Return k index
const int KVector::k() const
{
	return hkl_.y;
}

// Return l index
const int KVector::l() const
{
	return hkl_.z;
}

// Set BraggPeak index 
void KVector::setBraggPeakIndex(int index)
{
	braggPeakIndex_ = index;
}

// Return associated BraggPeak index
int KVector::braggPeakIndex() const
{
	return braggPeakIndex_;
}

// Zero cos/sin term arrays
void KVector::zeroCosSinTerms()
{
	cosTerms_ = 0.0;
	sinTerms_ = 0.0;
}

// Add value to cosTerm index specified
void KVector::addCosTerm(int atomTypeIndex, double value)
{
#ifdef CHECKS
	if (atomTypeIndex >= cosTerms_.nItems())
	{
		Messenger::print("BAD_USAGE - KVector::cosTerms_ index supplied (%i) is greated than the size of the array (%i).\n", atomTypeIndex, cosTerms_.nItems());
		return;
	}
#endif
	cosTerms_[atomTypeIndex] += value;
}

// Add value to sinTerm index specified
void KVector::addSinTerm(int atomTypeIndex, double value)
{
#ifdef CHECKS
	if (atomTypeIndex >= sinTerms_.nItems())
	{
		Messenger::print("BAD_USAGE - KVector::sinTerms_ index supplied (%i) is greated than the size of the array (%i).\n", atomTypeIndex, sinTerms_.nItems());
		return;
	}
#endif
	sinTerms_[atomTypeIndex] += value;
}

// Calculate intensities from stored cos and sin term arrays
void KVector::calculateIntensities(BraggPeak* peakArray)
{
#ifdef CHECKS
	if (peakArray == NULL)
	{
		Messenger::print("NULL_POINTER - NULL braggPeak array found in KVector::calculateIntensities().\n");
		return;
	}
#endif
	// Calculate final intensities from stored cos/sin terms
	// Take account of the half-sphere, doubling intensities of all k-vectors not on h == 0
	int i, j, nTypes = cosTerms_.nItems(), halfSphereNorm = (hkl_.x == 0 ? 1 : 2);
	double intensity;
	BraggPeak& braggPeak = peakArray[braggPeakIndex_];
	braggPeak.addKVectors(halfSphereNorm);
	for (i = 0; i<nTypes; ++i)
	{
		for (j = i; j < nTypes; ++j)
		{
			intensity = (cosTerms_[i]*cosTerms_[j] + sinTerms_[i]*sinTerms_[j]);
			braggPeak.addIntensity(i, j, intensity * halfSphereNorm);
		}
	}
}

// Return specified intensity
double KVector::intensity(int typeI, int typeJ)
{
	return (cosTerms_[typeI]*cosTerms_[typeJ] + sinTerms_[typeI]*sinTerms_[typeJ]) * (hkl_.x == 0 ? 1 : 2);
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool KVector::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	if (!procPool.broadcast(hkl_, rootRank)) return false;
	if (!procPool.broadcast(braggPeakIndex_, rootRank)) return false;
	if (!procPool.broadcast(cosTerms_, rootRank)) return false;
	if (!procPool.broadcast(sinTerms_, rootRank)) return false;
#endif
	return true;
}
