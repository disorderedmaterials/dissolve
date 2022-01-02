// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "main/dissolve.h"

// Remove all references to the specified Configuration
void Dissolve::removeReferencesTo(Configuration *cfg)
{
    // Remove references in keywords
    KeywordStore::objectNoLongerValid<Configuration>(cfg);
}

// Remove all references to the specified Module
void Dissolve::removeReferencesTo(Module *module)
{
    // Remove references in keywords
    KeywordStore::objectNoLongerValid<Module>(module);
}

// Remove all references to the specified Species
void Dissolve::removeReferencesTo(Species *sp)
{
    // Remove references in keywords
    KeywordStore::objectNoLongerValid<Species>(sp);

    // Check Configurations - if the Species was used, we must clear the configuration contents
    for (auto &cfg : configurations())
        if (cfg->containsSpecies(sp))
            cfg->empty();
}

// Remove all references to the specified SpeciesSite
void Dissolve::removeReferencesTo(SpeciesSite *site)
{
    // Remove references in keywords
    KeywordStore::objectNoLongerValid<SpeciesSite>(site);
}
