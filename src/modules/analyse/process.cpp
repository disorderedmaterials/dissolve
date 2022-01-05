// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/analyse/analyse.h"

// Run main processing
bool AnalyseModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for Configuration target
    if (!targetConfiguration_)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(targetConfiguration_->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, targetConfiguration_, uniqueName(), dissolve.processingModuleData()))
        return Messenger::error("Analysis failed.\n");

    return true;
}
