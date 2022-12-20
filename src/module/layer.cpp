// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "module/layer.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "module/module.h"
#include "modules/energy/energy.h"
#include "modules/registry.h"

/*
 * Layer Definition
 */

// Set name of layer
void ModuleLayer::setName(std::string_view name) { name_ = name; }

// Return name of layer
std::string_view ModuleLayer::name() const { return name_; }

// Frequency, relative to the main iteration counter, at which to execute the layer
void ModuleLayer::setFrequency(int frequency) { frequency_ = frequency; }

// Return frequency, relative to the main iteration counter, at which to execute the layer
int ModuleLayer::frequency() const { return frequency_; }

// Return short descriptive text relating frequency to supplied iteration number
std::string ModuleLayer::frequencyDetails(int iteration) const
{
    // Check edge cases
    if (frequency_ < 0)
        return "NEGATIVE?";
    else if (runControlFlags_.isSet(ModuleLayer::RunControlFlag::Disabled) || (frequency_ == 0))
        return "disabled";
    else if (frequency_ == 1)
        return "every time";
    else if ((iteration % frequency_) == 0)
        return "this iteration";

    // Calculate number of steps necessary to get to next multiple of the frequency_
    auto nToGo = frequency_ - (iteration - frequency_ * (iteration / frequency_));
    if (nToGo == 1)
        return "next iteration";

    return fmt::format("in {} steps time", nToGo);
}

// Return whether the layer should execute this iteration
bool ModuleLayer::runThisIteration(int iteration) const
{
    if ((frequency_ < 1) || runControlFlags_.isSet(ModuleLayer::RunControlFlag::Disabled))
        return false;
    else if ((iteration % frequency_) == 0)
        return true;
    else
        return false;
}

/*
 * Run Control
 */

// Return flags controlling run status
Flags<ModuleLayer::RunControlFlag> &ModuleLayer::runControlFlags() { return runControlFlags_; }
Flags<ModuleLayer::RunControlFlag> ModuleLayer::runControlFlags() const { return runControlFlags_; }

// Return whether all run control flags are satisfied and permit the layer to be run
bool ModuleLayer::canRun(GenericList &processingModuleData) const
{
    if (runControlFlags_.isSet(ModuleLayer::RunControlFlag::Disabled))
        return false;

    auto cfgs = allTargetedConfigurations();

    if (runControlFlags_.isSet(ModuleLayer::RunControlFlag::EnergyStability))
    {
        if (EnergyModule::nUnstable(processingModuleData, cfgs) != 0)
        {
            Messenger::print("One or more configurations have unstable energy, so the layer will not run.\n");
            return false;
        }
    }

    if (runControlFlags_.isSet(ModuleLayer::RunControlFlag::SizeFactors))
    {
        // Check that Configurations have unmodified size factor
        if (std::any_of(cfgs.begin(), cfgs.end(),
                        [](const auto *cfg)
                        { return std::abs(cfg->appliedSizeFactor() - 1.0) > 2 * std::numeric_limits<double>::epsilon(); }))
        {
            Messenger::print("One or more configurations have an applied size factor, so the layer will not run.\n");
            return false;
        }
    }

    return true;
}

/*
 * Modules
 */

// Clear modules
void ModuleLayer::clear() { modules_.clear(); }

// Append new module to this layer
Module *ModuleLayer::append(std::string_view moduleType, const std::vector<std::unique_ptr<Configuration>> &cfgs)
{
    auto *module = ModuleRegistry::create(moduleType, this);
    module->setTargets(cfgs, modulesAsMap());
    return module;
}

// Find associated Module by unique name
Module *ModuleLayer::find(std::string_view uniqueName) const
{
    auto it = std::find_if(modules_.begin(), modules_.end(),
                           [uniqueName](const auto &m) { return DissolveSys::sameString(m->name(), uniqueName); });
    if (it != modules_.end())
        return it->get();

    return nullptr;
}

// Return whether specified Module is present in the layer
bool ModuleLayer::contains(Module *searchModule) const
{
    return std::find_if(modules_.begin(), modules_.end(), [searchModule](const auto &m) { return searchModule == m.get(); }) !=
           modules_.end();
}

// Return vector of Modules
std::vector<std::unique_ptr<Module>> &ModuleLayer::modules() { return modules_; }

// Return map of modules in the layer, optionally preceding the specified module
std::map<std::string, std::vector<const Module *>> ModuleLayer::modulesAsMap(const Module *beforeThis) const
{
    std::map<std::string, std::vector<const Module *>> moduleMap;

    for (auto &module : modules_)
    {
        if (module.get() == beforeThis)
            break;
        moduleMap[std::string(module->type())].emplace_back(module.get());
    }

    return moduleMap;
}

/*
 * General Actions
 */

// Run set-up stages for all modules
bool ModuleLayer::setUpAll(Dissolve &dissolve, const ProcessPool &procPool)
{
    auto result = true;

    for (auto &module : modules_)
        if (!module->setUp(dissolve, procPool))
            result = false;

    return result;
}

// Return all configurations targeted by modules in the layer
std::vector<Configuration *> ModuleLayer::allTargetedConfigurations() const
{
    std::vector<Configuration *> result;

    for (auto &module : modules_)
    {
        if (module->keywords().find("Configuration"))
        {
            auto *cfg = module->keywords().getConfiguration("Configuration");
            if (cfg)
                result.push_back(cfg);
        }

        if (module->keywords().find("Configurations"))
        {
            auto cfgs = module->keywords().getVectorConfiguration("Configurations");
            std::copy(cfgs.begin(), cfgs.end(), std::back_inserter(result));
        }
    }

    return result;
}