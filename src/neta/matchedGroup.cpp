// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "neta/matchedGroup.h"
#include "classes/speciesAtom.h"
#include "classes/speciesBond.h"
#include <algorithm>

NETAMatchedGroup::NETAMatchedGroup(const BaseAtom *root) : root_(root) {}

/*
 * Data
 */

// Return set of matched atoms
const std::set<const BaseAtom *> &NETAMatchedGroup::set() const { return set_; }

// Return identified atoms (if any) in the group
const std::map<std::string, std::set<const BaseAtom *>> &NETAMatchedGroup::identifiers() const { return identifiers_; }

/*
 * Path Management
 */

// Insert atom into group
void NETAMatchedGroup::insert(const BaseAtom *i) { set_.insert(i); }

// Return whether the set contains the specified atom
bool NETAMatchedGroup::contains(const BaseAtom *i) const { return set_.find(i) != set_.end(); }

// Return whether the specified atom is the root atom of the match
bool NETAMatchedGroup::isRoot(const BaseAtom *i) const { return root_ == i; }

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
void NETAMatchedGroup::addIdentifier(const BaseAtom *i, std::string id) { identifiers_[id].insert(i); }
