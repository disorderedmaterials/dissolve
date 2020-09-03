/*
    *** Isotopologue Collection
    *** src/classes/isotopologuecollection.h
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

#include "classes/isotopologueset.h"
#include "genericitems/base.h"
#include "templates/optionalref.h"
#include "templates/reflist.h"

// Forward Declarations
class Configuration;
class Species;
class Isotopologue;
class LineParser;
class ProcessPool;

// IsotopologueCollection - Isotopologues over one or more Configurations
class IsotopologueCollection : public GenericItemBase
{
    public:
    IsotopologueCollection();
    ~IsotopologueCollection();

    /*
     * Sets
     */
    private:
    // IsotopologueSets for individual Configurations
    std::vector<IsotopologueSet> isotopologueSets_;

    private:
    // Remove any sets from the collection that are empty
    void pruneEmptySets();

    public:
    // Clear all existing data
    void clear();
    // Add Isotopologue weight for the specified Configuration / Species
    void add(Configuration *cfg, Isotopologue *iso, double relativeWeight);
    // Remove the specified set from the collection
    void remove(IsotopologueSet *set);
    // Remove the Configuration from the collection
    void remove(Configuration *cfg);
    // Remove the Species from the specified set
    void remove(IsotopologueSet *set, Species *sp);
    // Remove the IsotopologueWeight from the specified set
    void remove(IsotopologueSet *set, IsotopologueWeight *isoWeight);
    // Remove any occurrences of the specified Species from the collection
    void remove(Species *sp);
    // Remove any occurrences of the specified Isotopologue from the collection
    void remove(Isotopologue *iso);
    // Return defined sets
    std::vector<IsotopologueSet> &isotopologueSets();
    // Return defined sets (const)
    const std::vector<IsotopologueSet> &constIsotopologueSets() const;
    // Return whether a set exists for the supplied Configuration
    bool contains(const Configuration *cfg) const;
    // Return IsotopologueSet for the specified Configuration
    OptionalReferenceWrapper<const IsotopologueSet> getIsotopologueSet(const Configuration *cfg) const;
    // Return whether the Species has a defined set of isotopologues in the specified Configuration
    bool contains(const Configuration *cfg, const Species *sp) const;
    // Return Isotopologues for the Species in the specified Configuration
    OptionalReferenceWrapper<const Isotopologues> getIsotopologues(const Configuration *cfg, const Species *sp) const;
    // Complete the collection by making sure it contains every Species in every Configuration in the supplied list
    void complete(const RefList<Configuration> &configurations);

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
};
