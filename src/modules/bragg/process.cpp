// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/kvector.h"
#include "classes/neutronweights.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "modules/bragg/bragg.h"
#include "templates/algorithms.h"

// Run main processing
bool BraggModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    /*
     * Calculate Bragg contributions.
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     * Partial calculation routines called by this routine are parallel.
     */

    // Check for Configuration target
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", name());

    // Print argument/parameter summary
    Messenger::print("Bragg: Calculating Bragg S(Q) over {} < Q < {} Angstroms**-1 using bin size of {} Angstroms**-1.\n",
                     qMin_, qMax_, qDelta_);
    Messenger::print("Bragg: Multiplicity is ({} {} {}).\n", multiplicity_.x, multiplicity_.y, multiplicity_.z);
    if (averagingLength_ <= 1)
        Messenger::print("Bragg: No averaging of reflections will be performed.\n");
    else
        Messenger::print("Bragg: Reflections will be averaged over {} sets (scheme = {}).\n", averagingLength_,
                         Averaging::averagingSchemes().keyword(averagingScheme_));
    Messenger::print("Multiplicity of unit cell in source configuration is [{} {} {}].\n", multiplicity_.x, multiplicity_.y,
                     multiplicity_.z);
    Messenger::print("\n");

    // Realise an AtomTypeList containing the sum of atom types over all target configurations (currently only one)
    auto &combinedAtomTypes =
        dissolve.processingModuleData().realise<AtomTypeMix>("SummedAtomTypes", name_, GenericItem::InRestartFileFlag);
    combinedAtomTypes.clear();
    combinedAtomTypes.add(targetConfiguration_->atomTypes());

    // Store unit cell information
    auto &unitCellVolume = dissolve.processingModuleData().realise<double>("V0", name_, GenericItem::InRestartFileFlag);
    unitCellVolume = targetConfiguration_->box()->volume() / (multiplicity_.x * multiplicity_.y * multiplicity_.z);

    // Finalise combined AtomTypes matrix
    combinedAtomTypes.finalise();

    // Calculate Bragg vectors and intensities for the current Configuration
    bool alreadyUpToDate;
    if (!calculateBraggTerms(dissolve.processingModuleData(), procPool, targetConfiguration_, qMin_, qDelta_, qMax_,
                             multiplicity_, alreadyUpToDate))
        return false;

    // If we are already up-to-date, then there's nothing more to do for this Configuration
    if (alreadyUpToDate)
    {
        Messenger::print("Bragg data is up-to-date for Configuration '{}'.\n", targetConfiguration_->name());
        return true;
    }

    // Perform averaging of reflections data if requested
    if (averagingLength_ > 1)
        Averaging::vectorAverage<std::vector<BraggReflection>>(dissolve.processingModuleData(), "Reflections", name(),
                                                               averagingLength_, averagingScheme_);

    // Form partial and total reflection functions
    formReflectionFunctions(dissolve.processingModuleData(), procPool, targetConfiguration_, qMin_, qDelta_, qMax_);

    // Test reflection data
    if (!testReflectionsFile_.empty())
    {
        Messenger::print("Testing calculated intensity data against reference...\n");

        // Attempt to load the specified file
        LineParser reflectionParser(&procPool);
        if (!reflectionParser.openInput(testReflectionsFile_))
            return false;

        // Retrieve BraggReflection data from the Configuration's module data
        const auto &braggReflections =
            dissolve.processingModuleData().value<const std::vector<BraggReflection>>("Reflections", name());

        auto nErrors = 0;
        while (!reflectionParser.eofOrBlank())
        {
            if (reflectionParser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;

            // Line format is : ArrayIndex  Q     h k l     mult    Intensity(0,0)  Intensity(0,1) ...
            auto id = reflectionParser.argi(0);
            if (fabs(reflectionParser.argd(1) - braggReflections[id].q()) > 1.0e-3)
            {
                Messenger::print("Q value of reflection {} does not match reference ({} vs {}).\n", id,
                                 braggReflections[id].q(), reflectionParser.argd(1));
                ++nErrors;
            }
            auto hkl = reflectionParser.arg3i(2);
            if ((hkl - braggReflections[id].hkl()).absMax() != 0)
            {
                Messenger::print("Miller indices of reflection {} do not match reference ({} {} {} vs {} {} {}).\n", id,
                                 braggReflections[id].hkl().x, braggReflections[id].hkl().y, braggReflections[id].hkl().z,
                                 hkl.x, hkl.y, hkl.z);
                ++nErrors;
            }
            if (reflectionParser.argi(5) != braggReflections[id].nKVectors())
            {
                Messenger::print("Multiplicity of reflection {} does not match reference ({} vs {}).\n", id,
                                 braggReflections[id].nKVectors(), reflectionParser.argi(5));
                ++nErrors;
            }
            for (auto n = 6; n < reflectionParser.nArgs(); ++n)
            {
                if (fabs(reflectionParser.argd(n) - braggReflections[id].intensities()[n - 6]) > 1.0e-3)
                {
                    Messenger::print("Intensity value {} of reflection {} does not match reference ({} vs {}).\n", n - 6, id,
                                     braggReflections[id].intensities()[n - 6], reflectionParser.argd(n));
                    ++nErrors;
                }
            }
        }

        if (nErrors == 0)
            Messenger::print("All data match.\n");
        else
            return Messenger::error("Calculated and reference data are inconsistent.\n");
    }

    // Save reflection data?
    if (saveReflections_)
    {
        // Retrieve BraggReflection data from the Configuration's module data
        const auto &braggReflections =
            dissolve.processingModuleData().value<const std::vector<BraggReflection>>("Reflections", name());

        // Open a file and save the basic reflection data
        LineParser braggParser(&procPool);
        if (!braggParser.openOutput(fmt::format("{}-Reflections.txt", name_)))
            return false;
        braggParser.writeLineF("#   ID      Q     h k l     mult    Intensity(0,0)\n");
        auto count = 0;
        for (const auto &reflxn : braggReflections)
        {
            if (!braggParser.writeLineF("{:6d}  {:10.6f} {} {} {} {:8d}  {:10.6e}\n", ++count, reflxn.q(), reflxn.hkl().x,
                                        reflxn.hkl().y, reflxn.hkl().z, reflxn.nKVectors(), reflxn.intensity(0, 0)))
                return false;
        }
        braggParser.closeFiles();

        // Save intensity data
        auto &types = targetConfiguration_->atomTypes();
        auto success = for_each_pair_early(
            types.begin(), types.end(),
            [&](int i, const AtomTypeData &atd1, int j, const AtomTypeData &atd2) -> EarlyReturn<bool>
            {
                LineParser intensityParser(&procPool);
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
            return false;
    }

    return true;
}
