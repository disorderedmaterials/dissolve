// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "main/dissolve.h"
#include "modules/export_trajectory/exporttraj.h"

// Run main processing
bool ExportTrajectoryModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    if (!trajectoryFormat_.hasFilename())
        Messenger::error("No valid file/format set for trajectory export.\n");

    // Check for Configuration target
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", name());

    // Only the pool master saves the data
    if (procPool.isMaster())
    {
        Messenger::print("Export: Appending trajectory file ({}) for Configuration '{}'...\n",
                         trajectoryFormat_.formatDescription(), targetConfiguration_->name());

        if (!trajectoryFormat_.exportData(targetConfiguration_))
        {
            Messenger::print("Export: Failed to append trajectory file '{}'.\n", trajectoryFormat_.filename());
            procPool.decideFalse();
            return false;
        }

        procPool.decideTrue();
    }
    else if (!procPool.decision())
        return false;

    return true;
}