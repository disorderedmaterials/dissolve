// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "templates/flags.h"
#include "base/serialiser.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

// Forward Declarations
class Configuration;
class Dissolve;
class GenericList;
class Module;
class ProcessPool;

// Module Layer
class ModuleLayer : public Serialisable
{
    public:
    ModuleLayer() = default;
    ~ModuleLayer() = default;

    /*
     * Layer Definition
     */
    private:
    // Name of layer
    std::string name_{"Untitled Layer"};
    // Frequency, relative to the main iteration counter, at which to execute the layer
    int frequency_{1};

    public:
    // Set name of layer
    void setName(std::string_view name);
    // Return name of layer
    std::string_view name() const;
    // Frequency, relative to the main iteration counter, at which to execute the layer
    void setFrequency(int frequency);
    // Return frequency, relative to the main iteration counter, at which to execute the layer
    int frequency() const;
    // Return short descriptive text relating frequency to supplied iteration number
    std::string frequencyDetails(int iteration) const;
    // Return whether the layer should execute this iteration
    bool runThisIteration(int iteration) const;

    /*
     * Run Control
     */
    public:
    // Run Control Flags
    enum RunControlFlag
    {
        Disabled,        /* Layer is disabled and will never run */
        EnergyStability, /* Only run if the energy of all relevant configurations is stable */
        SizeFactors      /* Only run if the size factors of all relevant configurations are 1.0 */
    };

    private:
    // Flags controlling run status
    Flags<RunControlFlag> runControlFlags_;

    public:
    // Return flags controlling run status
    Flags<RunControlFlag> &runControlFlags();
    Flags<RunControlFlag> runControlFlags() const;
    // Return whether all run control flags are satisfied and permit the layer to be run
    bool canRun(GenericList &processingModuleData) const;

    /*
     * Modules
     */
    private:
    // Vector of Modules in the layer
    std::vector<std::unique_ptr<Module>> modules_;

    public:
    // Clear modules
    void clear();
    // Append new module to this layer
    Module *append(std::string_view moduleType, const std::vector<std::unique_ptr<Configuration>> &cfgs);
    // Find associated Module by unique name
    Module *find(std::string_view uniqueName) const;
    // Return whether specified Module is present in the layer
    bool contains(Module *searchModule) const;
    // Return vector of Modules
    std::vector<std::unique_ptr<Module>> &modules();
    // Return map of modules in the layer, optionally preceding the specified module
    std::map<std::string, std::vector<const Module *>> modulesAsMap(const Module *beforeThis = nullptr) const;

    /*
     * General Actions
     */
    public:
    // Run set-up stages for all modules
    bool setUpAll(Dissolve &dissolve, const ProcessPool &procPool);
    // Return all configurations targeted by modules in the layer
    std::vector<Configuration *> allTargetedConfigurations() const;
    SerialisedValue serialise() const override;
    void deserialise(const SerialisedValue &node) override;
};
