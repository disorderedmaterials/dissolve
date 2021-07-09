// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/analyse/analyse.h"

// Run main processing
bool AnalyseModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (targetConfigurationsKeyword_.data().empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Loop over target Configurations
    for (auto *cfg : targetConfigurationsKeyword_.data())
    {
        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Execute the analysis
        if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData()))
            return Messenger::error("Analysis failed.\n");
    }

    return true;
}
