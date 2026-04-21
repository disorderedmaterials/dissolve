// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/version.h"
#include "classes/configuration.h"
#include "classes/pairPotentialOverride.h"
#include "classes/species.h"
#include "data/elements.h"
#include "module/types.h"
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
     * Atom Types
     */
    private:
    // Core AtomTypes list
    std::vector<std::shared_ptr<AtomType>> atomTypes_;

    public:
    // Add new AtomType
    std::shared_ptr<AtomType> addAtomType(Elements::Element Z);
    // Remove specified AtomType
    void removeAtomType(std::shared_ptr<AtomType> &at);
    // Return number of AtomTypes in list
    int nAtomTypes() const;
    // Return core AtomTypes list
    std::vector<std::shared_ptr<AtomType>> &atomTypes();
    const std::vector<std::shared_ptr<AtomType>> &atomTypes() const;
    // Return nth AtomType in list
    std::shared_ptr<AtomType> atomType(int n);
    // Search for AtomType by name
    std::shared_ptr<AtomType> findAtomType(std::string_view name) const;
    // Clear all atom types
    void clearAtomTypes();

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
    // Copy Species
    Species *copySpecies(const Species *species);

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
     * Layers and Modules
     */
    private:
    // List of defined processing layers
    std::vector<std::unique_ptr<ModuleLayer>> processingLayers_;

    public:
    // Return vector of all existing Modules
    const std::vector<Module *> moduleInstances() const;
    // Search for any instance of any module with the specified unique name
    Module *findModule(std::string_view uniqueName) const;
    // Search for and return any instance(s) of the specified Module type
    std::vector<Module *> allOfType(ModuleTypes::ModuleType type) const;
    std::vector<Module *> allOfType(std::vector<ModuleTypes::ModuleType> types) const;
    template <class M> std::vector<M *> allOfType() const
    {
        std::vector<M *> results;

        for (auto *module : moduleInstances())
        {
            M *castModule = dynamic_cast<M *>(module);
            if (castModule)
                results.push_back(castModule);
        }

        return results;
    }

    // Add new processing layer
    ModuleLayer *addProcessingLayer();
    // Remove processing layer
    void removeProcessingLayer(ModuleLayer *layer);
    // Find named processing layer
    ModuleLayer *findProcessingLayer(std::string_view name) const;
    // Return number of processing layers
    int nProcessingLayers() const;
    // Return current processing layers
    std::vector<std::unique_ptr<ModuleLayer>> &processingLayers();
    const std::vector<std::unique_ptr<ModuleLayer>> &processingLayers() const;
    // Run the set-up stages of all modules in all layers
    bool setUpProcessingLayerModules(Dissolve &dissolve);

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

    /*
     * Object Management
     */
    public:
    // Remove all references to the specified data
    void removeReferencesTo(Configuration *data);
    void removeReferencesTo(Isotopologue *data);
    void removeReferencesTo(Module *data);
    void removeReferencesTo(Species *data);
    void removeReferencesTo(SpeciesSite *data);
    void removeReferencesTo(std::shared_ptr<AtomType> data);
};
