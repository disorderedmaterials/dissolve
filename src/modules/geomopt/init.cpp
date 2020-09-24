// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/geomopt/geomopt.h"

// Perform any necessary initialisation for the Module
void GeometryOptimisationModule::initialise()
{
    // Method Control
    keywords_.add("Calculation", new IntegerKeyword(1000, 1), "NCycles", "Maximum number of minimisation cycles to perform");
    keywords_.add("Calculation", new DoubleKeyword(1.0e-5, 1.0e-10), "StepSize", "Initial step size to employ");
    keywords_.add("Calculation", new DoubleKeyword(1.0e-4, 1.0e-10), "Tolerance",
                  "Tolerance controlling convergence of algorithm)");
}
