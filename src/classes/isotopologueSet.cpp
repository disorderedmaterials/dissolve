// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

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
    if (!isotopologues_.contains(iso->parent()))
        isotopologues_.set(iso->parent(), KeyedVector<const Isotopologue *, double>());

    isotopologues_[iso->parent()].add(iso, relativeWeight);
}

// Remove specified Species from the list (if it exists)
void IsotopologueSet::remove(const Species *sp)
{
    if (!isotopologues_.contains(sp))
    isotopologues_.erase(sp);
}

// Remove any occurrences of the specified Isotopologue
void IsotopologueSet::remove(const Isotopologue *iso)
{
    if (isotopologues_.contains(iso->parent()))
    {
        isotopologues_[iso->parent()].erase(iso);

        // Check for Isotopologues now being empty
        if (isotopologues_[iso->parent()].size() == 0)
            isotopologues_.erase(iso->parent());
    }
}

// Return whether Isotopologues for the specified Species exists
bool IsotopologueSet::contains(const Species *sp) const
{
    return isotopologues_.contains(sp);
}

// Return Isotopologues with normalised populations for the specified Species
const KeyedVector<const Isotopologue *, double> IsotopologueSet::normalisedIsotopologues(const Species *sp) const
{
    if (isotopologues_.contains(sp))
        return isotopologues_.value(sp);

    KeyedVector<const Isotopologue *, double> natural;
    natural.add(sp->naturalIsotopologue(), 1.0);
    return natural;
}

// Return number of species covered by set
int IsotopologueSet::nSpecies() const { return isotopologues_.size(); }

// Return vector of all Isotopologues
ResolvableKeyedVector<const Species*, KeyedVector<const Isotopologue *, double>>  &IsotopologueSet::isotopologues() { return isotopologues_; }

const ResolvableKeyedVector<const Species*, KeyedVector<const Isotopologue *, double>> &IsotopologueSet::isotopologues() const { return isotopologues_; }

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
        // Read Species name
        if (parser.getArgsDelim() != LineParser::Success)
            return false;
        auto species = coreData.findSpecies(parser.argsv(0));
        if (species == nullptr)
        {
            Messenger::error("Failed to find Species '{}' while reading IsotopologueSet.\n", parser.argsv(0));
            return false;
        }

        // Read isotopologues
        KeyedVector<const Isotopologue *, double> topes;
        auto nIso = parser.argi(1);
        for (auto n = 0; n < nIso; ++n)
        {
            if (parser.getArgsDelim() != LineParser::Success)
                return false;

            // Search for the named Isotopologue in the Species
            const auto iso = species->findIsotopologue(parser.argsv(0));
            if (!iso)
            {
                Messenger::error("Failed to find Isotopologue '{}' for Species '{}' while reading Isotopologues.\n",
                                 parser.argsv(0), species->name());
                return false;
            }

            topes.set(iso, parser.argd(1));
        }

        isotopologues_.set(species, topes);
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
    for (const auto &[species, topes] : isotopologues_)
    {
        if (!parser.writeLineF("'{}'  {}\n", species.raw()->name(), topes.size()))
            return false;

        // Write Isotopologues
        for (const auto &[iso, weight] : topes)
            if (!parser.writeLineF("{}  {}\n", iso->name(), weight))
                return false;
    }

    return true;
}

// Express as a serialisable value
void IsotopologueSet::serialise(std::string tag, SerialisedValue &target) const
{
    if (isotopologues_.size() == 0)
        return;

    SerialisedValue value;
    // for (const auto &[species, topes] : isotopologues_)
    // {
    //     // SerialisedValue mix;
    //     // for (const auto &[iso, weight] : topes)
    //         // mix[std::string(iso->name())] = weight;
    //
    //     value[std::string(species.name())] = fromVectorToTable(topes, [](const auto &iso) { return iso->name(); });
    // }

    value["set"] = fromVectorToTable(isotopologues_, [](const auto &topes) { return fromVectorToTable(topes, [](const auto &iso) { return iso->name(); }); });
    target[tag] = value;
}

// Read values from a serialisable value
void IsotopologueSet::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    clear();

    toMap(node, "set", [&](const std::string &name, const SerialisedValue &topes)
                    { isotopologues_[name] = toMap(topes, [](const std::string &name, const SerialisedValue &population) { }); });
    toVector(node, [this, &coreData](const auto &item) { isotopologues_.emplace_back().deserialise(item, coreData); });
}
