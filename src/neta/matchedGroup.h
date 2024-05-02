// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

// Forward Declarations
class SpeciesAtom;

// NETA Matched Group
class NETAMatchedGroup
{
    public:
    NETAMatchedGroup(const SpeciesAtom *root = nullptr);
    ~NETAMatchedGroup() = default;

    /*
     * Data
     */
    private:
    // Root atom of the matched group
    const SpeciesAtom *root_;
    // Set of matched atoms
    std::set<const SpeciesAtom *> set_;
    // Identified atoms (if any) in path
    std::map<std::string, std::set<const SpeciesAtom *>> identifiers_;

    public:
    // Return set of matched atoms
    const std::set<const SpeciesAtom *> &set() const;
    // Return identified atoms (if any) in the group
    const std::map<std::string, std::set<const SpeciesAtom *>> &identifiers() const;

    /*
     * Group Management
     */
    public:
    // Insert atom into group
    void insert(const SpeciesAtom *i);
    // Return whether the set contains the specified atom
    bool contains(const SpeciesAtom *i) const;
    // Return whether the specified atom is the root atom of the match
    bool isRoot(const SpeciesAtom *i) const;
    // Merge supplied group into this one, inserting new atoms and copying any relevant identifiers
    void merge(const NETAMatchedGroup &other);
    // Add identifier for specified atom
    void addIdentifier(const SpeciesAtom *i, std::string id);
};
