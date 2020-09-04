/*
    *** AtomTypeList Definition
    *** src/classes/atomtypelist.h
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

#include "classes/atomtypedata.h"
#include "classes/coredata.h"
#include "genericitems/base.h"
#include "templates/optionalref.h"
#include <tuple>
#include <vector>

// Forward Declarations
class AtomType;
class Isotope;

// AtomTypeList
class AtomTypeList : public GenericItemBase
{
    public:
    AtomTypeList();
    ~AtomTypeList();
    AtomTypeList(const AtomTypeList &source);
    void operator=(const AtomTypeList &source);
    AtomTypeData &operator[](unsigned int n);

    /*
     * Type List
     */
    private:
    // List of AtomTypeData
    std::vector<AtomTypeData> types_;

    public:
    // Clear all data
    void clear();
    // Zero populations of all types in the list
    void zero();
    // Add the specified AtomType to the list, returning the AtomTypeData
    AtomTypeData &add(std::shared_ptr<AtomType> atomType, double popAdd = 0);
    // Add the AtomTypes in the supplied list into this one, increasing populations etc.
    void add(const AtomTypeList &source);
    // Remove specified AtomType from the list
    void remove(std::shared_ptr<AtomType> atomType);
    // Add/increase this AtomType/Isotope pair, returning the index of the AtomType in the list
    void addIsotope(std::shared_ptr<AtomType> atomType, Isotope *tope = nullptr, double popAdd = 0);
    // Finalise list, calculating fractional populations etc.
    void finalise();
    // Finalise list, calculating fractional populations etc., and accounting for exchangeable sites in boundCoherent values
    void finalise(const AtomTypeList &exchangeable);
    // Make all AtomTypeData in the list reference only their natural isotope
    void naturalise();
    // Check for presence of AtomType in list
    bool contains(std::shared_ptr<AtomType> atomType) const;
    // Check for presence of AtomType/Isotope pair in list
    bool contains(std::shared_ptr<AtomType> atomType, Isotope *tope);
    // Return number of AtomType/Isotopes in list
    int nItems() const;
    // Return first item in list
    const AtomTypeData &first() const;
    // Return opening iterator
    std::vector<AtomTypeData>::const_iterator begin() const;
    // Return opening iterator
    std::vector<AtomTypeData>::const_iterator end() const;
    // Return index of AtomType in list
    int indexOf(std::shared_ptr<AtomType> atomtype) const;
    // Return index of names AtomType in list
    int indexOf(std::string_view name) const;
    // Return total population of all types in list
    double totalPopulation() const;
    // Return nth referenced AtomType
    std::shared_ptr<AtomType> atomType(int n);
    // Return AtomTypeData for specified AtomType
    OptionalReferenceWrapper<const AtomTypeData> atomTypeData(std::shared_ptr<AtomType> atomType);
    // Print AtomType populations
    void print() const;

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
