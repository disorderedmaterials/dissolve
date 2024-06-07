// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/energy/energy.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionalDouble.h"

EnergyModule::EnergyModule() : Module(ModuleTypes::Energy)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation(
        "Control", "Stability Detection",
        "Control detection of 'stable' energies. This may be used by other modules when deciding whether to run or not.");
    keywords_.add<DoubleKeyword>("StabilityThreshold",
                                 "Threshold value at which energy is deemed stable over the defined windowing period",
                                 stabilityThreshold_, 0.0);
    keywords_.add<IntegerKeyword>("StabilityWindow",
                                  "Number of points over which to assess the stability of the energy (per Configuration)",
                                  stabilityWindow_, 1);

    keywords_.setOrganisation("Advanced");
    keywords_.add<BoolKeyword>("Test", "Test production energy against analytic 'correct' values", test_);
    keywords_.add<DoubleKeyword>("TestThreshold", "Threshold of energy at which test comparison will fail", testThreshold_);

    keywords_.setOrganisation("Export");
    keywords_.add<BoolKeyword>("Save", "Save calculated energies to disk, one file per targeted configuration", save_);

    executeIfTargetsUnchanged_ = true;
}
