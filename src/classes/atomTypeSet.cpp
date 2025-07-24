// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/atomTypeSet.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
#include <ranges>

AtomTypeSet::AtomTypeSet(const AtomTypeSet &source) { (*this) = source; }

AtomTypeSet &AtomTypeSet::operator=(const AtomTypeSet &source)
{
    set_ = source.set_;
    return *this;
}

/*
 * Set
 */

// Clear all data
void AtomTypeSet::clear() { set_.clear(); }

// Zero populations of all types in the list
void AtomTypeSet::zero()
{
    for (auto &data : std::views::values(set_))
        data = 0.0;
}

// Add (to) the named type
void AtomTypeSet::add(const std::shared_ptr<AtomType> &atomType, double population) { set_[atomType] += population; }

// Add the specified set into ours
void AtomTypeSet::add(const AtomTypeSet &set)
{
    for (auto &[atomType, pop] : set)
        set_[atomType] += pop;
}

// Check for presence of named AtomType
bool AtomTypeSet::contains(const std::shared_ptr<AtomType> &atomType) const
{
    return set_.contains(atomType);
}

// Return number of AtomType/Isotopes
int AtomTypeSet::nItems() const { return set_.size(); }

// Return starting iterator
std::map<const std::shared_ptr<AtomType>, double>::const_iterator AtomTypeSet::begin() const { return set_.begin(); }

// Return ending iterator
std::map<const std::shared_ptr<AtomType>, double>::const_iterator AtomTypeSet::end() const { return set_.end(); }

// Return total population of all types
double AtomTypeSet::totalPopulation() const
{
    auto total = 0.0;
    for (auto &data : std::views::values(set_))
        total += data;
    return total;
}

// Return map of fractional populations of types
std::map<const std::shared_ptr<AtomType>, double> AtomTypeSet::fractional() const
{
    auto result = set_;
    auto totalPop = totalPopulation();
    for (auto &data : std::views::values(result))
        data /= totalPop;
    return result;
}
