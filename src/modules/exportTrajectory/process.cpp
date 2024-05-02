// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atom.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/exportTrajectory/exportTrajectory.h"

// Run main processing
Module::ExecutionResult ExportTrajectoryModule::process(ModuleContext &moduleContext)
{
    if (!trajectoryFormat_.hasFilename())
    {
        Messenger::error("No valid file/format set for trajectory export.\n");
        return ExecutionResult::Failed;
    }

    // Check for Configuration target
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Only the pool master saves the data
    if (moduleContext.processPool().isMaster())
    {
        Messenger::print("Export: Appending trajectory file ({}) for Configuration '{}'...\n",
                         trajectoryFormat_.formatDescription(), targetConfiguration_->name());

        if (!trajectoryFormat_.exportData(targetConfiguration_))
        {
            Messenger::print("Export: Failed to append trajectory file '{}'.\n", trajectoryFormat_.filename());
            moduleContext.processPool().decideFalse();
            return ExecutionResult::Failed;
        }

        moduleContext.processPool().decideTrue();
    }
    else if (!moduleContext.processPool().decision())
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
