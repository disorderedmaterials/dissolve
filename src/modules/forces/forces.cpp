// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/forces/forces.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/fileandformat.h"

ForcesModule::ForcesModule() : Module("Forces")
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Test
    keywords_.add<BoolKeyword>(
        "Test", "Test",
        "Test parallel force routines against basic serial versions and supplied reference values (if provided)", test_);
    keywords_.add<BoolKeyword>("Test", "TestAnalytic",
                               "Use analytic interatomic forces rather than (production) tabulated potentials for tests",
                               testAnalytic_);
    keywords_.add<BoolKeyword>("Test", "TestInter", "Include interatomic forces in test", testInter_);
    keywords_.add<BoolKeyword>("Test", "TestIntra", "Include intramolecular forces in test", testIntra_);
    keywords_.add<FileAndFormatKeyword>("Test", "TestReference", "Reference forces to test calculated forces against",
                                        referenceForces_, "EndTestReference");
    keywords_.add<DoubleKeyword>("Test", "TestThreshold", "Threshold of force (%) at which test comparison will fail",
                                 testThreshold_, 0.0);

    // Export
    keywords_.add<FileAndFormatKeyword>("Export", "SaveForces", "Save calculated energies to the specified file / format",
                                        exportedForces_, "EndSaveForces");
}
