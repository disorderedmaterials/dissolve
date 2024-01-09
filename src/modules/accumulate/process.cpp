// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "classes/partialSetAccumulator.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/accumulate/accumulate.h"

// Valid Target Modules / Data
const std::map<ModuleTypes::ModuleType, std::vector<std::string>> validTargets = {
    {ModuleTypes::GR, {"UnweightedGR", "", "OriginalGR"}},
    {ModuleTypes::SQ, {"", "UnweightedSQ", ""}},
    {ModuleTypes::NeutronSQ, {"WeightedGR", "WeightedSQ", ""}},
    {ModuleTypes::XRaySQ, {"WeightedGR", "WeightedSQ", ""}}};

// Return EnumOptions for TargetPartialSet
EnumOptions<AccumulateModule::TargetPartialSet> AccumulateModule::targetPartialSet()
{
    return EnumOptions<AccumulateModule::TargetPartialSet>(
        "TargetPartialSet",
        {{TargetPartialSet::GR, "GR"}, {TargetPartialSet::SQ, "SQ"}, {TargetPartialSet::OriginalGR, "OriginalRDF"}});
}

// Run main processing
Module::ExecutionResult AccumulateModule::process(ModuleContext &moduleContext)
{
    // Get the modules and decide on the PartialSet data name we're looking for
    if (targetModules_.empty())
    {
        Messenger::error("No target modules set.");
        return ExecutionResult::Failed;
    }

    Messenger::print("Accumulate: Target data to accumulate is '{}'.\n", targetPartialSet().keyword(targetPartialSet_));
    Messenger::print("Accumulate: Save data is {}.\n", DissolveSys::onOff(save_));
    Messenger::print("\n");

    for (const auto *targetModule : targetModules_)
    {
        // Print summary of parameters
        Messenger::print("Accumulating data from module '{}'...\n", targetModule->name());

        // Is the target module / data type a valid combination?
        auto targetDataIt = std::find_if(validTargets.begin(), validTargets.end(),
                                         [targetModule](const auto &target) { return target.first == targetModule->type(); });
        if (targetDataIt == validTargets.end())
        {
            Messenger::error("Module of type '{}' is not a valid target.\n", ModuleTypes::moduleType(targetModule->type()));
            return ExecutionResult::Failed;
        }

        auto dataName = targetDataIt->second[targetPartialSet_];
        if (dataName.empty())
        {
            Messenger::error("This data type ('{}') is not valid for a module of type '{}'.\n",
                             targetPartialSet().keyword(targetPartialSet_), ModuleTypes::moduleType(targetModule->type()));
            return ExecutionResult::Failed;
        }

        // Find the target data
        auto targetSet = moduleContext.dissolve().processingModuleData().valueIf<PartialSet>(dataName, targetModule->name());
        if (!targetSet)
        {
            Messenger::print("Target PartialSet data '{}' in module '{}' does not yet exist.\n",
                             targetPartialSet().keyword(targetPartialSet_), targetModule->name());
            return ExecutionResult::NotExecuted;
        }

        // Realise the accumulated partial set
        auto &accumulated = moduleContext.dissolve().processingModuleData().realise<PartialSetAccumulator>(
            targetModule->name(), name(), GenericItem::ItemFlag::InRestartFileFlag);

        accumulated += *targetSet;

        Messenger::print("Accumulated data count = {}.\n", accumulated.nAccumulated());

        // Save data if requested
        std::vector<std::string> suffixes = {"gr", "sq", "gr"};
        std::vector<std::string> units = {"r, Angstroms", "Q, Angstroms**-1", "r, Angstroms"};
        if (save_ && !(MPIRunMaster(procPool, accumulated.save(fmt::format("{}-{}", name(), targetModule->name()), dataName,
                                                               suffixes[targetPartialSet_], units[targetPartialSet_]))))
            return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
