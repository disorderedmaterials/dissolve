// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/coreData.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesAngle.h"
#include "classes/speciesBond.h"
#include "classes/speciesTorsion.h"
#include "main/dissolve.h"

// Clear all data
void CoreData::clear()
{
    configurations_.clear();
    species_.clear();
}

/*
 * Species
 */

// Add new Species
Species *CoreData::addSpecies()
{
    auto &newSpecies = species_.emplace_back(std::make_unique<Species>());

    // Create a suitable unique name
    newSpecies->setName(
        DissolveSys::uniqueName("NewSpecies", species_, [&](const auto &sp) { return newSpecies == sp ? "" : sp->name(); }));

    return newSpecies.get();
}

// Remove specified Species
void CoreData::removeSpecies(Species *sp)
{
    species_.erase(std::remove_if(species_.begin(), species_.end(), [&](const auto &p) { return sp == p.get(); }),
                   species_.end());
}

// Return number of Species in list
int CoreData::nSpecies() const { return species_.size(); }

// Return core Species list
std::vector<std::unique_ptr<Species>> &CoreData::species() { return species_; }

const std::vector<std::unique_ptr<Species>> &CoreData::species() const { return species_; }

// Search for Species by name
Species *CoreData::findSpecies(std::string_view name) const
{
    auto it = std::find_if(species_.begin(), species_.end(),
                           [name](const auto &sp) { return DissolveSys::sameString(sp->name(), name); });
    if (it != species_.end())
    {
        return (*it).get();
    }
    else
    {
        return nullptr;
    }
}

/*
 * Configuration
 */

// Add new Configuration
Configuration *CoreData::addConfiguration()
{
    auto &newConfiguration = configurations_.emplace_back(std::make_unique<Configuration>());

    // Create a suitable unique name
    newConfiguration->setName(DissolveSys::uniqueName("NewConfiguration", configurations_, [&](const auto &cfg)
                                                      { return newConfiguration == cfg ? "" : cfg->name(); }));

    return newConfiguration.get();
}

// Remove specified Configuration
void CoreData::removeConfiguration(Configuration *cfg)
{
    configurations_.erase(
        std::remove_if(configurations_.begin(), configurations_.end(), [cfg](const auto &c) { return cfg == c.get(); }),
        configurations_.end());
}

// Return number of Configurations in list
int CoreData::nConfigurations() const { return configurations_.size(); }

// Return core Configurations list
std::vector<std::unique_ptr<Configuration>> &CoreData::configurations() { return configurations_; }

const std::vector<std::unique_ptr<Configuration>> &CoreData::configurations() const { return configurations_; }

// Return nth Configuration in list
Configuration *CoreData::configuration(int n) { return configurations_[n].get(); }

// Search for Configuration by name
Configuration *CoreData::findConfiguration(std::string_view name) const
{
    auto it = std::find_if(configurations_.begin(), configurations_.end(),
                           [&name](const auto &cfg) { return DissolveSys::sameString(cfg->name(), name); });
    if (it == configurations_.end())
        return nullptr;
    return it->get();
}

// Find configuration by 'nice' name
Configuration *CoreData::findConfigurationByNiceName(std::string_view name) const
{
    auto it = std::find_if(configurations().begin(), configurations().end(),
                           [&name](const auto &cfg) { return DissolveSys::sameString(name, cfg->name()); });
    if (it == configurations().end())
        return nullptr;
    return it->get();
}

/*
 * Input Filename
 */

// Set pointer to the current input filename
void CoreData::setInputFilename(std::string_view filename) { inputFilename_ = filename; }

// Return the current input filename
std::string_view CoreData::inputFilename() const { return inputFilename_; }
