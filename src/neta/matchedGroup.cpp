// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "neta/matchedGroup.h"
#include "classes/speciesAtom.h"
#include "classes/speciesBond.h"
#include <algorithm>

NETAMatchedGroup::NETAMatchedGroup(const AtomBase *root) : root_(root) {}

/*
 * Data
 */

// Return vector of matched atoms in the order they were matched
const std::vector<const AtomBase *> &NETAMatchedGroup::matched() const { return matched_; }

// Return identified atoms (if any) in the group
const std::map<std::string, std::set<const AtomBase *>> &NETAMatchedGroup::identifiers() const { return identifiers_; }

/*
 * Path Management
 */

// Insert atom into group
void NETAMatchedGroup::append(const AtomBase *i)
{
    if (!contains(i))
        matched_.push_back(i);
}
void NETAMatchedGroup::append(const NETAMatchedGroup &other)
{ // Copy new, unique atoms to our set
    for (auto *i : other.matched_)
        append(i);

    // Copy any identifiers
    for (auto &&[key, atoms] : other.identifiers_)
        for (auto &i : atoms)
            identifiers_[key].insert(i);
}

// Return whether the set contains the specified atom
bool NETAMatchedGroup::contains(const AtomBase *i) const { return std::ranges::find(matched_, i) != matched_.end(); }

// Return whether the specified atom is the root atom of the match
bool NETAMatchedGroup::isRoot(const AtomBase *i) const { return root_ == i; }

// Add identifier for specified atom
void NETAMatchedGroup::addIdentifier(const AtomBase *i, std::string id) { identifiers_[id].insert(i); }
