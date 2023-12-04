// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "math/range.h"
#include "analyser/siteSelector.h"
#include <memory>

// Forward Declarations
class Configuration;
class SiteStack;
class Species;
class SpeciesSite;

// Site Filter
class SiteFilter
{
    public:
    SiteFilter(Configuration *cfg, const SiteSelector::SiteVector &sitesToFilter);

    /*
     * Targets
     */
    private:
    // Target configuration from which to select sites
    Configuration *configuration_{nullptr};
    // Vector of sites to filter
    const SiteSelector::SiteVector &targetSites_;

    /*
     * Filter Functions
     */
    public:
    // Filter by neighbour site proximity
    SiteSelector::SiteVector filterBySiteProximity(const SiteSelector::SiteVector &otherSites, Range range, int minCount, int maxCount) const;
};
