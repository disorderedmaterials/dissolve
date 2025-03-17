// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/siteStack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesSite.h"
#include <algorithm>
#include <numeric>

SiteStack::SiteStack()
{
    configuration_ = nullptr;
    configurationIndex_ = -1;
    speciesSite_ = nullptr;
    sitesInMolecules_ = false;
    sitesHaveOrientation_ = false;
}

/*
 * Target
 */

// Return target Configuration
Configuration *SiteStack::configuration() const { return configuration_; }

// Return target SpeciesSite
const SpeciesSite *SiteStack::speciesSite() const { return speciesSite_; }

/*
 * Generation
 */

// Create stack for specified Configuration and site
bool SiteStack::create(Configuration *cfg, const SpeciesSite *site)
{
    // Are we already up-to-date?
    if (configurationIndex_ == cfg->contentsVersion())
        return true;

    // Set the defining information for the stack
    configuration_ = cfg;
    speciesSite_ = site;
    sitesInMolecules_ = true;
    sitesHaveOrientation_ = speciesSite_->hasAxes();

    // Set new index and clear old arrays
    configurationIndex_ = configuration_->contentsVersion();
    sites_.clear();

    const auto &instances = site->instances();
    auto *targetSpecies = site->parent();

    auto sPop = configuration_->speciesPopulation(targetSpecies);
    if (sPop == 0)
        return true;

    sites_.reserve(instances.size() * sPop);

    Vec3<double> origin, x, y;
    const auto *box = configuration_->box();

    for (const auto &molecule : configuration_->molecules())
    {
        if (molecule->species() != targetSpecies)
            continue;

        auto index = 0;
        for (const auto &instance : instances)
        {
            sites_.emplace_back(speciesSite_, index++, molecule, instance, box);
        }
    }
    return true;
}

/*
 * Stack
 */

// Return number of sites in the stack
int SiteStack::nSites() const { return sites_.size(); }

// Return site with index specified
const Site &SiteStack::site(int index) const { return sites_.at(index); }
