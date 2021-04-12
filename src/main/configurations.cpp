// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "classes/box.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include <string.h>

// Add new Configuration
Configuration *Dissolve::addConfiguration()
{
    Configuration *cfg = coreData_.addConfiguration();

    return cfg;
}

// Own specified Configuration
bool Dissolve::ownConfiguration(Configuration *cfg)
{
    // Sanity check - do we already own this Configuration?
    if (coreData_.configurations().contains(cfg))
        return Messenger::error("Already own Configuration '{}', so nothing to do.\n", cfg->name());

    coreData_.configurations().own(cfg);

    return true;
}

// Remove specified Configuration
void Dissolve::removeConfiguration(Configuration *cfg)
{
    if (!cfg)
        return;

    // Remove references to the Configuration itself
    removeReferencesTo(cfg);

    // Now safe to remove the Configuration
    coreData_.removeConfiguration(cfg);
}

// Return number of defined Configurations
int Dissolve::nConfigurations() const { return coreData_.nConfigurations(); }

// Return Configuration list
List<Configuration> &Dissolve::configurations() { return coreData_.configurations(); }

const List<Configuration> &Dissolve::configurations() const { return coreData_.configurations(); }

// Find configuration by name
Configuration *Dissolve::findConfiguration(std::string_view name) const { return coreData_.findConfiguration(name); }

// Find configuration by 'nice' name
Configuration *Dissolve::findConfigurationByNiceName(std::string_view name) const
{
    for (auto *cfg = configurations().first(); cfg != nullptr; cfg = cfg->next())
        if (DissolveSys::sameString(name, cfg->niceName()))
            return cfg;

    return nullptr;
}
