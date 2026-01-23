// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/exchangeables.h"
#include "classes/isotopeMix.h"
#include "templates/doubleKeyedMap.h"

// Forward Declarations
class IsotopologueSet;

// Neutron Weights Container
class NeutronWeights
{
    public:
    NeutronWeights() = default;
    NeutronWeights(const std::map<const Species *, double> &speciesPopulations, const IsotopologueSet &speciesIsotopologues,
                   const Exchangeables &exchangeables = {});
    NeutronWeights(const NeutronWeights &source);
    void operator=(const NeutronWeights &source);

    /*
     * Construction
     */
    public:
    // Clear contents
    void clear();

    /*
     * Data
     */
    private:
    // Isotope mix
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

    private:
    // Calculate weighting matrices based on current AtomType / Isotope information
    void calculateWeightingMatrices(const std::map<const Species *, double> &speciesPopulations,
                                    const IsotopologueSet &isotopologues);

    public:
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
};
