// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "templates/array2d.h"
#include "templates/listitem.h"

// Forward Declarations
/* none */

// BraggReflection Class
class BraggReflection
{
    /*
     *  BraggReflection acts as a 'bin' for collecting contributions arising from a set of KVectors which occur at the same
     * Q value.
     */
    public:
    BraggReflection();
    ~BraggReflection();
    BraggReflection(const BraggReflection &source);
    // Operator=
    void operator=(const BraggReflection &source);
    // Operator+= (intensity addition)
    void operator+=(const BraggReflection &source);
    // Operator*= (intensity scaling)
    void operator*=(double factor);

    /*
     * Data
     */
    private:
    // Q-position of reflection
    double q_;
    // Integer index (derived from q_)
    int index_;
    // Intensity contributions from atomtype pairs
    Array2D<double> intensities_;
    // Number of k-vectors which contributed to the intensity (for normalisation)
    int nKVectors_;

    public:
    // Initialise arrays
    void initialise(double q, int index, int nTypes);
    // Return Q value of reflection
    double q() const;
    // Set index
    void setIndex(int index);
    // Return index
    int index() const;
    // Reset stored intensities
    void reset();
    // Add intensity between specified atomtypes from k-vector
    void addIntensity(int typeI, int typeJ, double intensity);
    // Scale intensities between all atom types by factor provided
    void scaleIntensities(double factor);
    // Scale intensity between all specific atom types by factor provided
    void scaleIntensity(int typeI, int typeJ, double factor);
    // Return intensity between specified atom types for this reflection
    double intensity(int typeI, int typeJ) const;
    // Increment number of contributing k-vectors
    void addKVectors(int count);
    // Return number of k-vectors contributing to this reflection
    int nKVectors() const;

    /*
     * Serialisation
     */
    public:
    // Read data through specified parser
    bool deserialise(LineParser &parser);
    // Write data through specified parser
    bool write(LineParser &parser);
};
