// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include <map>
#include <string>

// Forward Declarations
class AtomType;
class CoreData;
class LineParser;

// AtomType Set
class AtomTypeSet
{
    public:
    AtomTypeSet() = default;
    ~AtomTypeSet() = default;
    AtomTypeSet(const AtomTypeSet &source);
    AtomTypeSet &operator=(const AtomTypeSet &source);

    /*
     * Set
     */
    private:
    // Map of atom type name to real population
    std::map<std::string, double> set_;

    public:
    // Clear all data
    void clear();
    // Zero populations of all types
    void zero();
    // Add (to) the named AtomType
    void add(std::string_view atomTypeName, double popAdd = 0);
    // // Check for presence of AtomType
    // bool contains(const std::shared_ptr<AtomType> &atomType) const;
    // Return number of AtomType/Isotopes
    int nItems() const;
    // Return opening iterator
    std::map<std::string, double>::const_iterator begin() const;
    // Return ending iterator
    std::map<std::string, double>::const_iterator end() const;
    // Return total population of all types
    double totalPopulation() const;
    // Return map of fractional populations of types
    std::map<std::string, double> fractional() const;
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
