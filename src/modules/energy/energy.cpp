// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/energy/energy.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionaldouble.h"

EnergyModule::EnergyModule() : Module("Energy")
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Control
    keywords_.add<DoubleKeyword>("Control", "StabilityThreshold",
                                 "Threshold value at which energy is deemed stable over the defined windowing period",
                                 stabilityThreshold_, 0.0);
    keywords_.add<IntegerKeyword>("Control", "StabilityWindow",
                                  "Number of points over which to assess the stability of the energy (per Configuration)",
                                  stabilityWindow_, 1);

    // Test
    keywords_.add<BoolKeyword>(
        "Test", "Test", "Test parallel energy routines against basic serial versions and supplied reference values", test_);
    keywords_.add<BoolKeyword>("Test", "TestAnalytic",
                               "Use analytic interatomic energies rather than (production) tabulated potentials for tests",
                               testAnalytic_);
    keywords_.add<OptionalDoubleKeyword>("Test", "TestReferenceInter",
                                         "Reference value for interatomic energy against which to test calculated value",
                                         testReferenceInter_, -1.0e10, std::nullopt, 1.0e8, "Off");
    keywords_.add<OptionalDoubleKeyword>("Test", "TestReferenceIntra",
                                         "Reference value for intramolecular energy against which to test calculated value",
                                         testReferenceIntra_, -1.0e10, std::nullopt, 1.0e8, "Off");
    keywords_.add<DoubleKeyword>("Test", "TestThreshold", "Threshold of energy at which test comparison will fail",
                                 testThreshold_);

    // Export
    keywords_.add<BoolKeyword>("Export", "Save", "Save calculated energies to disk, one file per targetted configuration",
                               save_);
}
