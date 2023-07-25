// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "modules/analyse/analyse.h"

// Run main processing
Module::ExecutionResult AnalyseModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for Configuration target
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Execute the analysis
    ProcedureContext context(procPool, targetConfiguration_);
    context.setPrefix(name());
    if (!analyser_.execute(context))
    {
        Messenger::error("Analysis ExecutionResult::Failed.\n");
        return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
