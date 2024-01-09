// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/isotopologueSet.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include <algorithm>

/*
 * Data
 */

// Clear all existing data
void IsotopologueSet::clear() { isotopologues_.clear(); }

// Add Isotopologue with the specified relative weight
void IsotopologueSet::add(const Isotopologue *iso, double relativeWeight)
{
    auto it = std::find_if(isotopologues_.begin(), isotopologues_.end(),
                           [iso](auto &data) { return data.species() == iso->parent(); });
    if (it != isotopologues_.end())
        it->add(iso, relativeWeight);
    else
    {
        isotopologues_.emplace_back(iso->parent(), 0);
        isotopologues_.back().add(iso, relativeWeight);
    }
}

// Remove specified Species from the list (if it exists)
void IsotopologueSet::remove(const Species *sp)
{
    isotopologues_.erase(
        std::remove_if(isotopologues_.begin(), isotopologues_.end(), [sp](const auto &data) { return data.species() == sp; }),
        isotopologues_.end());
}

// Remove any occurrences of the specified Isotopologue
void IsotopologueSet::remove(const Isotopologue *iso)
{
    // Get parent Isotopologues from the contained Species pointer
    auto it = std::find_if(isotopologues_.begin(), isotopologues_.end(),
                           [iso](auto &data) { return data.species() == iso->parent(); });

    if (it != isotopologues_.end())
    {
        it->remove(iso);

        // Check for Isotopologues now being empty
        if (it->nIsotopologues() == 0)
            isotopologues_.erase(it);
    }
}

// Remove the specified IsotopologueWeight
void IsotopologueSet::remove(IsotopologueWeight *isoWeight)
{
    // Get Isotopologues related to the IsotopologueWeight's Species pointer
    auto it = std::find_if(isotopologues_.begin(), isotopologues_.end(),
                           [isoWeight](auto &data) { return data.species() == isoWeight->isotopologue()->parent(); });

    if (it != isotopologues_.end())
    {
        it->remove(isoWeight);

        // Check for Isotopologues now being empty
        if (it->nIsotopologues() == 0)
            isotopologues_.erase(it);
    }
}

// Return whether Isotopologues for the specified Species exists
bool IsotopologueSet::contains(const Species *sp) const
{
    return std::any_of(isotopologues_.cbegin(), isotopologues_.cend(),
                       [sp](const Isotopologues &mix) { return mix.species() == sp; });
}

// Return IsotopologueSet for the specified Species
OptionalReferenceWrapper<const Isotopologues> IsotopologueSet::getIsotopologues(const Species *sp) const
{
    auto it =
        std::find_if(isotopologues_.cbegin(), isotopologues_.cend(), [sp](const auto &data) { return data.species() == sp; });
    if (it == isotopologues_.end())
        return {};

    return *it;
}

// Return number of species covered by set
int IsotopologueSet::nSpecies() const { return isotopologues_.size(); }

// Return vector of all Isotopologues
std::vector<Isotopologues> &IsotopologueSet::isotopologues() { return isotopologues_; }

const std::vector<Isotopologues> &IsotopologueSet::isotopologues() const { return isotopologues_; }

/*
 * Serialisation
 */

// Read data through specified LineParser
bool IsotopologueSet::deserialise(LineParser &parser, const CoreData &coreData)
{
    clear();

    const auto nSpecies = parser.argi(0);
    for (auto n = 0; n < nSpecies; ++n)
    {
        // Add a new isotopologue set and read it
        isotopologues_.emplace_back();
        if (!isotopologues_.back().deserialise(parser, coreData))
            return false;
    }

    return true;
}

// Write data through specified LineParser
bool IsotopologueSet::write(LineParser &parser)
{
    // Write Configuration name and number of Isotopologues we have defined
    if (!parser.writeLineF("{}\n", isotopologues_.size()))
        return false;

    // Write details for each set of Isotopologues
    for (const auto &topes : isotopologues_)
        if (!topes.serialise(parser))
            return false;

    return true;
}

// Express as a serialisable value
SerialisedValue IsotopologueSet::serialise() const { return isotopologues_; }

// Read values from a serialisable value
void IsotopologueSet::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node, [this, &coreData](const auto &item) { isotopologues_.emplace_back().deserialise(item, coreData); });
}
