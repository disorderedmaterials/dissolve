/*
    *** Dissolve - Configurations
    *** src/main/configurations.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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

    // Remove any references to the Modules in the Configuration's local processing layer before we delete it
    ListIterator<Module> moduleIterator(cfg->modules());
    while (Module *module = moduleIterator.iterate())
        removeReferencesTo(module);

    // Remove references to the Configuration itself
    removeReferencesTo(cfg);

    // Now safe to remove the Configuration
    coreData_.removeConfiguration(cfg);
}

// Return number of defined Configurations
int Dissolve::nConfigurations() const { return coreData_.nConfigurations(); }

// Return Configuration list
List<Configuration> &Dissolve::configurations() { return coreData_.configurations(); }

// Return Configuration list (const)
const List<Configuration> &Dissolve::constConfigurations() const { return coreData_.configurations(); }

// Find configuration by name
Configuration *Dissolve::findConfiguration(std::string_view name) const { return coreData_.findConfiguration(name); }

// Find configuration by 'nice' name
Configuration *Dissolve::findConfigurationByNiceName(std::string_view name) const
{
    for (auto *cfg = constConfigurations().first(); cfg != nullptr; cfg = cfg->next())
        if (DissolveSys::sameString(name, cfg->niceName()))
            return cfg;

    return nullptr;
}
