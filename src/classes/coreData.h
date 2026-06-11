// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/version.h"
#include "classes/configuration.h"
#include "classes/pairPotentialOverride.h"
#include "classes/species.h"
#include "data/elements.h"
#include "templates/optionalRef.h"
#include <list>
#include <memory>
#include <optional>
#include <vector>

// Forward Declarations
class Dissolve;
class Module;
class ModuleLayer;

// Core Data Container
class CoreData
{
    public:
    CoreData() = default;
    ~CoreData() = default;
    // Clear all data
    void clear();

    /*
     * Species
     */
    private:
    // Core Species list
    std::vector<std::unique_ptr<Species>> species_;

    public:
    // Add new Species
    Species *addSpecies();
    // Remove specified Species
    void removeSpecies(Species *sp);
    // Return number of Species in list
    int nSpecies() const;
    // Return core Species list
    std::vector<std::unique_ptr<Species>> &species();
    const std::vector<std::unique_ptr<Species>> &species() const;
    // Search for Species by name
    Species *findSpecies(std::string_view name) const;

    /*
     * Configurations
     */
    private:
    // Core Configurations list
    std::vector<std::unique_ptr<Configuration>> configurations_;

    public:
    // Add new Configuration
    Configuration *addConfiguration();
    // Remove specified Configuration
    void removeConfiguration(Configuration *cfg);
    // Return number of Configuration in list
    int nConfigurations() const;
    // Return core Configuration list
    std::vector<std::unique_ptr<Configuration>> &configurations();
    const std::vector<std::unique_ptr<Configuration>> &configurations() const;
    // Return nth Configuration in list
    Configuration *configuration(int n);
    // Search for Configuration by name
    Configuration *findConfiguration(std::string_view name) const;
    // Find configuration by 'nice' name
    Configuration *findConfigurationByNiceName(std::string_view name) const;

    /*
     * Input Filename
     */
    private:
    // Current input filename (mirroring that in Dissolve)
    std::string inputFilename_;

    public:
    // Set current input filename
    void setInputFilename(std::string_view filename);
    // Return the current input filename
    std::string_view inputFilename() const;
};
