// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

// Forward Declarations
class Dissolve;
class GenericList;
class Module;
class ProcessPool;

// Module Layer
class ModuleLayer
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
    // Whether the layer is enabled
    bool enabled_{true};
    // Frequency, relative to the main iteration counter, at which to execute the layer
    int frequency_{1};

    public:
    // Set name of layer
    void setName(std::string_view name);
    // Return name of layer
    std::string_view name() const;
    // Set whether the layer is enabled
    void setEnabled(bool enabled);
    // Return whether the layer is enabled
    bool isEnabled() const;
    // Frequency, relative to the main iteration counter, at which to execute the layer
    void setFrequency(int frequency);
    // Return frequency, relative to the main iteration counter, at which to execute the layer
    int frequency() const;
    // Return short descriptive text relating frequency to supplied iteration number
    std::string frequencyDetails(int iteration) const;
    // Return whether the layer should execute this iteration
    bool runThisIteration(int iteration) const;

    /*
     * Modules
     */
    private:
    // Vector of Modules in the layer
    std::vector<std::unique_ptr<Module>> modules_;

    public:
    // Clear modules
    void clear();
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
    bool setUpAll(Dissolve &dissolve, ProcessPool &procPool);
};
