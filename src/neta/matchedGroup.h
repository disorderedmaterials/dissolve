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
    // Matched atoms
    std::vector<const AtomBase *> matched_;
    // Identified atoms (if any) in path
    std::map<std::string, std::set<const AtomBase *>> identifiers_;

    public:
    // Return vector of matched atoms in the order they were matched
    const std::vector<const AtomBase *> &matched() const;
    // Return identified atoms (if any) in the group
    const std::map<std::string, std::set<const AtomBase *>> &identifiers() const;

    /*
     * Group Management
     */
    public:
    // Append into group
    void append(const AtomBase *i);
    void append(const NETAMatchedGroup &other);
    // Return whether the set contains the specified atom
    bool contains(const AtomBase *i) const;
    // Return whether the specified atom is the root atom of the match
    bool isRoot(const AtomBase *i) const;
    // Add identifier for specified atom
    void addIdentifier(const AtomBase *i, std::string id);
};
