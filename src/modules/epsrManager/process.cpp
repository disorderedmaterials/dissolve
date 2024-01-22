// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "math/data1D.h"
#include "module/context.h"
#include "modules/epsr/epsr.h"
#include "modules/epsrManager/epsrManager.h"

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

        for (auto &&[atomTypePair, ep] : eps)
        {
            auto key = EPSRManagerModule::pairKey(atomTypePair);
            auto keyIt = potentials.find(key);
            if (keyIt == potentials.end())
                potentials[key] = {ep, 1, atomTypePair.first, atomTypePair.second};
            else
            {
                Interpolator::addInterpolated(ep, potentials[key].ep, 1.0);
                ++potentials[key].count;
            }
        }
    }
    for (auto &&[key, epData] : potentials)
    {
        epData.ep /= epData.count;
        // Grab pointer to the relevant pair potential (if it exists)
        auto *pp = moduleContext.dissolve().pairPotential(epData.at1, epData.at2);
        if (pp)
            pp->setUAdditional(epData.ep);
    }

    return ExecutionResult::Success;
}
