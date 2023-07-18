// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/geomOpt/geomOpt.h"

// Run main processing
enum Module::executionResult GeometryOptimisationModule::process(Dissolve &dissolve, const ProcessPool &procPool)
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
        return failed;
    }

    // Initialise working arrays for coordinates and forces
    rRef_.resize(targetConfiguration_->nAtoms(), Vec3<double>());
    rTemp_.resize(targetConfiguration_->nAtoms(), Vec3<double>());
    f_.resize(targetConfiguration_->nAtoms(), Vec3<double>());

    optimise<Configuration>(dissolve.potentialMap(), procPool, targetConfiguration_);

    return success;
}
