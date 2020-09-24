// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/forces/forces.h"

// Perform any necessary initialisation for the Module
void ForcesModule::initialise()
{
    // Test
    keywords_.add("Test", new BoolKeyword(false), "Test", "Test parallel force routines against simplified, serial ones");
    keywords_.add("Test", new BoolKeyword(false), "TestAnalytic",
                  "Compare parallel force routines against exact (analytic) force rather than tabulated values");
    keywords_.add("Test", new BoolKeyword(true), "TestInter", "Include interatomic forces in test");
    keywords_.add("Test", new BoolKeyword(true), "TestIntra", "Include intramolecular forces in test");
    keywords_.add("Test", new FileAndFormatKeyword(referenceForces_, "EndTestReference"), "TestReference",
                  "Reference forces for test");
    keywords_.add("Test", new DoubleKeyword(0.1), "TestThreshold", "Threshold of force (%) at which test comparison will fail");

    // Export
    keywords_.add("Export", new FileAndFormatKeyword(exportedForces_, "EndSaveForces"), "SaveForces",
                  "File to save calculated forces to");
}
