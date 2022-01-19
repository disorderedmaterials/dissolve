// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "module/layer.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "module/module.h"

/*
 * Layer Definition
 */

// Set name of layer
void ModuleLayer::setName(std::string_view name) { name_ = name; }

// Return name of layer
std::string_view ModuleLayer::name() const { return name_; }

// Set whether the layer is enabled
void ModuleLayer::setEnabled(bool enabled) { enabled_ = enabled; }

// Return whether the layer is enabled
bool ModuleLayer::isEnabled() const { return enabled_; }

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
    else if ((!enabled_) || (frequency_ == 0))
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
    if ((frequency_ < 1) || (!enabled_))
        return false;
    else if ((iteration % frequency_) == 0)
        return true;
    else
        return false;
}

/*
 * Modules
 */

// Clear modules
void ModuleLayer::clear() { modules_.clear(); }

// Find associated Module by unique name
Module *ModuleLayer::find(std::string_view uniqueName) const
{
    auto it = std::find_if(modules_.begin(), modules_.end(),
                           [uniqueName](const auto &m) { return DissolveSys::sameString(m->uniqueName(), uniqueName); });
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

/*
 * General Actions
 */

// Run set-up stages for all modules
bool ModuleLayer::setUpAll(Dissolve &dissolve, ProcessPool &procPool)
{
    auto result = true;

    for (auto &module : modules_)
        if (!module->setUp(dissolve, procPool))
            result = false;

    return result;
}
