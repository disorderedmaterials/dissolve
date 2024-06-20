// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/analyse/analyse.h"

// Run main processing
Module::ExecutionResult AnalyseModule::process(ModuleContext &moduleContext)
{
    // Execute the analysis
    if (!analyser_.execute({moduleContext.dissolve(), targetConfiguration_, name()}))
    {
        Messenger::error("Analysis ExecutionResult::Failed.\n");
        return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
