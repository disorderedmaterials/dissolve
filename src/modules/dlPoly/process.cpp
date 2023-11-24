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

// Return EnumOptions for TrajectoryKey
EnumOptions<DlPolyModule::TrajectoryKey> DlPolyModule::trajectoryKey()
{
    return EnumOptions<DlPolyModule::TrajectoryKey>(
        "TrajectoryKey",
        {{TrajectoryKey::pos, "pos"}, {TrajectoryKey::pos_vel, "pos-vel"}, {TrajectoryKey::pos_vel_force, "pos-vel-force"}});
}

// Return EnumOptions for CoulMethod
EnumOptions<DlPolyModule::CoulMethod> DlPolyModule::coulMethod()
{
    return EnumOptions<DlPolyModule::CoulMethod>(
        "CoulMethod",
        {{CoulMethod::off, "off"}, {CoulMethod::spme, "spme"}, {CoulMethod::dddp, "dddp"}, {CoulMethod::pairwise, "pairwise"}, {CoulMethod::reaction_field, "reaction_field"}, {CoulMethod::force_shifted,   "force_shifted"}});
}

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
                                             trajectoryFrequency_,
                                             trajectoryKey().keyword(trajectoryKey_),
                                             coulMethod().keyword(coulMethod_),
                                             coulPrecision_))
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
