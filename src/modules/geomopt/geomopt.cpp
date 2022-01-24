// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/geomopt/geomopt.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"

GeometryOptimisationModule::GeometryOptimisationModule() : Module("GeometryOptimisation")
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Method Control
    keywords_.add<IntegerKeyword>("Control", "MaxCycles", "Maximum number of minimisation cycles to perform", maxCycles_, 1);
    keywords_.add<DoubleKeyword>("Control", "StepSize", "Initial step size to employ", initialStepSize_, 1.0e-10);
    keywords_.add<DoubleKeyword>("Control", "Tolerance", "Tolerance controlling convergence of algorithm)", tolerance_,
                                 1.0e-10);
}
