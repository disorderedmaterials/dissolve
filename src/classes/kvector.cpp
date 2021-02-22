// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
void KVector::addCosTerm(int atomTypeIndex, double value) { cosTerms_[atomTypeIndex] += value; }

// Add value to sinTerm index specified
void KVector::addSinTerm(int atomTypeIndex, double value) { sinTerms_[atomTypeIndex] += value; }

// Calculate intensities from stored cos and sin term arrays
void KVector::calculateIntensities(BraggReflection *reflectionArray)
{
    assert(reflectionArray);

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
