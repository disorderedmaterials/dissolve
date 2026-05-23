// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

// Forward Declarations
class AtomBase;

// NETA Matched Group
class NETAMatchedGroup
{
    public:
    NETAMatchedGroup(const AtomBase *root = nullptr);
    ~NETAMatchedGroup() = default;

    /*
     * Data
     */
    private:
    // Root atom of the matched group
    const AtomBase *root_;
    // Set of matched atoms
    std::set<const AtomBase *> set_;
    // Identified atoms (if any) in path
    std::map<std::string, std::set<const AtomBase *>> identifiers_;

    public:
    // Return set of matched atoms
    const std::set<const AtomBase *> &set() const;
    // Return identified atoms (if any) in the group
    const std::map<std::string, std::set<const AtomBase *>> &identifiers() const;

    /*
     * Group Management
     */
    public:
    // Insert atom into group
    void insert(const AtomBase *i);
    // Return whether the set contains the specified atom
    bool contains(const AtomBase *i) const;
    // Return whether the specified atom is the root atom of the match
    bool isRoot(const AtomBase *i) const;
    // Merge supplied group into this one, inserting new atoms and copying any relevant identifiers
    void merge(const NETAMatchedGroup &other);
    // Add identifier for specified atom
    void addIdentifier(const AtomBase *i, std::string id);
};
