// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/partialsetaccumulator.h"
#include "main/dissolve.h"
#include "modules/accumulate/accumulate.h"

// Valid Target Modules / Data
const std::map<std::string, std::vector<std::string>> validTargets = {{"RDF", {"UnweightedGR", "", "OriginalGR"}},
                                                                      {"SQ", {"", "UnweightedSQ", ""}},
                                                                      {"NeutronSQ", {"WeightedGR", "WeightedSQ", ""}},
                                                                      {"XRaySQ", {"WeightedGR", "WeightedSQ", ""}}};

// Return EnumOptions for TargetPartialSet
EnumOptions<AccumulateModule::TargetPartialSet> AccumulateModule::targetPartialSet()
{
    return EnumOptions<AccumulateModule::TargetPartialSet>(
        "TargetPartialSet",
        {{TargetPartialSet::GR, "RDF"}, {TargetPartialSet::SQ, "SQ"}, {TargetPartialSet::OriginalGR, "OriginalRDF"}});
}

// Run main processing
bool AccumulateModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Get keyword data
    const auto &targets = keywords_.retrieve<std::vector<Module *>>("Target");
    if (targets.empty())
        return Messenger::error("No target module set.");
    const auto targetData = keywords_.enumeration<AccumulateModule::TargetPartialSet>("Data");
    const auto saveData = keywords_.asBool("Save");

    // Get the module and decide on the PartialSet data name we're looking for
    const auto *targetModule = targets.front();
    if (!targetModule)
        return Messenger::error("No target module set.\n");

    // Print summary of parameters
    Messenger::print("Accumulate: Source module is '{}'.\n", targetModule->uniqueName());
    Messenger::print("Accumulate: Target data to accumulate is '{}'.\n", targetPartialSet().keyword(targetData));
    Messenger::print("Accumulate: Save data is {}.\n", DissolveSys::onOff(saveData));
    Messenger::print("\n");

    // Is the target module / data type a valid combination?
    auto targetsIt = std::find_if(validTargets.begin(), validTargets.end(),
                                  [targetModule](const auto &target) { return target.first == targetModule->type(); });
    if (targetsIt == validTargets.end())
        return Messenger::error("Module of type '{}' is not a valid target.\n", targetModule->type());
    auto dataName = targetsIt->second[targetData];
    if (dataName.empty())
        return Messenger::error("This data type ('{}') is not valid for a module of type '{}'.\n",
                                targetPartialSet().keyword(targetData), targetModule->type());

    // Find the target data
    auto targetSet = dissolve.processingModuleData().valueIf<PartialSet>(dataName, targetModule->uniqueName());
    if (!targetSet)
    {
        Messenger::print("Target PartialSet data '{}' in module '{}' does not yet exist.\n",
                         targetPartialSet().keyword(targetData), targetModule->uniqueName());
        return true;
    }

    // Realise the accumulated partial set
    auto &accumulated = dissolve.processingModuleData().realise<PartialSetAccumulator>(
        "Accumulation", uniqueName(), GenericItem::ItemFlag::InRestartFileFlag);

    accumulated += *targetSet;

    Messenger::print("Accumulated data count = {}.\n", accumulated.nAccumulated());

    // Save data if requested
    std::vector<std::string> suffixes = {"gr", "sq", "gr"};
    std::vector<std::string> units = {"r, Angstroms", "Q, Angstroms**-1", "r, Angstroms"};
    if (saveData && (!MPIRunMaster(procPool, accumulated.save(uniqueName_, dataName, suffixes[targetData], units[targetData]))))
        return false;

    return true;
}
