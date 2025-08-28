// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/isotopeMix.h"
#include "classes/isotopologues.h"
#include "templates/doubleKeyedMap.h"
#include <vector>

// Forward Declarations
class IsotopologueSet;

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
    // Isotope mix derived from Species referenced in isotopologueMixtures_
    IsotopeMix isotopeMix_;
    // Concentration product matrix (ci * cj)
    DoubleKeyedMap<double> concentrationProducts_;
    // Bound coherent product matrix (bi * bj)
    DoubleKeyedMap<double> boundCoherentProducts_;
    // Full scattering weights
    DoubleKeyedMap<double> weights_;
    // Intramolecular scattering weights
    DoubleKeyedMap<double> intramolecularWeights_;
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
    // Return isotope mix
    const IsotopeMix &isotopeMix() const;
    // Return full scattering weights
    const DoubleKeyedMap<double> &weights() const;
    // Return concentration products
    const DoubleKeyedMap<double> &concentrationProducts() const;
    // Return bound coherent scattering products
    const DoubleKeyedMap<double> &boundCoherentProducts() const;
    // Return full intramolecular scattering weights
    const DoubleKeyedMap<double> &intramolecularWeights() const;
    // Return bound coherent average squared scattering (<b>**2)
    double boundCoherentSquareOfAverage() const;
    // Return bound coherent squared average scattering (<b**2>)
    double boundCoherentAverageOfSquares() const;
    // Return whether the structure is valid (i.e. has been finalised)
    bool isValid() const;
};
