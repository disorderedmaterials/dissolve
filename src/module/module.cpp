// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "module/module.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/coreData.h"
#include "keywords/configuration.h"
#include "main/dissolve.h"
#include "module/context.h"

// Module Types

namespace ModuleTypes
{
// ENumerated Options for ModuleTypes
EnumOptions<ModuleTypes::ModuleType> moduleTypes_("ModuleType", {{ModuleTypes::Accumulate, "Accumulate"},
                                                                 {ModuleTypes::Analyse, "Analyse"},
                                                                 {ModuleTypes::Angle, "Angle"},
                                                                 {ModuleTypes::AtomShake, "AtomShake"},
                                                                 {ModuleTypes::AvgMol, "AvgMol"},
                                                                 {ModuleTypes::AxisAngle, "AxisAngle"},
                                                                 {ModuleTypes::Benchmark, "Benchmark"},
                                                                 {ModuleTypes::Bragg, "Bragg"},
                                                                 {ModuleTypes::Checks, "Checks"},
                                                                 {ModuleTypes::CheckSpecies, "CheckSpecies"},
                                                                 {ModuleTypes::Compare, "Compare"},
                                                                 {ModuleTypes::DAngle, "DAngle"},
                                                                 {ModuleTypes::DataTest, "DataTest"},
                                                                 {ModuleTypes::Energy, "Energy"},
                                                                 {ModuleTypes::EPSR, "EPSR"},
                                                                 {ModuleTypes::EPSRManager, "EPSRManager"},
                                                                 {ModuleTypes::ExportCoordinates, "ExportCoordinates"},
                                                                 {ModuleTypes::ExportPairPotentials, "ExportPairPotentials"},
                                                                 {ModuleTypes::ExportTrajectory, "ExportTrajectory"},
                                                                 {ModuleTypes::Forces, "Forces"},
                                                                 {ModuleTypes::GeometryOptimisation, "GeometryOptimisation"},
                                                                 {ModuleTypes::GR, "GR"},
                                                                 {ModuleTypes::HistogramCN, "HistogramCN"},
                                                                 {ModuleTypes::ImportTrajectory, "ImportTrajectory"},
                                                                 {ModuleTypes::IntraAngle, "IntraAngle"},
                                                                 {ModuleTypes::IntraDistance, "IntraDistance"},
                                                                 {ModuleTypes::IntraShake, "IntraShake"},
                                                                 {ModuleTypes::MD, "MD"},
                                                                 {ModuleTypes::ModifierOSites, "ModifierOSites"},
                                                                 {ModuleTypes::MolShake, "MolShake"},
                                                                 {ModuleTypes::NeutronSQ, "NeutronSQ"},
                                                                 {ModuleTypes::OrientedSDF, "OrientedSDF"},
                                                                 {ModuleTypes::QSpecies, "QSpecies"},
                                                                 {ModuleTypes::SDF, "SDF"},
                                                                 {ModuleTypes::SiteRDF, "SiteRDF"},
                                                                 {ModuleTypes::SQ, "SQ"},
                                                                 {ModuleTypes::TemperatureSchedule, "TemperatureSchedule"},
                                                                 {ModuleTypes::Test, "Test"},
                                                                 {ModuleTypes::XRaySQ, "XRaySQ"}});

// Return module type string for specified type enumeration
std::string moduleType(ModuleTypes::ModuleType type) { return moduleTypes_.keyword(type); }
// Return module type enumeration for specified module type string
std::optional<ModuleTypes::ModuleType> moduleType(std::string_view keyword)
{
    if (moduleTypes_.isValid(keyword))
        return moduleTypes_.enumeration(keyword);
    else
        return {};
};
} // namespace ModuleTypes

Module::Module(const ModuleTypes::ModuleType type) : type_(type), frequency_(1), enabled_(true) {}

Module::~Module() {}

/*
 * Definition
 */

// Return type of Module
ModuleTypes::ModuleType Module::type() const { return type_; }

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
    Messenger::print("Valid keywords for '{}' Module are:\n", ModuleTypes::moduleType(type()));

    for (const auto &keywordSection : keywords_.sections())
        for (const auto &keywordGroup : keywordSection.groups())
            for (const auto &[keyword, keywordType] : keywordGroup.keywords())
                if (keywordType != KeywordBase::KeywordType::Deprecated)
                    Messenger::print("  {:30}  {}\n", keyword->name(), keyword->description());
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

// Get current target configurations
std::pair<std::vector<const Configuration *>, int> Module::getCurrentTargetConfigurations()
{
    auto expectedTargetCount = 0;
    std::vector<const Configuration *> currentTargets;
    for (auto *keyword : keywords_.targetKeywords())
    {
        if (keyword->typeIndex() == typeid(ConfigurationKeyword *))
        {
            ++expectedTargetCount;
            auto optCfg = keywords_.get<Configuration *, ConfigurationKeyword>(keyword->name());
            if (optCfg)
                currentTargets.push_back(*optCfg);
            else
                throw(std::runtime_error("Failed to get data from ConfigurationKeyword when checking targets.\n"));
        }
        else if (keyword->typeIndex() == typeid(ConfigurationVectorKeyword *))
        {
            ++expectedTargetCount;
            auto optCfgs = keywords_.get<std::vector<Configuration *>, ConfigurationVectorKeyword>(keyword->name());
            if (optCfgs)
                currentTargets.insert(currentTargets.end(), optCfgs->begin(), optCfgs->end());
            else
                throw(std::runtime_error("Failed to get data from ConfigurationVectorKeyword when checking targets.\n"));
        }
    }

    return {currentTargets, expectedTargetCount};
}

// Check the current configurations targeted by the module
Module::ExecutionResult Module::checkConfigurationTargets(GenericList &processingModuleData)
{
    // Assemble target configurations
    auto &&[currentTargets, expectedTargetCount] = getCurrentTargetConfigurations();

    // If we are expecting targets, make sure we actually have them
    if (expectedTargetCount > 0)
    {
        // Check basic target count
        if (currentTargets.size() < expectedTargetCount)
        {
            Messenger::error("Not enough configuration targets set for module '{}'.\n", name());
            return ExecutionResult::Failed;
        }

        // Check that the current targets are consistent with the ones we last used
        if (!lastProcessedConfigurations_.empty())
        {
            // If the vector of previous targets isn't the same size as the current targets then we must clear our data
            if (currentTargets.size() != lastProcessedConfigurations_.size() ||
                !std::all_of(currentTargets.begin(), currentTargets.end(),
                             [&](const auto *currentTarget)
                             {
                                 return std::find_if(lastProcessedConfigurations_.begin(), lastProcessedConfigurations_.end(),
                                                     [currentTarget](const auto &pair) {
                                                         return pair.first == currentTarget;
                                                     }) != lastProcessedConfigurations_.end();
                             }))
            {
                Messenger::warn("Target configuration(s) have changed for module '{}' so processing data for that module will "
                                "be cleared...\n");
                processingModuleData.removeWithPrefix(name());
                lastProcessedConfigurations_.clear();
            }
            else if (!executeIfTargetsUnchanged_)
            {
                // Targets are the same - are _all_ versions different?
                if (std::any_of(currentTargets.begin(), currentTargets.end(),
                                [&](const auto *currentTarget)
                                { return lastProcessedConfigurations_[currentTarget] == currentTarget->contentsVersion(); }))
                {
                    Messenger::warn("One or more target configurations have not changed since module '{}' was last run, so it "
                                    "will not run in the current iteration.\n",
                                    name());
                    return ExecutionResult::NotExecuted;
                }
            }
        }
    }

    return ExecutionResult::Success;
}

// Run main processing
Module::ExecutionResult Module::process(ModuleContext &moduleContext) { return ExecutionResult::Failed; }

// Set target data
void Module::setTargets(const std::vector<std::unique_ptr<Configuration>> &configurations,
                        const std::map<ModuleTypes::ModuleType, std::vector<const Module *>> &moduleMap)
{
    // Search for Configuration-based targets
    if (!configurations.empty())
        for (auto *keyword : keywords_.targetKeywords())
        {
            if (keyword->typeIndex() == typeid(ConfigurationKeyword *))
                keywords_.set(keyword->name(), configurations.front().get());
            else if (keyword->typeIndex() == typeid(ConfigurationVectorKeyword *))
            {
                std::vector<Configuration *> rawCfgs(configurations.size());
                std::transform(configurations.begin(), configurations.end(), rawCfgs.begin(),
                               [](const auto &unique) { return unique.get(); });
                keywords_.set(keyword->name(), rawCfgs);
            }
        }
}

// Run set-up stage
bool Module::setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals) { return true; }

// Run main processing stage
Module::ExecutionResult Module::executeProcessing(ModuleContext &moduleContext)
{
    // Check target configurations
    auto targetCheckResult = checkConfigurationTargets(moduleContext.dissolve().processingModuleData());
    if (targetCheckResult != ExecutionResult::Success)
        return targetCheckResult;

    // Begin timer
    Timer timer;
    timer.start();

    // Run main processing routine
    auto result = process(moduleContext);

    // Accumulate timing information
    timer.stop();

    if (result == ExecutionResult::Success)
        processTimes_ += timer.secondsElapsed();

    // Update last processed configuration data
    auto &&[currentTargets, expectedTargetCount] = getCurrentTargetConfigurations();
    for (auto *currentTarget : currentTargets)
        lastProcessedConfigurations_[currentTarget] = currentTarget->contentsVersion();

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

// Express as a serialisable value
SerialisedValue Module::serialise() const
{
    SerialisedValue result{{"type", ModuleTypes::moduleType(type_)}, {"frequency", frequency_}};
    if (!enabled_)
        result["disabled"] = true;
    return keywords_.serialiseOnto(result);
}

// Read values from a serialisable value
void Module::deserialise(const SerialisedValue &node, const CoreData &data)
{
    enabled_ = !toml::find_or<bool>(node, "disabled", false);
    frequency_ = toml::find<int>(node, "frequency");

    keywords_.deserialiseFrom(node, data);
}
