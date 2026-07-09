// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/isotopologueSet.h"
#include "classes/species.h"
#include <algorithm>

IsotopologueSet::IsotopologueSet(const std::vector<std::pair<const Isotopologue *, double>> &topes)
{
    for (const auto &[iso, weight] : topes)
        add(iso, weight);
}

/*
 * Data
 */

// Clear all existing data
void IsotopologueSet::clear() { isotopologues_.clear(); }

// Add Isotopologue with the specified relative weight
void IsotopologueSet::add(const Isotopologue *iso, double relativeWeight)
{
    if (!isotopologues_.contains(iso->parent()))
        isotopologues_.set(iso->parent(), ResolvableKeyedVector<const Isotopologue *, double>());

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
bool IsotopologueSet::contains(const Species *sp) const { return isotopologues_.contains(sp); }

// Return Isotopologues with normalised populations for the specified Species
std::map<const Isotopologue *, double> IsotopologueSet::normalisedIsotopologues(const Species *sp) const
{
    if (isotopologues_.contains(sp))
    {
        auto normalised = isotopologues_.value(sp);
        auto sum = std::accumulate(normalised.begin(), normalised.end(), 0.0,
                                   [](const auto acc, const auto &isoWeight) { return acc + isoWeight.second; });
        for (auto &weight : std::views::values(normalised))
            weight /= sum;
        return normalised;
    }

    std::map<const Isotopologue *, double> natural;
    natural[sp->naturalIsotopologue()] = 1.0;
    return natural;
}

// Return number of species covered by set
int IsotopologueSet::nSpecies() const { return isotopologues_.size(); }

// Return vector of all Isotopologues
ResolvableKeyedVector<const Species *, ResolvableKeyedVector<const Isotopologue *, double>> &IsotopologueSet::isotopologues()
{
    return isotopologues_;
}

const ResolvableKeyedVector<const Species *, ResolvableKeyedVector<const Isotopologue *, double>> &
IsotopologueSet::isotopologues() const
{
    return isotopologues_;
}

/*
 * Serialisation
 */

// Express as a serialisable value
void IsotopologueSet::serialise(std::string tag, SerialisedValue &target) const
{
    if (isotopologues_.size() == 0)
        return;

    SerialisedValue value;
    value["set"] = fromVectorToTable(isotopologues_, [](const auto &topes)
                                     { return fromVectorToTable(topes, [](const auto isoWeight) { return isoWeight; }); });
    target[tag] = value;
}

// Read values from a serialisable value
void IsotopologueSet::deserialise(const SerialisedValue &node)
{
    clear();

    toMap(node, "set",
          [&](const std::string &speciesName, const SerialisedValue &topes)
          {
              auto &set = isotopologues_[speciesName];
              toMap(topes, [&](const std::string &isoName, const SerialisedValue &population)
                    { set[isoName] = population.as_floating(); });
          });
}

// Resolve internal resolvable name references with supplied data
void IsotopologueSet::resolve(const std::map<std::string, const Species *> &speciesInScope)
{
    for (auto &[resolvableSpecies, topes] : isotopologues_)
    {
        // Find the named species
        if (speciesInScope.contains(std::string(resolvableSpecies.name())))
            resolvableSpecies.resolve(speciesInScope.at(std::string(resolvableSpecies.name())));
        else
            throw(std::runtime_error(std::format("Species '{}' is used in IsotopologueSet, but no such species is in scope.\n",
                                                 resolvableSpecies.name())));

        // Resolve Isotopologues
        auto *sp = resolvableSpecies.raw();
        for (auto &resolvableIsotopologue : std::views::keys(topes))
        {
            auto *iso = sp->findIsotopologue(resolvableIsotopologue.name());
            if (iso)
                resolvableIsotopologue.resolve(iso);
            else
                throw(std::runtime_error(std::format(
                    "Isotopologue '{}' from species '{}' is used in IsotopologueSet, but no such isotopologue exists.\n",
                    sp->name(), resolvableIsotopologue.name())));
        }
    }
}
