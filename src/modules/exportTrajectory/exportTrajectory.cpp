// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/exportTrajectory/exportTrajectory.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"

ExportTrajectoryModule::ExportTrajectoryModule() : Module(ModuleTypes::ExportTrajectory)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "File");
    keywords_.add<FileAndFormatKeyword>("Format", "File / format for trajectory", trajectoryFormat_, "EndFormat");

    executeIfTargetsUnchanged_ = true;
}
