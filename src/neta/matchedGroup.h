// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

// Forward Declarations
class BaseAtom;

// NETA Matched Group
class NETAMatchedGroup
{
    public:
    NETAMatchedGroup(const BaseAtom *root = nullptr);
    ~NETAMatchedGroup() = default;

    /*
     * Data
     */
    private:
    // Root atom of the matched group
    const BaseAtom *root_;
    // Set of matched atoms
    std::set<const BaseAtom *> set_;
    // Identified atoms (if any) in path
    std::map<std::string, std::set<const BaseAtom *>> identifiers_;

    public:
    // Return set of matched atoms
    const std::set<const BaseAtom *> &set() const;
    // Return identified atoms (if any) in the group
    const std::map<std::string, std::set<const BaseAtom *>> &identifiers() const;

    /*
     * Group Management
     */
    public:
    // Insert atom into group
    void insert(const BaseAtom *i);
    // Return whether the set contains the specified atom
    bool contains(const BaseAtom *i) const;
    // Return whether the specified atom is the root atom of the match
    bool isRoot(const BaseAtom *i) const;
    // Merge supplied group into this one, inserting new atoms and copying any relevant identifiers
    void merge(const NETAMatchedGroup &other);
    // Add identifier for specified atom
    void addIdentifier(const BaseAtom *i, std::string id);
};
