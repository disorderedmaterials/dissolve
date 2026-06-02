// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "math/data1D.h"
#include "modules/epsrManager/epsrManager.h"

// Run set-up stage
bool EPSRManagerModule::setUp(Dissolve &dissolve, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    // Notify targeted EPSR modules that they should not apply potentials
    // for (auto *module : target_)
    // {
    //     auto *epsrModule = dynamic_cast<EPSRModule *>(module);
    //     Messenger::print("EPSRManager: Notifying '{}' that it should not apply generated potentials.\n", epsrModule->name());
    //     epsrModule->setApplyPotentials(false);
    // }
    // TODO DISSOLVE2
    return true;
}

// Run main processing
Module::ExecutionResult EPSRManagerModule::process(Dissolve &dissolve)
{
    if (averagingLength_)
        Messenger::print("Potentials will be averaged over {} sets (scheme = {}).\n", averagingLength_.value(),
                         Averaging::averagingSchemes().keyword(averagingScheme_));
    else
        Messenger::print("Potentials: No averaging of potentials will be performed.\n");

    auto &moduleData = dissolve.processingModuleData();

    // Loop over target data and form summed / averaged potentials
    PotentialSet newPotentials;
    DoubleKeyedMap<double> counts;
    for (auto *module : target_)
    {
        // auto *epsrModule = dynamic_cast<EPSRModule *>(module);
        // auto eps = epsrModule->empiricalPotentials();
        //
        // for (auto &&[at1, at2, potential] : eps)
        // {
        //     newPotentials.potentials()[{at1->name(), at2->name()}] += potential;
        //     counts[{at1->name(), at2->name()}] += 1.0;
        // }
        // TODO DISSOLVE2
    }

    // Normalise potentials to counts
    for (auto &&[key, potential] : newPotentials.potentials())
        potential /= counts[key];

    // Does a PotentialSet already exist for this Configuration?
    auto originalPotentialsObject = moduleData.realiseIf<PotentialSet>("PotentialSet", name_, GenericItem::InRestartFileFlag);

    // Set restart equal to changes
    originalPotentialsObject.first = newPotentials;

    // Reference to the current potentials
    auto &currentPotentials = moduleData.realise<PotentialSet>("PotentialSet", name_, GenericItem::InRestartFileFlag);

    // Average the Potentials
    if (averagingLength_)
        Averaging::average<PotentialSet>(dissolve.processingModuleData(), "PotentialSet", name(), averagingLength_.value(),
                                         averagingScheme_);

    // Apply potential scalings
    auto scalings = DissolveSys::splitString(potentialScalings_, ",");
    for (const auto &scaling : scalings)
    {
        // Parse the string into the atom type pair and scaling factor
        auto typePairs = DissolveSys::beforeChar(scaling, '=');
        auto typeA = DissolveSys::strip(DissolveSys::beforeChar(typePairs, '-'));
        auto typeB = DissolveSys::strip(DissolveSys::afterChar(typePairs, '-'));
        auto scaleFactorString = DissolveSys::afterChar(scaling, '=');
        if (typeA.empty() || typeB.empty() || scaleFactorString.empty())
        {
            Messenger::error("Failed to parse scaling string '{}'\n", scaling);
            return ExecutionResult::Failed;
        }
        auto scaleFactor = std::stod(std::string(scaleFactorString));

        Messenger::print("Apply scaling factor of {} to potential(s) {}-{}...\n", scaleFactor, typeA, typeB);
        auto count = 0;
        for (auto &&[key, potential] : currentPotentials.potentials())
        {
            auto parts = currentPotentials.potentials().keyPair(key);

            // Is this potential a match
            if ((DissolveSys::sameWildString(typeA, parts.first) && DissolveSys::sameWildString(typeB, parts.second)) ||
                (DissolveSys::sameWildString(typeB, parts.first) && DissolveSys::sameWildString(typeA, parts.second)))
            {
                Messenger::print(" ... matched and scaled potential {}-{}\n", parts.first, parts.second);
                potential *= scaleFactor;
                ++count;
            }
        }
        Messenger::print(" ... matched {} potential(s) in total.\n", count);
    }

    // Adjust global potentials
    // TODO DISSOLVE2
    // for (auto &&[key, epData] : currentPotentials.potentials())
    // {
    //     // Grab pointer to the relevant pair potential (if it exists)
    //     auto *pp = dissolve.pairPotential(epData.at1, epData.at2);
    //     if (pp)
    //         pp->setAdditionalPotential(epData.potential);
    // }

    return ExecutionResult::Success;
}
