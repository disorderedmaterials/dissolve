// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "neta/matchedGroup.h"
#include "classes/speciesAtom.h"
#include <algorithm>

NETAMatchedGroup::NETAMatchedGroup(const SpeciesAtom *root) : root_(root) {}

/*
 * Data
 */

// Return set of matched atoms
const std::set<const SpeciesAtom *> &NETAMatchedGroup::set() const { return set_; }

// Return identified atoms (if any) in the group
const std::map<std::string, std::set<const SpeciesAtom *>> &NETAMatchedGroup::identifiers() const { return identifiers_; }

/*
 * Path Management
 */

// Insert atom into group
void NETAMatchedGroup::insert(const SpeciesAtom *i) { set_.insert(i); }

// Return whether the set contains the specified atom
bool NETAMatchedGroup::contains(const SpeciesAtom *i) const { return set_.find(i) != set_.end(); }

// Return whether the specified atom is the root atom of the match
bool NETAMatchedGroup::isRoot(const SpeciesAtom *i) const { return root_ == i; }

// Merge supplied set into this one, inserting new atoms and copying any relevant identifiers
void NETAMatchedGroup::merge(const NETAMatchedGroup &other)
{
    // Copy new, unique atoms to our set
    std::copy(other.set_.begin(), other.set_.end(), std::inserter(set_, set_.begin()));

    // Copy any identifiers
    for (auto &&[key, atoms] : other.identifiers_)
        for (auto &i : atoms)
            identifiers_[key].insert(i);
}

// Add identifier for specified atom
void NETAMatchedGroup::addIdentifier(const SpeciesAtom *i, std::string id) { identifiers_[id].insert(i); }
