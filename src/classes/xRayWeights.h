// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "data/formFactors.h"
#include "templates/array2D.h"
#include "templates/keyedVector.h"
#include <functional>
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
    // X-Ray form factors to use
    XRayFormFactors::XRayFormFactorData formFactors_{XRayFormFactors::NoFormFactorData};
    // Type fractions
    KeyedVector<const AtomType *, double> typeFractions_;
    // Form factor data for atom types
    std::vector<std::reference_wrapper<const FormFactorData>> formFactorData_;
    // Concentration products (ci)
    std::vector<double> concentrations_;
    // Concentration product matrix (ci * cj)
    Array2D<double> concentrationProducts_;
    // Pre-factors matrix (ci * cj * [2-dij])
    Array2D<double> preFactors_;

    public:
    // Set-up from supplied species populations and form factors
    bool setUp(const std::vector<std::pair<const Species *, double>> &speciesPopulations,
               XRayFormFactors::XRayFormFactorData formFactors);
    // Return X-Ray form factors being used
    XRayFormFactors::XRayFormFactorData formFactors() const;
    // Return atom type fractions
    const KeyedVector<const AtomType *, double> &typeFractions() const;
    // Return concentration product for type i
    double concentration(int typeIndexI) const;
    // Return concentration product for types i and j
    double concentrationProduct(int typeIndexI, int typeIndexJ) const;
    // Return pre-factor for types i and j
    double preFactor(int typeIndexI, int typeIndexJ) const;
    // Return form factor product for types i and j at specified Q value
    double formFactorProduct(int typeIndexI, int typeIndexJ, double Q) const;
    // Return form factor for type i over supplied Q values
    std::vector<double> formFactor(int typeIndexI, const std::vector<double> &Q) const;
    // Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) at specified Q value
    double weight(int typeIndexI, int typeIndexJ, double Q) const;
    // Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) over supplied Q values
    std::vector<double> weight(int typeIndexI, int typeIndexJ, const std::vector<double> &Q) const;
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
