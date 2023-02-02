// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/energy/energy.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionaldouble.h"

EnergyModule::EnergyModule() : Module("Energy")
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Control");
    keywords_.add<DoubleKeyword>("StabilityThreshold",
                                 "Threshold value at which energy is deemed stable over the defined windowing period",
                                 stabilityThreshold_, 0.0);
    keywords_.add<IntegerKeyword>("StabilityWindow",
                                  "Number of points over which to assess the stability of the energy (per Configuration)",
                                  stabilityWindow_, 1);

    keywords_.setOrganisation("Advanced");
    keywords_.add<BoolKeyword>(
        "Test", "Test parallel energy routines against basic serial versions and supplied reference values", test_);
    keywords_.add<BoolKeyword>("TestAnalytic",
                               "Use analytic interatomic energies rather than (production) tabulated potentials for tests",
                               testAnalytic_);
    keywords_.add<OptionalDoubleKeyword>("TestReferenceInter",
                                         "Reference value for interatomic energy against which to test calculated value",
                                         testReferenceInter_, -1.0e10, std::nullopt, 1.0e8, "Off");
    keywords_.add<OptionalDoubleKeyword>("TestReferenceIntra",
                                         "Reference value for intramolecular energy against which to test calculated value",
                                         testReferenceIntra_, -1.0e10, std::nullopt, 1.0e8, "Off");
    keywords_.add<DoubleKeyword>("TestThreshold", "Threshold of energy at which test comparison will fail", testThreshold_);

    keywords_.setOrganisation("Export");
    keywords_.add<BoolKeyword>("Save", "Save calculated energies to disk, one file per targetted configuration", save_);
}
