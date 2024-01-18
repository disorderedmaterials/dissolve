// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/epsr/epsr.h"
#include "modules/epsrManager/epsrManager.h"

// Run main processing
Module::ExecutionResult EPSRManagerModule::process(ModuleContext &moduleContext)
{
    // Loop over target data
    for (auto *module : target_)
    {
        auto *epsrModule = dynamic_cast<EPSRModule*>(module);
        auto &moduleData = moduleContext.dissolve().processingModuleData();
        auto potentials = epsrModule->empiricalPotentials();
    }
    return ExecutionResult::Success;
}
