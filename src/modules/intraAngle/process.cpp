// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "expression/variable.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/intraAngle/intraAngle.h"
#include "procedure/nodes/calculateAngle.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/operateExpression.h"
#include "procedure/nodes/select.h"

// Run main processing
Module::ExecutionResult IntraAngleModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
