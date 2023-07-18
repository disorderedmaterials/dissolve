// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "modules/analyse/analyse.h"

// Run main processing
enum Module::executionResult AnalyseModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for Configuration target
    if (!targetConfiguration_)
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return failed;

    // Execute the analysis
    ProcedureContext context(procPool, targetConfiguration_);
    context.setDataListAndPrefix(dissolve.processingModuleData(), name());
    if (!analyser_.execute(context))
        Messenger::error("Analysis failed.\n");
        return failed;

    return success;
}
