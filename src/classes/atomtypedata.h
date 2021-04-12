// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/list.h"
#include <memory>

// Forward Declarations
class AtomType;
class CoreData;
class IsotopeData;
class Isotope;

/*
 * AtomTypeData Definition
 */
class AtomTypeData
{
    public:
    AtomTypeData(std::shared_ptr<AtomType> type, double population = 0, double fraction = 0, double boundCoherent = 0,
                 int nIso = 0);
    AtomTypeData(const AtomTypeData &source);
    AtomTypeData(int listIndex, std::shared_ptr<AtomType> atomType, double population);
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
    // Return the number of defined Isotopes
    int nIsotopes() const;
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
    bool serialise(LineParser &parser) const;
};
