// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/kVector.h"
#include "classes/neutronWeights.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "module/context.h"
#include "modules/bragg/bragg.h"
#include "templates/algorithms.h"

// Run main processing
Module::ExecutionResult BraggModule::process(ModuleContext &moduleContext)
{
    /*
     * Calculate Bragg contributions.
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     * Partial calculation routines called by this routine are parallel.
     */

    // Check for Configuration target
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Print argument/parameter summary
    Messenger::print("Bragg: Calculating Bragg S(Q) over {} < Q < {} Angstroms**-1 using bin size of {} Angstroms**-1.\n",
                     qMin_, qMax_, qDelta_);
    Messenger::print("Bragg: Multiplicity is ({} {} {}).\n", multiplicity_.x, multiplicity_.y, multiplicity_.z);
    if (averagingLength_)
        Messenger::print("Bragg: Reflections will be averaged over {} sets (scheme = {}).\n", averagingLength_.value(),
                         Averaging::averagingSchemes().keyword(averagingScheme_));
    else
        Messenger::print("Bragg: No averaging of reflections will be performed.\n");
    Messenger::print("Multiplicity of unit cell in source configuration is [{} {} {}].\n", multiplicity_.x, multiplicity_.y,
                     multiplicity_.z);
    Messenger::print("\n");

    // Realise an AtomTypeList containing the sum of atom types over all target configurations (currently only one)
    auto &combinedAtomTypes = moduleContext.dissolve().processingModuleData().realise<AtomTypeMix>(
        "SummedAtomTypes", name_, GenericItem::InRestartFileFlag);
    combinedAtomTypes.clear();
    combinedAtomTypes.add(targetConfiguration_->atomTypes());

    // Store unit cell information
    auto &unitCellVolume =
        moduleContext.dissolve().processingModuleData().realise<double>("V0", name_, GenericItem::InRestartFileFlag);
    unitCellVolume = targetConfiguration_->box()->volume() / (multiplicity_.x * multiplicity_.y * multiplicity_.z);

    // Finalise combined AtomTypes matrix
    combinedAtomTypes.finalise();

    // Calculate Bragg vectors and intensities for the current Configuration
    bool alreadyUpToDate;
    if (!calculateBraggTerms(moduleContext.dissolve().processingModuleData(), moduleContext.processPool(), targetConfiguration_,
                             qMin_, qDelta_, qMax_, multiplicity_, alreadyUpToDate))
        return ExecutionResult::Failed;

    // If we are already up-to-date, then there's nothing more to do for this Configuration
    if (alreadyUpToDate)
    {
        Messenger::print("Bragg data is up-to-date for Configuration '{}'.\n", targetConfiguration_->name());
        return ExecutionResult::NotExecuted;
    }

    // Perform averaging of reflections data if requested
    if (averagingLength_)
        Averaging::vectorAverage<std::vector<BraggReflection>>(moduleContext.dissolve().processingModuleData(), "Reflections",
                                                               name(), averagingLength_.value(), averagingScheme_);

    // Form partial and total reflection functions
    formReflectionFunctions(moduleContext.dissolve().processingModuleData(), moduleContext.processPool(), targetConfiguration_,
                            qMin_, qDelta_, qMax_);

    // Save reflection data?
    if (saveReflections_)
    {
        // Retrieve BraggReflection data from the Configuration's module data
        const auto &braggReflections =
            moduleContext.dissolve().processingModuleData().value<const std::vector<BraggReflection>>("Reflections", name());

        // Open a file and save the basic reflection data
        LineParser braggParser(&moduleContext.processPool());
        if (!braggParser.openOutput(fmt::format("{}-Reflections.txt", name_)))
            return ExecutionResult::Failed;
        braggParser.writeLineF("#   ID      Q     h k l     mult    Intensity(0,0)\n");
        auto count = 0;
        for (const auto &reflxn : braggReflections)
        {
            if (!braggParser.writeLineF("{:6d}  {:10.6f} {} {} {} {:8d}  {:10.6e}\n", ++count, reflxn.q(), reflxn.hkl().x,
                                        reflxn.hkl().y, reflxn.hkl().z, reflxn.nKVectors(), reflxn.intensity(0, 0)))
                return ExecutionResult::Failed;
        }
        braggParser.closeFiles();

        // Save intensity data
        auto &types = targetConfiguration_->atomTypes();
        auto success = for_each_pair_early(
            types.begin(), types.end(),
            [&](int i, const AtomTypeData &atd1, int j, const AtomTypeData &atd2) -> EarlyReturn<bool>
            {
                LineParser intensityParser(&moduleContext.processPool());
                if (!intensityParser.openOutput(fmt::format("{}-{}-{}.txt", name_, atd1.atomTypeName(), atd2.atomTypeName())))
                    return false;
                intensityParser.writeLineF("#     Q      Intensity({},{})\n", atd1.atomTypeName(), atd2.atomTypeName());
                for (const auto &reflxn : braggReflections)
                    if (!intensityParser.writeLineF("{:10.6f}  {:10.6e}\n", reflxn.q(), reflxn.intensity(i, j)))
                        return false;
                intensityParser.closeFiles();

                return EarlyReturn<bool>::Continue;
            });
        if (!success.value_or(true))
            return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
