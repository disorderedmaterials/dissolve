// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/dAngle/dAngle.h"

// Run main processing
Module::ExecutionResult DAngleModule::process(ModuleContext &moduleContext)
{
    // Check for Configuration target
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
