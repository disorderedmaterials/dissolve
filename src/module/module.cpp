// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "module/module.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "keywords/configuration.h"

// Static Singletons
std::vector<Module *> Module::instances_;

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
                                                                 {ModuleTypes::MolShake, "MolShake"},
                                                                 {ModuleTypes::NeutronSQ, "NeutronSQ"},
                                                                 {ModuleTypes::OrientedSDF, "OrientedSDF"},
                                                                 {ModuleTypes::QSpecies, "QSpecies"},
                                                                 {ModuleTypes::SDF, "SDF"},
                                                                 {ModuleTypes::SiteRDF, "SiteRDF"},
                                                                 {ModuleTypes::SQ, "SQ"},
                                                                 {ModuleTypes::Test, "Test"},
                                                                 {ModuleTypes::XRaySQ, "XRaySQ"}});

// Return module type string for specified type enumeration
std::string moduleType(ModuleTypes::ModuleType type) { return moduleTypes_.keyword(type); }
// Return the lowerCamelCase name of the module type provided
std::string lccModuleType(ModuleTypes::ModuleType type)
{
    auto lccModuleType = moduleTypes_.keyword(type);
    if (!std::all_of(lccModuleType.begin(), lccModuleType.end(), [](const char c) { return std::isupper(c); }))
        lccModuleType.front() = tolower(lccModuleType.front());
    return lccModuleType;
}
// Return module type enumeration for specified module type string
std::optional<ModuleTypes::ModuleType> moduleType(std::string_view keyword)
{
    if (moduleTypes_.isValid(keyword))
        return moduleTypes_.enumeration(keyword);
    else
        return {};
};
} // namespace ModuleTypes

Module::Module(const ModuleTypes::ModuleType type) : type_(type), frequency_(1), enabled_(true) { instances_.push_back(this); }

Module::~Module() { instances_.erase(std::remove(instances_.begin(), instances_.end(), this)); }

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
    // Begin timer
    Timer timer;
    timer.start();

    // Run main processing routine
    auto result = process(moduleContext);

    // Accumulate timing information
    timer.stop();

    if (result == ExecutionResult::Success)
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
std::vector<Module *> Module::allOfType(ModuleTypes::ModuleType type)
{
    return allOfType(std::vector<ModuleTypes::ModuleType>{type});
}

std::vector<Module *> Module::allOfType(std::vector<ModuleTypes::ModuleType> types)
{
    std::vector<Module *> modules;
    std::copy_if(instances_.begin(), instances_.end(), std::back_inserter(modules),
                 [&types](const auto *m) { return std::find(types.begin(), types.end(), m->type()) != types.end(); });
    return modules;
}

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
