// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "analyser/siteFilter.h"
#include "classes/configuration.h"

SiteFilter::SiteFilter(Configuration *cfg, const SiteSelector::SiteVector &sitesToFilter) : configuration_(cfg), targetSites_(sitesToFilter)
{
}

/*
 * Filter Functions
 */

// Filter by neighbour site proximity
SiteSelector::SiteVector SiteFilter::filterBySiteProximity(const SiteSelector::SiteVector &otherSites, Range range, int minCount, int maxCount) const
{
    SiteSelector::SiteVector filteredSites(targetSites_.size());
    const auto *box = configuration_->box();
    for (auto &&[site,index] : targetSites_)
    {
        auto count = 0;
        for (auto &&[nbr, nbrIndex] : otherSites)
        {
            if (range.contains(box->minimumDistance(site->origin(), nbr->origin())))
                ++count;

            // Check current count against maximum value
            if (count > maxCount)
                break;
        }

        // Accept this site?
        if (count >= minCount && count <= maxCount)
            filteredSites.emplace_back(site, index);
    }

    return filteredSites;
}
