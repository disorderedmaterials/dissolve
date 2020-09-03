/*
    *** Isotopologues Definition
    *** src/classes/isotopologues.h
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

#include "classes/isotopologueweight.h"
#include "genericitems/base.h"
#include <vector>

// Forward Declarations
class Species;
class Isotopologue;
class ProcessPool;
class LineParser;

// Isotopologues
class Isotopologues : public GenericItemBase
{
    public:
    Isotopologues(Species *species = nullptr, int speciesPopulation = 0);
    ~Isotopologues();

    /*
     * Isotopologue Mixture Data
     */
    private:
    // Associated Species
    Species *species_;
    // Integer population of associated Species
    int speciesPopulation_;
    // Weighted Isotopologue mixture
    std::vector<IsotopologueWeight> mix_;

    public:
    // Set associated Species and population
    void setSpecies(Species *sp, int population);
    // Return associated Species
    Species *species() const;
    // Return associated Species population
    int speciesPopulation() const;
    /// Prune defunct Isotopologue entries
    void pruneMissing();
    // Add next available Isotopologue to list
    bool addNext();
    // Add specific Isotopologue to list
    void add(const Isotopologue *iso, double relativeWeight);
    // Set Isotopologue component in list
    bool set(const Isotopologue *iso, double relativeWeight);
    // Remove references to the specified Isotopologue
    void remove(const Isotopologue *iso);
    // Remove the specified IsotopologueWeight
    void remove(IsotopologueWeight *isoWeight);
    // Return whether the mix contains the specified Isotopologue
    bool contains(const Isotopologue *iso) const;
    // Return Isotopologue/weight mix
    std::vector<IsotopologueWeight> &mix();
    // Return Isotopologue/weight mix (const)
    const std::vector<IsotopologueWeight> &constMix() const;
    // Return number of Isotopologues in mix
    int nIsotopologues() const;
    // Return total relative population
    double totalRelative() const;
    // Normalise total relative population to 1.0
    void normalise();

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
    // Broadcast data
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check item equality
    bool equality(ProcessPool &procPool);
};
