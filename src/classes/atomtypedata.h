/*
    *** AtomTypeData Definition
    *** src/classes/atomtypedata.h
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

#include "templates/list.h"
#include <memory>

// Forward Declarations
class AtomType;
class CoreData;
class IsotopeData;
class Isotope;
class ProcessPool;

/*
 * AtomTypeData Definition
 */
class AtomTypeData
{
    public:
    AtomTypeData(std::shared_ptr<AtomType> type, double population = 0, double fraction = 0, double boundCoherent = 0,
                 int nIso = 0);
    AtomTypeData(const AtomTypeData &source);
    // Read data through specified LineParser
    AtomTypeData(LineParser &parser, const CoreData &coreData, int listIndex);
    // Old Initialise
    AtomTypeData(int listIndex, std::shared_ptr<AtomType> atomType, double population);
    // Assignment Operator
    void operator=(const AtomTypeData &source);

    /*
     * Properties
     */
    private:
    // List index of AtomTypeData in AtomTypeList
    int listIndex_;
    // Reference AtomType
    std::shared_ptr<AtomType> atomType_;
    // Whether the AtomType has been marked as exchangeable
    bool exchangeable_;
    // Isotopes information (if any)
    List<IsotopeData> isotopes_;
    // Total population
    double population_;
    // World fractional population over all Isotopes
    double fraction_;
    // Calculated bound coherent scattering over all Isotopes
    double boundCoherent_;

    public:
    // Add to population
    void add(double nAdd);
    // Add to population of Isotope
    void add(Isotope *tope, double nAdd);
    // Zero populations
    void zeroPopulations();
    // Return list index of AtomTypeData in AtomTypeList
    int listIndex() const;
    // Return reference AtomType
    std::shared_ptr<AtomType> atomType() const;
    // Set exchangeable flag
    void setAsExchangeable();
    // Return whether the associated AtomType is exchangeable
    bool exchangeable() const;
    // Finalise, calculating fractional populations etc.
    void finalise(double nWorldAtoms);
    // Remove any existing isotopes, and add only the natural isotope
    void naturalise();
    // Return if specified Isotope is already in the list
    bool hasIsotope(Isotope *tope);
    // Set this AtomType to have only the single Isotope provided
    void setSingleIsotope(Isotope *tope);
    // Return first IsotopeData
    IsotopeData *isotopeData() const;
    // Return total population over all isotopes
    int population() const;
    // Return total fractional population including all isotopes
    double fraction() const;
    // Set calculated bond coherent scattering over all isotopes
    void setBoundCoherent(double d);
    // Return calculated bound coherent scattering over all Isotopes
    double boundCoherent() const;
    // Return referenced AtomType name
    std::string_view atomTypeName() const;

    /*
     * I/O
     */
    public:
    // Write data through specified LineParser
    bool write(LineParser &parser);

    /*
     * Parallel Comms
     */
    public:
    // Broadcast data from Master to all Slaves
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check item equality
    bool equality(ProcessPool &procPool);
};
