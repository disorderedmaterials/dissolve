// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/forces/forces.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/fileAndFormat.h"

ForcesModule::ForcesModule() : Module(ModuleTypes::Forces)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("SaveForces", "Save calculated energies to the specified file / format",
                                        exportedForces_, "EndSaveForces");

    keywords_.setOrganisation("Advanced");
    keywords_.add<BoolKeyword>("Test", "Test analytic forces against production values", test_);
    keywords_.add<DoubleKeyword>("TestThreshold", "Threshold of force (%) at which test comparison will fail", testThreshold_,
                                 0.0);

    executeIfTargetsUnchanged_ = true;
}
