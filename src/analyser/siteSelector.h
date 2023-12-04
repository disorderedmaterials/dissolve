// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/site.h"
#include <memory>

// Forward Declarations
class Configuration;
class SiteStack;
class Species;
class SpeciesSite;

// Site Selector
class SiteSelector
{
    public:
    SiteSelector(Configuration *cfg, const std::vector<const SpeciesSite *> &sites);

    // Typedefs
    using SiteData = std::tuple<const Site *, int>;
    using SiteVector = std::vector<SiteData>;

    private:
    // Target configuration from which to select sites
    Configuration *configuration_{nullptr};
    // Vector of sites to select
    std::vector<const SpeciesSite *> speciesSites_;
    // Vector of selected sites
    SiteVector sites_;

    public:
    // Return vector of selected sites
    const SiteVector &sites() const;
};
