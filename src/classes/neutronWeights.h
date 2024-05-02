// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/atomTypeMix.h"
#include "classes/isotopologues.h"
#include "templates/array2D.h"
#include <vector>

// Neutron Weights Container
class NeutronWeights
{
    public:
    NeutronWeights();
    NeutronWeights(const NeutronWeights &source);
    void operator=(const NeutronWeights &source);

    /*
     * Construction
     */
    private:
    // Vector of Isotopologues for Species
    std::vector<Isotopologues> isotopologueMixtures_;

    public:
    // Clear contents
    void clear();
    // Add Species Isotopologue to the relevant mixture
    void addIsotopologue(const Species *sp, double speciesPopulation, const Isotopologue *iso,
                         double isotopologueRelativePopulation);
    // Return whether an Isotopologues definition exists for the provided Species
    bool containsIsotopologues(const Species *sp) const;
    // Print atomtype / weights information
    void print() const;

    /*
     * Data
     */
    private:
    // Type mix derived from Species referenced in isotopologueMixtures_
    AtomTypeMix atomTypes_;
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
    void createFromIsotopologues(const std::vector<std::shared_ptr<AtomType>> &exchangeableTypes);
    // Reduce data to be naturally-weighted
    void naturalise();
    // Return AtomTypeMix
    const AtomTypeMix &atomTypes() const;
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
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
};
