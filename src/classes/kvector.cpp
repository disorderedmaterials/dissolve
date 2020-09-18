/*
    *** KVector
    *** src/classes/kvector.cpp
    Copyright T. Youngs 2012-2020

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

#include "classes/kvector.h"
#include "base/processpool.h"
#include "classes/braggreflection.h"

KVector::KVector(int h, int k, int l, int reflectionIndex, int nAtomTypes)
{
    hkl_.set(h, k, l);
    braggReflectionIndex_ = reflectionIndex;

    // Create atomtype contribution arrays
    cosTerms_.initialise(nAtomTypes);
    sinTerms_.initialise(nAtomTypes);
}

KVector::~KVector() {}

KVector::KVector(const KVector &source) { (*this) = source; }

// Operator=
void KVector::operator=(const KVector &source)
{
    hkl_ = source.hkl_;
    braggReflectionIndex_ = source.braggReflectionIndex_;
    cosTerms_ = source.cosTerms_;
    sinTerms_ = source.sinTerms_;
}

/*
 * Data
 */

// Initialise
void KVector::initialise(int h, int k, int l, int reflectionIndex, int nAtomTypes)
{
    hkl_.set(h, k, l);
    braggReflectionIndex_ = reflectionIndex;

    // Create atomtype contribution arrays
    cosTerms_.initialise(nAtomTypes);
    sinTerms_.initialise(nAtomTypes);
}

// Return integer hkl indices
const Vec3<int> &KVector::hkl() const { return hkl_; }

// Return h index
int KVector::h() const { return hkl_.x; }

// Return k index
int KVector::k() const { return hkl_.y; }

// Return l index
int KVector::l() const { return hkl_.z; }

// Set BraggReflection index
void KVector::setBraggReflectionIndex(int index) { braggReflectionIndex_ = index; }

// Return associated BraggReflection index
int KVector::braggReflectionIndex() const { return braggReflectionIndex_; }

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
        Messenger::print("BAD_USAGE - KVector::cosTerms_ index supplied ({}) is greated than the size of the array ({}).\n",
                         atomTypeIndex, cosTerms_.nItems());
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
        Messenger::print("BAD_USAGE - KVector::sinTerms_ index supplied ({}) is greated than the size of the array ({}).\n",
                         atomTypeIndex, sinTerms_.nItems());
        return;
    }
#endif
    sinTerms_[atomTypeIndex] += value;
}

// Calculate intensities from stored cos and sin term arrays
void KVector::calculateIntensities(BraggReflection *reflectionArray)
{
#ifdef CHECKS
    if (reflectionArray == nullptr)
    {
        Messenger::print("NULL_POINTER - NULL BraggReflection array found in KVector::calculateIntensities().\n");
        return;
    }
#endif
    // Calculate final intensities from stored cos/sin terms
    // Take account of the half-sphere, doubling intensities of all k-vectors not on h == 0
    // Do *not* multiply cross-terms (i != j) by 2 - we want to generate the unmultiplied intensity for consistency with
    // other objects
    int i, j, nTypes = cosTerms_.nItems(), halfSphereNorm = (hkl_.x == 0 ? 1 : 2);
    double intensity;
    BraggReflection &braggReflection = reflectionArray[braggReflectionIndex_];
    braggReflection.addKVectors(halfSphereNorm);
    for (i = 0; i < nTypes; ++i)
    {
        for (j = i; j < nTypes; ++j)
        {
            intensity = (cosTerms_[i] * cosTerms_[j] + sinTerms_[i] * sinTerms_[j]);
            braggReflection.addIntensity(i, j, intensity * halfSphereNorm);
        }
    }
}

// Return specified intensity
double KVector::intensity(int typeI, int typeJ)
{
    return (cosTerms_[typeI] * cosTerms_[typeJ] + sinTerms_[typeI] * sinTerms_[typeJ]) * (hkl_.x == 0 ? 1 : 2);
}

/*
 * GenericItemBase Implementations
 */

// Return class name
std::string_view KVector::itemClassName() { return "KVector"; }

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool KVector::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    if (!procPool.broadcast(hkl_, root))
        return false;
    if (!procPool.broadcast(braggReflectionIndex_, root))
        return false;
    if (!procPool.broadcast(cosTerms_, root))
        return false;
    if (!procPool.broadcast(sinTerms_, root))
        return false;
#endif
    return true;
}

// Check item equality
bool KVector::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.equality(hkl_))
        return Messenger::error("KVector hkl value is not equivalent.\n");
    if (!procPool.equality(braggReflectionIndex_))
        return Messenger::error("KVector bragg reflection index is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                braggReflectionIndex_);
    if (!procPool.equality(cosTerms_))
        return Messenger::error("KVector cos terms are not equivalent.\n");
    if (!procPool.equality(sinTerms_))
        return Messenger::error("KVector sin terms are not equivalent.\n");
#endif
    return true;
}
