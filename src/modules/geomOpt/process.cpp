// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/geomOpt/geomOpt.h"

// Run main processing
Module::ExecutionResult GeometryOptimisationModule::process(Dissolve &dissolve)
{
    // Print argument/parameter summary
    Messenger::print("Optimise: Maximum number of cycles is {}.\n", maxCycles_);
    Messenger::print("Optimise: Base convergence tolerance is {:e}.\n", tolerance_);
    Messenger::print("Optimise: Initial step size to be used is {:e}.\n", initialStepSize_);
    Messenger::print("\n");

    // Initialise working arrays for coordinates and forces
    rRef_.resize(targetConfiguration_->nAtoms(), Vector3());
    rTemp_.resize(targetConfiguration_->nAtoms(), Vector3());
    f_.resize(targetConfiguration_->nAtoms(), Vector3());

    optimise<Configuration>(dissolve.potentialMap(), targetConfiguration_);

    return ExecutionResult::Success;
}
