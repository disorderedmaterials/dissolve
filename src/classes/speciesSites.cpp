// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/speciesSites.h"
#include "base/lineParser.h"
#include "classes/species.h"

SpeciesSites::SpeciesSites(const std::vector<const SpeciesSite *> &sites)
{
    for (const auto &site : sites)
        add(site);
}

/*
 * Data
 */

// Clear all existing data
void SpeciesSites::clear() { sites_.clear(); }

// Add Isotopologue with the specified relative weight
void SpeciesSites::add(const SpeciesSite *site)
{
    if (!sites_.contains(site->parent()))
        sites_.set(site->parent(), ResolvableKeyedVector<const SpeciesSite *, double>());

    sites_[site->parent()].add(site, 1.0);
}

// Remove specified site from the list (if it exists)
void SpeciesSites::remove(const SpeciesSite *site)
{
    if (sites_.contains(site->parent()))
    {
        sites_[site->parent()].erase(site);

        // Check for species entry now being empty
        if (sites_[site->parent()].size() == 0)
            sites_.erase(site->parent());
    }
}

// Return vector of all Isotopologues
ResolvableKeyedVector<const Species *, ResolvableKeyedVector<const SpeciesSite *, double>> &SpeciesSites::sites()
{
    return sites_;
}

const ResolvableKeyedVector<const Species *, ResolvableKeyedVector<const SpeciesSite *, double>> &SpeciesSites::sites() const
{
    return sites_;
}

// Return straight vector of all sites
std::vector<const SpeciesSite *> SpeciesSites::getSpeciesSites() const
{
    std::vector<const SpeciesSite *> result;
    for (const auto &[species, sites] : sites_)
        std::ranges::transform(std::views::keys(sites), std::back_inserter(result),
                               [](const auto &resolvable) { return resolvable.raw(); });
    return result;
}

/*
 * Serialisation
 */

// Express as a serialisable value
void SpeciesSites::serialise(std::string tag, SerialisedValue &target) const
{
    if (sites_.size() == 0)
        return;

    SerialisedValue value;
    value["sites"] = fromVectorToTable(sites_, [](const auto &sites)
                                       { return fromVectorToTable(sites, [](const auto isoWeight) { return isoWeight; }); });
    target[tag] = value;
}

// Read values from a serialisable value
void SpeciesSites::deserialise(const SerialisedValue &node)
{
    clear();

    toMap(node, "set",
          [&](const std::string &speciesName, const SerialisedValue &sites)
          {
              auto &set = sites_[speciesName];
              toMap(sites, [&](const std::string &siteName, const SerialisedValue &population)
                    { set[siteName] = population.as_floating(); });
          });
}

// Resolve internal resolvable name references with supplied data
void SpeciesSites::resolve(const std::map<std::string, const Species *> &speciesInScope)
{
    for (auto &[resolvableSpecies, topes] : sites_)
    {
        // Find the named species
        if (speciesInScope.contains(std::string(resolvableSpecies.name())))
            resolvableSpecies.resolve(speciesInScope.at(std::string(resolvableSpecies.name())));
        else
            throw(std::runtime_error(std::format("Species '{}' is used in SpeciesSites, but no such species is in scope.\n",
                                                 resolvableSpecies.name())));

        // Resolve SpeciesSites
        auto *sp = resolvableSpecies.raw();
        for (auto &resolvableSpeciesSite : std::views::keys(topes))
        {
            auto *site = sp->findSite(resolvableSpeciesSite.name());
            if (site)
                resolvableSpeciesSite.resolve(site);
            else
                throw(std::runtime_error(
                    std::format("Species site '{}' from species '{}' is used in SpeciesSites, but no such site exists.\n",
                                sp->name(), resolvableSpeciesSite.name())));
        }
    }
}
