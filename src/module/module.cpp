// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "module/module.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/configuration.h"

// Static Singletons
std::vector<Module *> Module::instances_;

Module::Module(std::string typeName) : typeName_(typeName), frequency_(1), enabled_(true) { instances_.push_back(this); }

Module::~Module() { instances_.erase(std::remove(instances_.begin(), instances_.end(), this)); }

/*
 * Definition
 */

// Return type of Module
const std::string_view Module::type() const { return typeName_; }

// Set name of Module
void Module::setName(std::string_view uniqueName) { name_ = uniqueName; }

// Return unique name of Module
std::string_view Module::name() const { return name_; }

/*
 * Keywords
 */

// Return list of recognised keywords
KeywordStore &Module::keywords() { return keywords_; }
const KeywordStore &Module::keywords() const { return keywords_; };

// Print valid keywords
void Module::printValidKeywords()
{
    Messenger::print("Valid keywords for '{}' Module are:\n", type());

    for (auto &[name, keyword] : keywords_.keywords())
        Messenger::print("  {:30}  {}\n", name, keyword->description());
}

/*
 * Control
 */

// Set frequency at which to run Module (relative to master simulation loop counter)
void Module::setFrequency(int freq) { frequency_ = freq; }

// Return frequency at which to run Module (relative to master simulation loop counter)
int Module::frequency() const { return frequency_; }

// Return whether the Module should run this iteration
bool Module::runThisIteration(int iteration) const
{
    if ((frequency_ < 1) || (!enabled_))
        return false;
    else if ((iteration % frequency_) == 0)
        return true;
    else
        return false;
}

// Return short descriptive text relating frequency to supplied iteration number
std::string Module::frequencyDetails(int iteration) const
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

// Set whether the Module is enabled
void Module::setEnabled(bool b) { enabled_ = b; }

// Return whether the Module is enabled
bool Module::isEnabled() const { return enabled_; }

// Return whether the Module is disabled
bool Module::isDisabled() const { return !enabled_; }

/*
 * Processing
 */

// Run main processing
bool Module::process(Dissolve &dissolve, const ProcessPool &procPool) { return false; }

// Set target data
void Module::setTargets(const std::vector<std::unique_ptr<Configuration>> &configurations,
                        const std::map<std::string, std::vector<const Module *>> &moduleMap)
{
    // Search for Configuration-based targets
    if (!configurations.empty())
        for (auto &target : keywords_.targetsGroup())
        {
            if (target->typeIndex() == typeid(ConfigurationKeyword *))
                keywords_.set(target->name(), configurations.front().get());
            else if (target->typeIndex() == typeid(ConfigurationVectorKeyword *))
            {
                std::vector<Configuration *> rawCfgs(configurations.size());
                std::transform(configurations.begin(), configurations.end(), rawCfgs.begin(),
                               [](const auto &unique) { return unique.get(); });
                keywords_.set(target->name(), rawCfgs);
            }
        }
}

// Run set-up stage
bool Module::setUp(Dissolve &dissolve, const ProcessPool &procPool, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    return true;
}

// Run main processing stage
bool Module::executeProcessing(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Begin timer
    Timer timer;
    timer.start();

    // Run main processing routine
    auto result = process(dissolve, procPool);

    // Accumulate timing information
    timer.stop();
    processTimes_ += timer.secondsElapsed();

    return result;
}

/*
 * Timing
 */

// Return timing information (in seconds) for this Module
SampledDouble Module::processTimes() const { return processTimes_; }

// Read timing information through specified parser
bool Module::readProcessTimes(LineParser &parser) { return processTimes_.deserialise(parser); }

/*
 * Management
 */

// Return vector of all existing Modules
const std::vector<Module *> &Module::instances() { return instances_; }

// Search for any instance of any module with the specified unique name
Module *Module::find(std::string_view uniqueName)
{
    auto it = std::find_if(instances_.begin(), instances_.end(),
                           [uniqueName](const auto *m) { return DissolveSys::sameString(m->name(), uniqueName); });
    if (it != instances_.end())
        return *it;

    return nullptr;
}

// Search for and return any instance(s) of the specified Module type
std::vector<Module *> Module::allOfType(std::string_view moduleType)
{
    return allOfType(std::vector<std::string>{std::string{moduleType}});
}

std::vector<Module *> Module::allOfType(std::vector<std::string> types)
{
    std::vector<Module *> modules;
    std::copy_if(instances_.begin(), instances_.end(), std::back_inserter(modules),
                 [&types](const auto *m) { return std::find(types.begin(), types.end(), m->type()) != types.end(); });
    return modules;
}

// Express as a tree node
SerialisedValue Module::serialise() const
{
    SerialisedValue result = {{"name", name_}, {"type", typeName_}};
    if (!enabled_)
        result["disabled"] = true;
    return keywords_.serialiseOnto(result);
}

// Read values from a tree node
void Module::deserialise(const SerialisedValue &node, const CoreData &data)
{
    name_ = toml::find<std::string>(node, "name");
    enabled_ = toml::find_or<bool>(node, "disabled", false);

    keywords_.deserialiseFrom(node, data);
}