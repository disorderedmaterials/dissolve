// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "data/formFactors.h"
#include "templates/doubleKeyedMap.h"
#include "templates/keyedVector.h"
#include <functional>
#include <map>
#include <vector>

// Forward Declarations
class AtomType;
class Species;

// XRay Weights Container
class XRayWeights
{
    public:
    XRayWeights() = default;
    ~XRayWeights() = default;

    private:
    // Type fractions
    KeyedVector<const AtomType *, double> typeFractions_;
    // Form factor data for atom types
    std::map<const AtomType *, std::reference_wrapper<const FormFactorData>> formFactorData_;
    // Concentration product matrix (ci * cj)
    DoubleKeyedMap<double> concentrationProducts_;
    // Pre-factors matrix (ci * cj * [2-dij])
    DoubleKeyedMap<double> preFactors_;

    public:
    // Set-up from supplied species populations and form factors
    bool setUp(const std::vector<std::pair<const Species *, double>> &speciesPopulations,
               XRayFormFactors::XRayFormFactorData formFactors);
    // Return atom type fractions
    const KeyedVector<const AtomType *, double> &typeFractions() const;
    // Return pre-factor for types i and j
    const DoubleKeyedMap<double> &preFactors() const;
    // Return form factor product for types i and j at specified Q value
    double formFactorProduct(const AtomType *i, const AtomType *j, double Q) const;
    // Return form factor for type i over supplied Q values
    std::vector<double> formFactor(const AtomType *i, const std::vector<double> &Q) const;
    // Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) at specified Q value
    double weight(const AtomType *i, const AtomType *j, double Q) const;
    // Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) over supplied Q values
    std::vector<double> weight(const AtomType *i, const AtomType *j, const std::vector<double> &Q) const;
    // Calculate and return Q-dependent average squared scattering (<b>**2) for supplied Q value
    double boundCoherentSquareOfAverage(double Q) const;
    // Calculate and return Q-dependent average squared scattering (<b>**2) for supplied Q values
    std::vector<double> boundCoherentSquareOfAverage(const std::vector<double> &Q) const;
    // Calculate and return Q-dependent squared average scattering (<b**2>) for supplied Q value
    double boundCoherentAverageOfSquares(double Q) const;
    // Calculate and return Q-dependent squared average scattering (<b**2>) for supplied Q values
    std::vector<double> boundCoherentAverageOfSquares(const std::vector<double> &Q) const;
    // Print the atom fractions
    void print() const;
};
