// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atom.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "main/dissolve.h"
#include "modules/exportTrajectory/exportTrajectory.h"

// Run main processing
Module::ExecutionResult ExportTrajectoryModule::process(Dissolve &dissolve)
{
    if (!trajectoryFormat_.hasFilename())
    {
        Messenger::error("No valid file/format set for trajectory export.\n");
        return ExecutionResult::Failed;
    }

    Messenger::print("Export: Appending trajectory file ({}) for Configuration '{}'...\n",
                     trajectoryFormat_.formatDescription(), targetConfiguration_->name());

    if (!trajectoryFormat_.exportData(targetConfiguration_))
    {
        Messenger::print("Export: Failed to append trajectory file '{}'.\n", trajectoryFormat_.filename());
        return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
