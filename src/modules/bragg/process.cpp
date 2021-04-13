// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/neutronweights.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "modules/bragg/bragg.h"
#include "templates/algorithms.h"

// Run main processing
bool BraggModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Calculate Bragg contributions.
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     * Partial calculation routines called by this routine are parallel.
     */

    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    const auto averaging = keywords_.asInt("Averaging");
    if (!Averaging::averagingSchemes().isValid(keywords_.asString("AveragingScheme")))
        return Averaging::averagingSchemes().errorAndPrintValid(keywords_.asString("AveragingScheme"));
    Averaging::AveragingScheme averagingScheme =
        Averaging::averagingSchemes().enumeration(keywords_.asString("AveragingScheme"));
    const auto qDelta = keywords_.asDouble("QDelta");
    const auto qMax = keywords_.asDouble("QMax");
    const auto qMin = keywords_.asDouble("QMin");
    const auto multiplicity = keywords_.asVec3Int("Multiplicity");
    // TODO Could look for this value in the Configuration's module data (could be set/stored if a known crystal repeat was
    // used in the generation of the Configuration).
    const bool saveReflections = keywords_.asBool("SaveReflections");

    // Print argument/parameter summary
    Messenger::print("Bragg: Calculating Bragg S(Q) over {} < Q < {} Angstroms**-1 using bin size of {} Angstroms**-1.\n", qMin,
                     qMax, qDelta);
    Messenger::print("Bragg: Multiplicity is ({} {} {}).\n", multiplicity.x, multiplicity.y, multiplicity.z);
    if (averaging <= 1)
        Messenger::print("Bragg: No averaging of reflections will be performed.\n");
    else
        Messenger::print("Bragg: Reflections will be averaged over {} sets (scheme = {}).\n", averaging,
                         Averaging::averagingSchemes().keyword(averagingScheme));
    Messenger::print("\n");

    /*
     * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or
     * multiple independent Configurations, we must loop over the specified targetConfigurations_ and calculate the partials
     * for each.
     */
    for (Configuration *cfg : targetConfigurations_)
    {
        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Calculate Bragg vectors and intensities for the current Configuration
        bool alreadyUpToDate;
        if (!calculateBraggTerms(dissolve.processingModuleData(), procPool, cfg, qMin, qDelta, qMax, multiplicity,
                                 alreadyUpToDate))
            return false;

        // If we are already up-to-date, then theres nothing more to do for this Configuration
        if (alreadyUpToDate)
        {
            Messenger::print("Bragg data is up-to-date for Configuration '{}'.\n", cfg->name());
            continue;
        }

        // Perform averaging of reflections data if requested
        if (averaging > 1)
        {
            Averaging::vectorAverage<std::vector<BraggReflection>>(dissolve.processingModuleData(),
                                                                   fmt::format("{}//BraggReflections", cfg->niceName()),
                                                                   uniqueName(), averaging, averagingScheme);
        }

        // Form partial and total reflection functions
        formReflectionFunctions(dissolve.processingModuleData(), procPool, cfg, qMin, qDelta, qMax);

        // Save reflection data?
        if (saveReflections)
        {
            // Retrieve BraggReflection data from the Configuration's module data
            const auto &braggReflections = dissolve.processingModuleData().value<const std::vector<BraggReflection>>(
                fmt::format("{}//BraggReflections", cfg->niceName()), uniqueName());

            // Open a file and save the basic reflection data
            LineParser braggParser(&procPool);
            if (!braggParser.openOutput(fmt::format("{}-Bragg.txt", cfg->niceName())))
                return false;
            braggParser.writeLineF("#   ID      Q       nKVecs    Intensity(0,0)\n");
            auto count = 0;
            for (const auto &reflxn : braggReflections)
            {
                if (!braggParser.writeLineF("{:6d}  {:10.6f}  {:8d}  {:10.6e}\n", ++count, reflxn.q(), reflxn.nKVectors(),
                                            reflxn.intensity(0, 0)))
                    return false;
            }
            braggParser.closeFiles();

            // Save intensity data
            auto &types = cfg->usedAtomTypesList();
            auto success = for_each_pair_early(
                types.begin(), types.end(),
                [&](int i, const AtomTypeData &atd1, int j, const AtomTypeData &atd2) -> EarlyReturn<bool> {
                    LineParser intensityParser(&procPool);
                    if (!intensityParser.openOutput(
                            fmt::format("{}-Bragg-{}-{}.txt", cfg->niceName(), atd1.atomTypeName(), atd2.atomTypeName())))
                        return false;
                    intensityParser.writeLineF("#     Q      Intensity({},{})\n", atd1.atomTypeName(), atd2.atomTypeName());
                    for (const auto &reflxn : braggReflections)
                    {
                        if (!intensityParser.writeLineF("{:10.6f}  {:10.6e}\n", reflxn.q(), reflxn.intensity(i, j)))
                            return false;
                        intensityParser.writeLineF("#     Q      Intensity({},{})\n", atd1.atomTypeName(), atd2.atomTypeName());
                    }
                    intensityParser.closeFiles();

                    return EarlyReturn<bool>::Continue;
                });
            if (!success.value_or(true))
                return false;
        }
    }

    return true;
}
