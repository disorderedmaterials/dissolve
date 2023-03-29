// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/export_trajectory/exporttraj.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"

ExportTrajectoryModule::ExportTrajectoryModule() : Module(ModuleTypes::ExportTrajectory)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("File");
    keywords_.add<FileAndFormatKeyword>("Format", "File / format for trajectory", trajectoryFormat_, "EndFormat");
}
