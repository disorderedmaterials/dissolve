// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/exchangeables.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
#include "classes/species.h"

Exchangeables::Exchangeables(const std::vector<std::string_view> &atomTypeNames)
{
    for (const auto &atomTypeName : atomTypeNames)
        add(atomTypeName);
}

/*
 * Data
 */

// Clear all existing data
void Exchangeables::clear() { atomTypeNames_.clear(); }

// Add atom type
void Exchangeables::add(const std::string_view atomTypeName) { atomTypeNames_.emplace(atomTypeName); }

// Remove the specified atom type
void Exchangeables::remove(const std::string_view atomTypeName) { atomTypeNames_.erase(std::string(atomTypeName)); }

// Return whether the specified atom type name is present
bool Exchangeables::contains(const std::string_view atomTypeName) const
{
    return atomTypeNames_.contains(std::string(atomTypeName));
}

// Return vector of all exchangeables
std::set<std::string> &Exchangeables::exchangeables() { return atomTypeNames_; }

const std::set<std::string> &Exchangeables::exchangeables() const { return atomTypeNames_; }

/*
 * Serialisation
 */

// Express as a serialisable value
void Exchangeables::serialise(std::string tag, SerialisedValue &target) const
{
    if (atomTypeNames_.size() == 0)
        return;

    SerialisedValue value;
    value["atomTypeNames"] = fromRange(atomTypeNames_, [](const auto &name) { return name; });
    target[tag] = value;
}

// Read values from a serialisable value
void Exchangeables::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    clear();

    toVector(node, "atomTypeNames", [&](const auto &nameNode) { atomTypeNames_.emplace(nameNode.as_string()); });
}
