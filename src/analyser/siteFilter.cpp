// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "analyser/siteFilter.h"
#include "classes/configuration.h"

SiteFilter::SiteFilter(Configuration *cfg, const Analyser::SiteVector &sitesToFilter)
    : configuration_(cfg), targetSites_(sitesToFilter)
{
}

/*
 * Filter Functions
 */

// Filter by neighbour site proximity
std::pair<Analyser::SiteVector, Analyser::SiteMap>
SiteFilter::filterBySiteProximity(const Analyser::SiteVector &otherSites, Range range, int minCount, int maxCount) const
{
    Analyser::SiteVector filteredSites(targetSites_.size()), neighbours;
    Analyser::SiteMap filteredSiteMap;
    const auto *box = configuration_->box();
    for (auto &&[site, index] : targetSites_)
    {
        neighbours.clear();
        for (auto &&[nbr, nbrIndex] : otherSites)
        {
            if (range.contains(box->minimumDistance(site->origin(), nbr->origin())))
                neighbours.emplace_back(nbr, nbrIndex);

            // Check current count against maximum value
            if (neighbours.size() > maxCount)
                break;
        }

        // Accept this site?
        if (neighbours.size() >= minCount && neighbours.size() <= maxCount)
        {
            filteredSites.emplace_back(site, index);
            filteredSiteMap[site] = neighbours;
        }
    }

    return {filteredSites, filteredSiteMap};
}
