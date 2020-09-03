/*
    *** Bragg Reflection
    *** src/classes/braggreflection.h
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

#pragma once

#include "base/lineparser.h"
#include "genericitems/base.h"
#include "templates/array2d.h"
#include "templates/listitem.h"

// Forward Declarations
/* none */

// BraggReflection Class
class BraggReflection : public GenericItemBase
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
     * GenericItemBase Implementations
     */
    public:
    // Return class name
    static std::string_view itemClassName();
    // Read data through specified parser
    bool read(LineParser &parser, CoreData &coreData);
    // Write data through specified parser
    bool write(LineParser &parser);

    /*
     * Parallel Comms
     */
    public:
    // Broadcast data from root to all other processes
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check item equality
    bool equality(ProcessPool &procPool);
};
