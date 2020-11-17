// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "classes/atomtypelist.h"
#include "data/formfactors.h"
#include "genericitems/base.h"
#include "templates/array2d.h"
#include "templates/list.h"
#include <functional>
#include <vector>

// Forward Declarations
class Species;
class SpeciesInfo;

// XRay Weights Container
class XRayWeights : public GenericItemBase
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
    XRayFormFactors::XRayFormFactorData formFactors_;
    // Type list derived from supplied Species
    AtomTypeList atomTypes_;
    // Form factor data for atom types
    std::vector<std::reference_wrapper<const FormFactorData>> formFactorData_;
    // Whether the structure is valid (i.e. has been finalised)
    bool valid_;

    private:
    // Initialise form factor data for the current atom types
    bool initialiseFormFactors();

    public:
    // Clear contents
    void clear();
    // Set-up from supplied SpeciesInfo list
    bool setUp(List<SpeciesInfo> &speciesInfoList, XRayFormFactors::XRayFormFactorData formFactors);
    // Add Species to weights in the specified population
    void addSpecies(const Species *sp, int population);
    // Finalise weights after addition of all individual Species
    bool finalise(XRayFormFactors::XRayFormFactorData formFactors);
    // Return AtomTypeList
    AtomTypeList &atomTypes();
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
    // Average squared scattering (<b>**2)
    double boundCoherentSquareOfAverage_;
    // Bound coherent squared average scattering (<b**2>)
    double boundCoherentAverageOfSquares_;

    private:
    // Set up matrices based on current AtomType information
    void setUpMatrices();

    public:
    // Return concentration product for type i
    double concentration(int typeIndexI) const;
    // Return concentration product for types i and j
    double concentrationProduct(int typeIndexI, int typeIndexJ) const;
    // Return form factor product for types i and j at specified Q value
    double formFactorProduct(int typeIndexI, int typeIndexJ, double Q) const;
    // Return form factor for type i over supplied Q values
    std::vector<double> formFactor(int typeIndexI, const std::vector<double> &Q) const;
    // Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) at specified Q value
    double weight(int typeIndexI, int typetypeIndexJ, double Q) const;
    // Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) over supplied Q values
    std::vector<double> weight(int typeIndexI, int typeIndexJ, const std::vector<double> &Q) const;
    // Calculate and return Q-dependent average squared scattering (<b>**2) for supplied Q values
    std::vector<double> boundCoherentSquareOfAverage(const std::vector<double> &Q) const;
    // Calculate and return Q-dependent squared average scattering (<b**2>) for supplied Q values
    std::vector<double> boundCoherentAverageOfSquares(const std::vector<double> &Q) const;
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
