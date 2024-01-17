// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/epsrManager/epsrManager.h"

// Run main processing
Module::ExecutionResult EPSRManagerModule::process(ModuleContext &moduleContext)
{
    // Loop over target data
    for (auto *module : target_)
    {
        auto &moduleData = moduleContext.dissolve().processingModuleData();
        std::optional<double> rho;
        // Set up the additional potentials - reconstruct them from the current coefficients
        auto rmaxpt = rMaxPT_ ? rMaxPT_.value() : moduleContext.dissolve().pairPotentialRange();
        auto rminpt = rMinPT_ ? rMinPT_.value() : rmaxpt - 2.0;
        generateEmpiricalPotentials(module->name(), moduleData, moduleContext.dissolve(), rho.value_or(0.1), nCoeffP_, rminpt,
                                    rmaxpt, gSigma1_, gSigma2_);
    }

    return ExecutionResult::Success;
}
