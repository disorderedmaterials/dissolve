// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "neta/matchedpath.h"
#include "classes/speciesatom.h"
#include <algorithm>

// Return path of matched atoms
const std::vector<const SpeciesAtom *> &NETAMatchedPath::path() const { return path_; }

// Return identified atoms (if any) in path
const std::map<std::string, std::set<const SpeciesAtom *>> &NETAMatchedPath::identifiers() const { return identifiers_; }

/*
 * Path Management
 */

// Append atom to path
void NETAMatchedPath::append(const SpeciesAtom *i) { path_.push_back(i); }

// Return whether the path contains the specified atom
bool NETAMatchedPath::contains(const SpeciesAtom *i) const { return std::find(path_.begin(), path_.end(), i) != path_.end(); }

// Return whether the specified atom is the root atom in the path
bool NETAMatchedPath::isRoot(const SpeciesAtom *i) const { return !path_.empty() && (path_.front() == i); }

// Merge supplied path into this one, appending new atoms and copying any relevant identifiers
void NETAMatchedPath::merge(const NETAMatchedPath &other)
{
    // Copy new, unique atoms to our path
    std::copy_if(other.path_.begin(), other.path_.end(), std::back_inserter(path_),
                 [&](const auto *j) { return std::find(path_.begin(), path_.end(), j) == path_.end(); });

    // Copy any identifiers
    for (auto &&[key, atoms] : other.identifiers_)
        for (auto &i : atoms)
            identifiers_[key].insert(i);
}

// Add identifier for specified atom
void NETAMatchedPath::addIdentifier(const SpeciesAtom *i, std::string id) { identifiers_[id].insert(i); }
