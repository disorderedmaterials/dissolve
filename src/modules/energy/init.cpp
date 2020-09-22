// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/energy/energy.h"

// Perform any necessary initialisation for the Module
void EnergyModule::initialise()
{
    // Calculation
    keywords_.add("Calculation", new DoubleKeyword(0.001), "StabilityThreshold",
                  "Threshold value at which energy is deemed stable over the defined windowing period", "<value[0.0-1.0]>");
    keywords_.add("Calculation", new IntegerKeyword(10), "StabilityWindow",
                  "Number of points over which to assess the stability of the energy (per Configuration)");

    // Test
    keywords_.add("Test", new BoolKeyword(false), "Test", "Test parallel energy routines against simplified, serial ones");
    keywords_.add("Test", new BoolKeyword(false), "TestAnalytic",
                  "Compare parallel energy routines against exact (analytic) energy rather than tabulated values");
    keywords_.add("Test", new DoubleKeyword(0.0), "TestReferenceInter",
                  "Reference value for interatomic energy against which to test calculated value");
    keywords_.add("Test", new DoubleKeyword(0.0), "TestReferenceIntra",
                  "Reference value for intramolecular energy against which to test calculated value");
    keywords_.add("Test", new DoubleKeyword(0.1), "TestThreshold", "Threshold of energy at which test comparison will fail");

    // Export
    keywords_.add("Export", new BoolKeyword(false), "Save", "Save calculate energy points to the file '<name>.energy.txt'");
}
