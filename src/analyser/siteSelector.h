// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/typeDefs.h"
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

    private:
    // Target configuration from which to select sites
    Configuration *configuration_{nullptr};
    // Vector of sites to select
    std::vector<const SpeciesSite *> speciesSites_;
    // Vector of selected sites
    Analyser::SiteVector sites_;

    public:
    // Return vector of selected sites
    const Analyser::SiteVector &sites() const;
};
