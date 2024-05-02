// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/geomOpt/geomOpt.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"

GeometryOptimisationModule::GeometryOptimisationModule() : Module(ModuleTypes::GeometryOptimisation)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Control");
    keywords_.add<IntegerKeyword>("MaxCycles", "Maximum number of minimisation cycles to perform", maxCycles_, 1);
    keywords_.add<DoubleKeyword>("StepSize", "Initial step size to employ", initialStepSize_, 1.0e-10);
    keywords_.add<DoubleKeyword>("Tolerance", "Tolerance controlling convergence of algorithm)", tolerance_, 1.0e-10);
}
