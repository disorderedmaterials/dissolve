// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/site.h"

// Forward Declarations
class Box;
class Configuration;
class SpeciesSite;
class Molecule;

// Site Stack Definition
class SiteStack
{
    public:
    SiteStack();
    ~SiteStack() = default;

    /*
     * Target
     */
    private:
    // Target Configuration from which we will determine sites
    Configuration *configuration_;
    // Index at which the sites were last calculated for the Configuration
    int configurationIndex_;
    // Target SpeciesSite
    const SpeciesSite *speciesSite_;

    public:
    // Return target Configuration
    Configuration *configuration() const;
    // Return target SpeciesSite
    const SpeciesSite *speciesSite() const;

    /*
     * Creation
     */
    public:
    // Create stack for specified Configuration and site
    bool create(Configuration *cfg, const SpeciesSite *site);

    /*
     * Stack
     */
    private:
    // Whether the stack contains associate Molecule information
    bool sitesInMolecules_;
    // Whether the current stack contains local axes information
    bool sitesHaveOrientation_;
    // Site array
    std::vector<Site> sites_;

    public:
    // Return number of sites in the stack
    int nSites() const;
    // Return site with index specified
    const Site &site(int index) const;
};
