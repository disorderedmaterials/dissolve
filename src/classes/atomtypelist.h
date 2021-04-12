// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/atomtypedata.h"
#include "classes/coredata.h"
#include "templates/optionalref.h"
#include <tuple>
#include <vector>

// Forward Declarations
class AtomType;
class Isotope;

// AtomTypeList
class AtomTypeList
{
    public:
    AtomTypeList();
    ~AtomTypeList();
    AtomTypeList(const AtomTypeList &source);
    void operator=(const AtomTypeList &source);
    AtomTypeData &operator[](int n);

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
    const std::shared_ptr<AtomType> atomType(int n) const;
    // Return AtomTypeData for specified AtomType
    OptionalReferenceWrapper<const AtomTypeData> atomTypeData(std::shared_ptr<AtomType> atomType);
    // Print AtomType populations
    void print() const;

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
};
