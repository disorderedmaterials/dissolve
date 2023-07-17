// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atom.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "main/dissolve.h"
#include "modules/exportTrajectory/exportTrajectory.h"

// Run main processing
enum executionResult ExportTrajectoryModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    if (!trajectoryFormat_.hasFilename())
    {
        Messenger::error("No valid file/format set for trajectory export.\n");
        return failed;
    }

    // Check for Configuration target
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return failed;
    }

    // Only the pool master saves the data
    if (procPool.isMaster())
    {
        Messenger::print("Export: Appending trajectory file ({}) for Configuration '{}'...\n",
                         trajectoryFormat_.formatDescription(), targetConfiguration_->name());

        if (!trajectoryFormat_.exportData(targetConfiguration_))
        {
            Messenger::print("Export: Failed to append trajectory file '{}'.\n", trajectoryFormat_.filename());
            procPool.decideFalse();
            return failed;
        }

        procPool.decideTrue();
    }
    else if (!procPool.decision())
        return failed;

    return success;
}
