// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/randomBuffer.h"
#include "base/timer.h"
#include "data/atomicMasses.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"
#include "modules/dlPoly/dlPoly.h"

// Run main processing
Module::ExecutionResult DlPolyModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Save DlPoly CONTROL file with props
    if (moduleContext.processPool().isMaster())
    {
        Messenger::print("Export: Writing TESTTEST file ({}) for Configuration '{}'...\n",
                         dlPolyControlFormat_.formatDescription(), targetConfiguration_->name());

        if (!dlPolyControlFormat_.exportData(targetConfiguration_,
                                             capForces_,
                                             capForcesAt_,
                                             cutoffDistance_,
                                             timestepVariable_,
                                             fixedTimestep_,
                                             energyFrequency_,
                                             nSteps_,
                                             outputFrequency_,
                                             randomVelocities_,
                                             trajectoryFrequency_))
        {
            Messenger::print("Export: Failed to export coordinates file '{}'.\n", dlPolyControlFormat_.filename());
            moduleContext.processPool().decideFalse();
            return ExecutionResult::Failed;
        }

        moduleContext.processPool().decideTrue();
    }
    else if (!moduleContext.processPool().decision())
        return ExecutionResult::Failed;



    /*
     * Calculation End
     */

    return ExecutionResult::Success;
}
