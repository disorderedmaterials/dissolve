// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "templates/array2d.h"

// BraggReflection Class
class BraggReflection
{
    public:
    BraggReflection();
    ~BraggReflection() = default;
    BraggReflection(const BraggReflection &source) = default;
    BraggReflection(BraggReflection &&source) = default;
    BraggReflection &operator=(const BraggReflection &source) = default;
    void operator+=(const BraggReflection &source);
    void operator*=(double factor);
    BraggReflection operator*(double factor) const;

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
    // Scale intensity between specified atom types by factor provided
    void scaleIntensity(int typeI, int typeJ, double factor);
    // Return intensities array
    const Array2D<double> intensities() const;
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
    bool serialise(LineParser &parser) const;
};
