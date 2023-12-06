// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "analyser/typeDefs.h"
#include "math/range.h"

// Forward Declarations
class Configuration;
class SiteStack;
class Species;
class SpeciesSite;

// Site Filter
class SiteFilter
{
    public:
    SiteFilter(Configuration *cfg, const Analyser::SiteVector &sitesToFilter);

    /*
     * Targets
     */
    private:
    // Target configuration from which to select sites
    Configuration *configuration_{nullptr};
    // Vector of sites to filter
    const Analyser::SiteVector &targetSites_;

    /*
     * Filter Functions
     */
    public:
    // Filter by neighbour site proximity
    std::pair<Analyser::SiteVector, Analyser::SiteMap> filterBySiteProximity(const Analyser::SiteVector &otherSites,
                                                                             Range range, int minCount, int maxCount) const;
};
