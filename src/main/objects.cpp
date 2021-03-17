// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"

// Remove all references to the specified Configuration
void Dissolve::removeReferencesTo(Configuration *cfg)
{
    // Remove references in keywords
    KeywordBase::objectNoLongerValid<Configuration>(cfg);

    // Remove references (targets) in Modules
    for (Module *module : moduleInstances_)
        if (module->isTargetConfiguration(cfg))
            module->removeTargetConfiguration(cfg);
}

// Remove all references to the specified Module
void Dissolve::removeReferencesTo(Module *module)
{
    // Remove references in keywords
    KeywordBase::objectNoLongerValid<Module>(module);
}

// Remove all references to the specified Species
void Dissolve::removeReferencesTo(Species *sp)
{
    // Remove references in keywords
    KeywordBase::objectNoLongerValid<Species>(sp);

    // Check Configurations - if the Species was used, we must clear the configuration contents
    ListIterator<Configuration> configIterator(configurations());
    while (Configuration *cfg = configIterator.iterate())
        if (cfg->hasUsedSpecies(sp))
            cfg->empty();
}

// Remove all references to the specified SpeciesSite
void Dissolve::removeReferencesTo(SpeciesSite *site)
{
    // Remove references in keywords
    KeywordBase::objectNoLongerValid<SpeciesSite>(site);
}
