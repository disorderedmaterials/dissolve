// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/forces/forces.h"

// Perform any necessary initialisation for the Module
void ForcesModule::initialise()
{
    // Test
    keywords_.add<BoolKeyword>(
        "Test", "Test",
        "Test parallel force routines against basic serial versions and supplied reference values (if provided)", test_);
    keywords_.add<BoolKeyword>("Test", "TestAnalytic",
                               "Use analytic interatomic forces rather than (production) tabulated potentials for tests",
                               testAnalytic_);
    keywords_.add<BoolKeyword>("Test", "TestInter", "Include interatomic forces in test", testInter_);
    keywords_.add<BoolKeyword>("Test", "TestIntra", "Include intramolecular forces in test", testIntra_);
    keywords_.add("Test", new FileAndFormatKeyword(referenceForces_, "EndTestReference"), "TestReference",
                  "Reference forces to test calculated forces against");
    keywords_.add<DoubleKeyword>("Test", "TestThreshold", "Threshold of force (%) at which test comparison will fail",
                                 testThreshold_, 0.0);

    // Export
    keywords_.add("Export", new FileAndFormatKeyword(exportedForces_, "EndSaveForces"), "SaveForces",
                  "Save calculated energies to the specified file / format");
}
