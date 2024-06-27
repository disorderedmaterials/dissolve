// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/atomTypeData.h"
#include "templates/optionalRef.h"
#include <tuple>
#include <vector>

// Forward Declarations
class AtomType;
class CoreData;
class LineParser;

// AtomTypeMix
class AtomTypeMix
{
    public:
    AtomTypeMix() = default;
    ~AtomTypeMix() = default;
    AtomTypeMix(const AtomTypeMix &source);
    void operator=(const AtomTypeMix &source);
    AtomTypeData &operator[](int n);
    const AtomTypeData &operator[](int n) const;

    /*
     * Types
     */
    private:
    // Vector of AtomTypeData
    std::vector<AtomTypeData> types_;

    public:
    // Clear all data
    void clear();
    // Zero populations of all types
    void zero();
    // Add the specified AtomType to the list, returning data object and its index in the vector
    std::pair<AtomTypeData &, int> add(std::shared_ptr<AtomType> atomType, double popAdd = 0);
    // Add the AtomTypes in the supplied object into this one, increasing populations etc.
    void add(const AtomTypeMix &source);
    // Add/increase this AtomType/Isotope pair, returning the index of the AtomType
    void addIsotope(std::shared_ptr<AtomType> atomType, Sears91::Isotope tope, double popAdd = 0);
    // Finalise, calculating fractional populations etc.
    void finalise();
    // Finalise, calculating fractional populations etc., and accounting for exchangeable sites in boundCoherent values
    void finalise(const std::vector<std::shared_ptr<AtomType>> &exchangeableTypes);
    // Make all AtomTypeData reference only their natural isotope
    void naturalise();
    // Check for presence of AtomType
    bool contains(const std::shared_ptr<AtomType> &atomType) const;
    // Check for presence of AtomType/Isotope pair
    bool contains(const std::shared_ptr<AtomType> &atomType, Sears91::Isotope tope) const;
    // Return number of AtomType/Isotopes
    int nItems() const;
    // Return first item in list
    const AtomTypeData &first() const;
    // Return opening iterator
    std::vector<AtomTypeData>::const_iterator begin() const;
    // Return ending iterator
    std::vector<AtomTypeData>::const_iterator end() const;
    // Return index of AtomType
    std::optional<int> indexOf(const std::shared_ptr<AtomType> &atomType) const;
    // Return indices of AtomType pair
    std::optional<std::pair<int, int>> indexOf(const std::shared_ptr<AtomType> &at1,
                                               const std::shared_ptr<AtomType> &at2) const;
    // Return index of named AtomType
    std::optional<int> indexOf(std::string_view name) const;
    // Return total population of all types
    double totalPopulation() const;
    // Return nth referenced AtomType
    const std::shared_ptr<AtomType> atomType(int n) const;
    // Return AtomTypeData for specified AtomType
    OptionalReferenceWrapper<const AtomTypeData> atomTypeData(const std::shared_ptr<AtomType> &atomType) const;
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
