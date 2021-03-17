// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "main/dissolve.h"
#include "modules/export_trajectory/exporttraj.h"

// Run main processing
bool ExportTrajectoryModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    if (!trajectoryFormat_.hasValidFileAndFormat())
        Messenger::error("No valid file/format set for trajectory export.\n");

    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration target set for module '{}'.\n", uniqueName());

    // Loop over target Configurations
    auto *cfg = targetConfigurations_.firstItem();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    // Only the pool master saves the data
    if (procPool.isMaster())
    {
        Messenger::print("Export: Appending trajectory file ({}) for Configuration '{}'...\n", trajectoryFormat_.description(),
                         cfg->name());

        if (!trajectoryFormat_.exportData(cfg))
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
