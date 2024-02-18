// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/axisAngle/axisAngle.h"

// Run main processing
Module::ExecutionResult AxisAngleModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
