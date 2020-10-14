// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"
#include "modules/geomopt/geomopt.h"

// Run main processing
bool GeometryOptimisationModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Retrieve Module options
    nCycles_ = keywords_.asInt("NCycles");
    tolerance_ = keywords_.asDouble("Tolerance");
    initialStepSize_ = keywords_.asDouble("StepSize");

    // Print argument/parameter summary
    Messenger::print("Optimise: Maximum number of cycles is {}.\n", nCycles_);
    Messenger::print("Optimise: Base convergence tolerance is {:e}.\n", tolerance_);
    Messenger::print("Optimise: Initial step size to be used is {:e}.\n", initialStepSize_);
    Messenger::print("\n");

    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Loop over target Configurations
    for (RefListItem<Configuration> *ri = targetConfigurations_.first(); ri != nullptr; ri = ri->next())
    {
        // Grab Configuration pointer
        Configuration *cfg = ri->item();

        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Initialise working arrays for coordinates and forces
        xRef_.initialise(cfg->nAtoms());
        yRef_.initialise(cfg->nAtoms());
        zRef_.initialise(cfg->nAtoms());
        xTemp_.initialise(cfg->nAtoms());
        yTemp_.initialise(cfg->nAtoms());
        zTemp_.initialise(cfg->nAtoms());
        xForce_.initialise(cfg->nAtoms());
        yForce_.initialise(cfg->nAtoms());
        zForce_.initialise(cfg->nAtoms());

        optimise<Configuration>(dissolve, procPool, cfg);
    }

    return true;
}
