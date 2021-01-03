// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/forces/forces.h"

// Perform any necessary initialisation for the Module
void ForcesModule::initialise()
{
    // Test
    keywords_.add("Test", new BoolKeyword(false), "Test",
                  "Test parallel force routines against basic serial versions and supplied reference values (if provided)");
    keywords_.add("Test", new BoolKeyword(false), "TestAnalytic",
                  "Use analytic interatomic forces rather than (production) tabulated potentials for tests");
    keywords_.add("Test", new BoolKeyword(true), "TestInter", "Include interatomic forces in test");
    keywords_.add("Test", new BoolKeyword(true), "TestIntra", "Include intramolecular forces in test");
    keywords_.add("Test", new FileAndFormatKeyword(referenceForces_, "EndTestReference"), "TestReference",
                  "Reference forces to test calculated forces against");
    keywords_.add("Test", new DoubleKeyword(0.1), "TestThreshold", "Threshold of force (%) at which test comparison will fail");

    // Export
    keywords_.add("Export", new FileAndFormatKeyword(exportedForces_, "EndSaveForces"), "SaveForces",
                  "Save calculated energies to the specified file / format");
}
