// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"
#include "modules/geomopt/geomopt.h"

// Run main processing
bool GeometryOptimisationModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Print argument/parameter summary
    Messenger::print("Optimise: Maximum number of cycles is {}.\n", maxCycles_);
    Messenger::print("Optimise: Base convergence tolerance is {:e}.\n", tolerance_);
    Messenger::print("Optimise: Initial step size to be used is {:e}.\n", initialStepSize_);
    Messenger::print("\n");

    // Check for zero Configuration targets
    if (targetConfigurations_.empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Loop over target Configurations
    for (auto *cfg : targetConfigurations_)
    {
        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Initialise working arrays for coordinates and forces
        rRef_.resize(cfg->nAtoms(), Vec3<double>());
        rTemp_.resize(cfg->nAtoms(), Vec3<double>());
        f_.resize(cfg->nAtoms(), Vec3<double>());

        optimise<Configuration>(dissolve.potentialMap(), procPool, cfg);
    }

    return true;
}
