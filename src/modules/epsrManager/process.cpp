// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "math/data1D.h"
#include "module/context.h"
#include "modules/epsr/epsr.h"
#include "modules/epsrManager/epsrManager.h"

// Run set-up stage
bool EPSRManagerModule::setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    // Notify targeted EPSR modules that they should not apply potentials
    for (auto *module : target_)
    {
        auto *epsrModule = dynamic_cast<EPSRModule *>(module);
        Messenger::print("EPSRManager: Notifying '{}' that it should not apply generated potentials.\n", epsrModule->name());
        epsrModule->setApplyPotentials(false);
    }

    return true;
}

// Run main processing
Module::ExecutionResult EPSRManagerModule::process(ModuleContext &moduleContext)
{
    auto &moduleData = moduleContext.dissolve().processingModuleData();

    std::map<std::string, EPData> potentials;

    // Loop over target data
    for (auto *module : target_)
    {
        auto *epsrModule = dynamic_cast<EPSRModule *>(module);
        auto eps = epsrModule->empiricalPotentials();

        for (auto &&[at1, at2, ep] : eps)
        {
            auto key = EPSRManagerModule::pairKey(at1, at2);
            auto keyIt = potentials.find(key);
            if (keyIt == potentials.end())
                potentials[key] = {ep, 1, at1, at2};
            else
            {
                Interpolator::addInterpolated(ep, potentials[key].ep, 1.0);
                ++potentials[key].count;
            }
        }
    }

    // Form averages
    for (auto &&[key, epData] : potentials)
        epData.ep /= epData.count;

    // Vector of averaged potentials over multiple iterations
    std::vector<std::map<std::string, EPData>> averagedPotentialsStore;
    std::map<std::string, EPData> averagedPotentials = potentials;
    // Check if ran the right amount of iterations before averaging
    if (averagedPotentialsStore.size() < averagingLength_)
    {
        // If not then add data to vector
        averagedPotentialsStore.emplace_back(potentials);
    }
    else
    {
        // If yes then average the potentials and replace the map with the new averaged
        for (auto n : averagedPotentialsStore)
        {
            for (auto &&[key, epData] : n)
            {
                averagedPotentials[key].ep += epData.ep;
            }
        }
    }
    potentials = averagedPotentials;
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
        for (auto &&[key, epData] : potentials)
        {
            // Is this potential a match
            if ((DissolveSys::sameWildString(typeA, epData.at1->name()) &&
                 DissolveSys::sameWildString(typeB, epData.at2->name())) ||
                (DissolveSys::sameWildString(typeB, epData.at1->name()) &&
                 DissolveSys::sameWildString(typeA, epData.at2->name())))
            {
                Messenger::print(" ... matched and scaled potential {}-{}\n", epData.at1->name(), epData.at2->name());
                epData.ep *= scaleFactor;
                ++count;
            }
        }
        Messenger::print(" ... matched {} potential(s) in total.\n", count);
    }

    // Adjust global potentials
    for (auto &&[key, epData] : potentials)
    {
        // Grab pointer to the relevant pair potential (if it exists)
        auto *pp = moduleContext.dissolve().pairPotential(epData.at1, epData.at2);
        if (pp)
            pp->setAdditionalPotential(epData.ep);
    }

    return ExecutionResult::Success;
}
