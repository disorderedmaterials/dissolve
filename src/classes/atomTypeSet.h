// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include <map>
#include <memory>
#include <string>

// Forward Declarations
class AtomType;

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
    // Map of atom type to real population
    std::map<const std::shared_ptr<AtomType>, double> set_;

    public:
    // Clear all data
    void clear();
    // Zero populations of all types
    void zero();
    // Add (to) the named AtomType
    void add(const std::shared_ptr<AtomType> &atomType, double popAdd = 0);
    // Add the specified set into ours
    void add(const AtomTypeSet &set);
    // Check for presence of AtomType
    bool contains(const std::shared_ptr<AtomType> &atomType) const;
    // Return number of AtomType/Isotopes
    int nItems() const;
    // Return opening iterator
    std::map<const std::shared_ptr<AtomType>, double>::const_iterator begin() const;
    // Return ending iterator
    std::map<const std::shared_ptr<AtomType>, double>::const_iterator end() const;
    // Return total population of all types
    double totalPopulation() const;
    // Return map of fractional populations of types
    std::map<const std::shared_ptr<AtomType>, double> fractional() const;
    // Print AtomType populations
    void print() const;
};
