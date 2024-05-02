// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/vector3.h"
#include <vector>

// Forward Declarations
class BraggReflection;
class CoreData;

// K-Vector
class KVector
{
    public:
    KVector(int h = 0, int k = 0, int l = 0, int reflectionIndex = -1, int nAtomTypes = 0);
    ~KVector() = default;
    KVector(const KVector &source);
    KVector(KVector &&source) noexcept;
    KVector(const KVector &&source) noexcept;
    KVector &operator=(const KVector &source);

    /*
     * Data
     */
    private:
    // Integer hkl indices of vector
    Vec3<int> hkl_;
    // Associated BraggReflection index
    int braggReflectionIndex_;
    // Contributions to this k-vector from individual atom types
    std::vector<double> cosTerms_, sinTerms_;

    public:
    // Initialise
    void initialise(int h, int k, int l, int reflectionIndex, int nAtomTypes);
    // Return hkl indices
    const Vec3<int> &hkl() const;
    // Return h index
    int h() const;
    // Return k index
    int k() const;
    // Return l index
    int l() const;
    // Set BraggReflection index
    void setBraggReflectionIndex(int index);
    // Return associated BraggReflection index
    int braggReflectionIndex() const;
    // Zero cos/sin term arrays
    void zeroCosSinTerms();
    // Add value to cosTerm index specified
    void addCosTerm(int atomTypeIndex, double value);
    // Add value to sinTerm index specified
    void addSinTerm(int atomTypeIndex, double value);
    // Calculate intensities and sum into associated BraggReflections
    void calculateIntensities(std::vector<BraggReflection> &reflections);
    // Return specified intensity
    double intensity(int typeI, int typeJ);
};
