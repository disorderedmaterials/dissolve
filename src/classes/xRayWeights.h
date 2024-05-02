// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/atomTypeMix.h"
#include "data/formFactors.h"
#include "templates/array2D.h"
#include <functional>
#include <vector>

// Forward Declarations
class Species;
class SpeciesInfo;

// XRay Weights Container
class XRayWeights
{
    public:
    XRayWeights();
    XRayWeights(const XRayWeights &source);
    void operator=(const XRayWeights &source);

    /*
     * Source AtomTypes
     */
    private:
    // X-Ray form factors to use
    XRayFormFactors::XRayFormFactorData formFactors_{XRayFormFactors::NoFormFactorData};
    // Type list derived from supplied Species
    AtomTypeMix atomTypeMix_;
    // Form factor data for atom types
    std::vector<std::reference_wrapper<const FormFactorData>> formFactorData_;
    // Whether the structure is valid (i.e. has been finalised)
    bool valid_{false};

    private:
    // Initialise form factor data for the current atom types
    bool initialiseFormFactors();

    public:
    // Clear contents
    void clear();
    // Set-up from supplied SpeciesInfo list
    bool setUp(std::vector<std::pair<const Species *, int>> &speciesPopulations,
               XRayFormFactors::XRayFormFactorData formFactors);
    // Add Species to weights in the specified population
    void addSpecies(const Species *sp, int population);
    // Finalise weights after addition of all individual Species
    bool finalise(XRayFormFactors::XRayFormFactorData formFactors);
    // Return X-Ray form factors being used
    XRayFormFactors::XRayFormFactorData formFactors() const;
    // Return AtomTypeMix
    const AtomTypeMix &atomTypeMix() const;
    // Return number of used AtomTypes
    int nUsedTypes() const;
    // Print atomtype information
    void print() const;

    /*
     * Data
     */
    private:
    // Concentration products (ci)
    std::vector<double> concentrations_;
    // Concentration product matrix (ci * cj)
    Array2D<double> concentrationProducts_;
    // Pre-factors matrix (ci * cj * [2-dij])
    Array2D<double> preFactors_;

    private:
    // Set up matrices based on current AtomType information
    void setUpMatrices();

    public:
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
