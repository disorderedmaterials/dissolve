// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/dissolve.h"
#include "module/context.h"
#include "modules/energy/energy.h"
#include "modules/geomOpt/geomOpt.h"

// Run main processing
Module::ExecutionResult GeometryOptimisationModule::process(ModuleContext &moduleContext)
{
    // Print argument/parameter summary
    Messenger::print("Optimise: Maximum number of cycles is {}.\n", maxCycles_);
    Messenger::print("Optimise: Base convergence tolerance is {:e}.\n", tolerance_);
    Messenger::print("Optimise: Initial step size to be used is {:e}.\n", initialStepSize_);
    Messenger::print("\n");

    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Initialise working arrays for coordinates and forces
    rRef_.resize(targetConfiguration_->nAtoms(), Vec3<double>());
    rTemp_.resize(targetConfiguration_->nAtoms(), Vec3<double>());
    f_.resize(targetConfiguration_->nAtoms(), Vec3<double>());

    optimise<Configuration>(moduleContext.dissolve().potentialMap(), moduleContext.processPool(), targetConfiguration_);

    return ExecutionResult::Success;
}
