// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/site.h"
#include "templates/array.h"
#include "templates/listitem.h"

// Forward Declarations
class Configuration;
class SpeciesSite;
class Molecule;

// Site Stack Definition
class SiteStack : public ListItem<SiteStack>
{
    public:
    SiteStack();
    ~SiteStack();

    /*
     * Target
     */
    private:
    // Target Configuration from which we will determine sites
    Configuration *configuration_;
    // Index at which the sites were last calculated for the Configuration
    int configurationIndex_;
    // Target SpeciesSite
    SpeciesSite *speciesSite_;

    public:
    // Create stack for specified Configuration and site
    bool create(Configuration *cfg, SpeciesSite *speciesSite);
    // Return target Configuration
    Configuration *configuration() const;
    // Return target SpeciesSite
    SpeciesSite *speciesSite() const;

    /*
     * Stack
     */
    private:
    // Whether the stack contains associate Molecule information
    bool sitesInMolecules_;
    // Whether the current stack contains local axes information
    bool sitesHaveOrientation_;
    // Basic site array (if no local axes are defined)
    Array<Site> sites_;
    // Oriented site array (if local axes are defined)
    Array<OrientedSite> orientedSites_;

    public:
    // Return number of sites in the stack
    int nSites() const;
    // Return whether the stack contains associate Molecule information
    bool sitesInMolecules() const;
    // Return whether the current stack contains local axes information
    bool sitesHaveOrientation() const;
    // Return site with index specified
    const Site &site(int index) const;
};
