// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "analyser/siteSelector.h"
#include "classes/configuration.h"

SiteSelector::SiteSelector(Configuration *cfg, const std::vector<const SpeciesSite *> &sites)
    : configuration_(cfg), speciesSites_(sites)
{
    auto siteIndex = 0;
    for (auto *spSite : speciesSites_)
    {
        const auto *siteStack = configuration_->siteStack(spSite);
        if (siteStack == nullptr)
            return;

        for (auto n = 0; n < siteStack->nSites(); ++n)
            sites_.emplace_back(&siteStack->site(n), ++siteIndex);
    }
}

// Return vector of selected sites
const Analyser::SiteVector &SiteSelector::sites() const { return sites_; }
