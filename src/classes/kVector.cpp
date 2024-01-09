// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/kVector.h"
#include "classes/braggReflection.h"
#include "templates/algorithms.h"

KVector::KVector(int h, int k, int l, int reflectionIndex, int nAtomTypes) { initialise(h, k, l, reflectionIndex, nAtomTypes); }

KVector::KVector(const KVector &source) { (*this) = source; }

KVector::KVector(KVector &&source) noexcept { (*this) = source; }

KVector::KVector(const KVector &&source) noexcept { (*this) = source; }

KVector &KVector::operator=(const KVector &source) = default;

/*
 * Data
 */

// Initialise
void KVector::initialise(int h, int k, int l, int reflectionIndex, int nAtomTypes)
{
    hkl_.set(h, k, l);
    braggReflectionIndex_ = reflectionIndex;

    // Create atomtype contribution arrays
    cosTerms_.resize(nAtomTypes, 0);
    sinTerms_.resize(nAtomTypes, 0);
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
    std::fill(cosTerms_.begin(), cosTerms_.end(), 0.0);
    std::fill(sinTerms_.begin(), sinTerms_.end(), 0.0);
}

// Add value to cosTerm index specified
void KVector::addCosTerm(int atomTypeIndex, double value) { cosTerms_[atomTypeIndex] += value; }

// Add value to sinTerm index specified
void KVector::addSinTerm(int atomTypeIndex, double value) { sinTerms_[atomTypeIndex] += value; }

// Calculate intensities from stored cos and sin term arrays
void KVector::calculateIntensities(std::vector<BraggReflection> &reflections)
{
    // Calculate final intensities from stored cos/sin terms
    // Take account of the half-sphere, doubling intensities of all k-vectors not on h == 0
    // Do *not* multiply cross-terms (i != j) by 2 - we want to generate the un-multiplied intensity for consistency with
    // other objects
    auto halfSphereNorm = (hkl_.x == 0 ? 1 : 2);
    auto &braggReflection = reflections[braggReflectionIndex_];
    braggReflection.addKVectors(halfSphereNorm);
    dissolve::for_each_pair(
        ParallelPolicies::par, 0, cosTerms_.size(),
        [&](auto i, auto j)
        { braggReflection.addIntensity(i, j, (cosTerms_[i] * cosTerms_[j] + sinTerms_[i] * sinTerms_[j]) * halfSphereNorm); });
}

// Return specified intensity
double KVector::intensity(int typeI, int typeJ)
{
    return (cosTerms_[typeI] * cosTerms_[typeJ] + sinTerms_[typeI] * sinTerms_[typeJ]) * (hkl_.x == 0 ? 1 : 2);
}
