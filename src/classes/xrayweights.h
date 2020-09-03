/*
    *** XRay Weights Container
    *** src/classes/xrayweights.h
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
    Array<double> concentrations_;
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
    Array<double> formFactor(int typeIndexI, const Array<double> &Q) const;
    // Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) at specified Q value
    double weight(int typeIndexI, int typetypeIndexJ, double Q) const;
    // Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) over supplied Q values
    Array<double> weight(int typeIndexI, int typeIndexJ, const Array<double> &Q) const;
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
