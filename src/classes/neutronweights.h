/*
    *** Neutron Weights Container
    *** src/classes/neutronweights.h
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

#include "classes/atomtypelist.h"
#include "classes/isotopologues.h"
#include "genericitems/base.h"
#include "templates/array2d.h"
#include "templates/list.h"
#include <vector>

// Forward Declarations
/* none */

// Neutron Weights Container
class NeutronWeights : public GenericItemBase
{
    public:
    NeutronWeights();
    NeutronWeights(const NeutronWeights &source);
    void operator=(const NeutronWeights &source);

    /*
     * Construction
     */
    private:
    // List of Isotopologues for Species
    std::vector<Isotopologues> isotopologueMixtures_;

    public:
    // Clear contents
    void clear();
    // Add Species Isotopologue to the relevant mixture
    void addIsotopologue(Species *sp, int speciesPopulation, const Isotopologue *iso, double isotopologueRelativePopulation);
    // Return whether an Isotopologues definition exists for the provided Species
    bool containsIsotopologues(Species *sp) const;
    // Print atomtype / weights information
    void print() const;

    /*
     * Data
     */
    private:
    // Type list derived from Species referenced in isotopologueMixtures_
    AtomTypeList atomTypes_;
    // Concentration product matrix (ci * cj)
    Array2D<double> concentrationProducts_;
    // Bound coherent product matrix (bi * bj)
    Array2D<double> boundCoherentProducts_;
    // Full scattering weights
    Array2D<double> weights_;
    // Intramolecular scattering weights
    Array2D<double> intramolecularWeights_;
    // Bound coherent average squared scattering (<b>**2)
    double boundCoherentSquareOfAverage_;
    // Bound coherent squared average scattering (<b**2>)
    double boundCoherentAverageOfSquares_;
    // Whether the structure is valid (i.e. has been finalised)
    bool valid_;

    private:
    // Calculate weighting matrices based on current AtomType / Isotope information
    void calculateWeightingMatrices();

    public:
    // Create AtomType list and matrices based on stored Isotopologues information
    void createFromIsotopologues(const AtomTypeList &exchangeableTypes);
    // Reduce data to be naturally-weighted
    void naturalise();
    // Return AtomTypeList
    AtomTypeList &atomTypes();
    // Return number of used AtomTypes
    int nUsedTypes() const;
    // Return concentration product for types i and j
    double concentrationProduct(int i, int j) const;
    // Return bound coherent scattering product for types i and j
    double boundCoherentProduct(int i, int j) const;
    // Return full weighting for types i and j (ci * cj * bi * bj * [2-dij])
    double weight(int i, int j) const;
    // Return full intramolecular weighting for types i and j
    double intramolecularWeight(int i, int j) const;
    // Return full scattering weights matrix
    const Array2D<double> &weights() const;
    // Return full intramolecular scattering weights matrix
    const Array2D<double> &intramolecularWeights() const;
    // Return bound coherent average squared scattering (<b>**2)
    double boundCoherentSquareOfAverage() const;
    // Return bound coherent squared average scattering (<b**2>)
    double boundCoherentAverageOfSquares() const;
    // Return whether the structure is valid (i.e. has been finalised)
    bool isValid() const;

    /*
     * GenericItemBase Implementations
     */
    public:
    // Return class name
    static std::string_view itemClassName();
    // Read data through specified LineParser
    bool read(LineParser &parser, CoreData &coreData);
    // Write data through specified LineParser
    bool write(LineParser &parser);

    /*
     * Parallel Comms
     */
    public:
    // Broadcast item contents
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check item equality
    bool equality(ProcessPool &procPool);
};
