// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

// Forward Declarations
class SpeciesAtom;

// NETA Matched Path
class NETAMatchedPath
{
    private:
    // Path of matched atoms
    std::vector<const SpeciesAtom *> path_;
    // Identified atoms (if any) in path
    std::map<std::string, std::set<const SpeciesAtom *>> identifiers_;

    public:
    // Return path of matched atoms
    const std::vector<const SpeciesAtom *> &path() const;
    // Return identified atoms (if any) in path
    const std::map<std::string, std::set<const SpeciesAtom *>> &identifiers() const;

    /*
     * Path Management
     */
    public:
    // Append atom to path
    void append(const SpeciesAtom *i);
    // Return whether the path contains the specified atom
    bool contains(const SpeciesAtom *i) const;
    // Return whether the specified atom is the root atom in the path
    bool isRoot(const SpeciesAtom *i) const;
    // Merge supplied path into this one, appending new atoms and copying any relevant identifiers
    void merge(const NETAMatchedPath &other);
    // Add identifier for specified atom
    void addIdentifier(const SpeciesAtom *i, std::string id);
};
