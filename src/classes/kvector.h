// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/array.h"
#include "templates/listitem.h"
#include "templates/vector3.h"

// Forward Declarations
class BraggReflection;
class CoreData;
class ProcessPool;

// K-Vector
class KVector
{
    public:
    KVector(int h = 0, int k = 0, int l = 0, int reflectionIndex = -1, int nAtomTypes = 0);
    ~KVector();
    KVector(const KVector &source);
    // Operator=
    void operator=(const KVector &source);

    /*
     * Data
     */
    private:
    // Integer hkl indices of vector
    Vec3<int> hkl_;
    // Associated BraggReflection index
    int braggReflectionIndex_;
    // Contributions to this kvector from individual atom types
    Array<double> cosTerms_, sinTerms_;

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
    // Calculate intensities and sum into associated BraggReflection
    void calculateIntensities(BraggReflection *reflectionArray);
    // Return specified intensity
    double intensity(int typeI, int typeJ);

    /*
     * Serialisation
     */
    public:
    /*
     * Parallel Comms
     */
    public:
    // Broadcast data from root to all other processes
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check item equality
    bool equality(ProcessPool &procPool);
};
