// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/geomopt/geomopt.h"

// Perform any necessary initialisation for the Module
void GeometryOptimisationModule::initialise()
{
    // Method Control
    keywords_.add<IntegerKeyword>("Control", "MaxCycles", "Maximum number of minimisation cycles to perform", maxCycles_, 1);
    keywords_.add<DoubleKeyword>("Control", "StepSize", "Initial step size to employ", initialStepSize_, 1.0e-10);
    keywords_.add<DoubleKeyword>("Control", "Tolerance", "Tolerance controlling convergence of algorithm)", tolerance_,
                                 1.0e-10);
}
