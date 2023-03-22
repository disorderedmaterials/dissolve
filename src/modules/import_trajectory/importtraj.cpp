// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/import_trajectory/importtraj.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"

ImportTrajectoryModule::ImportTrajectoryModule() : Module(ModuleTypes::ImportTrajectory)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("File");
    keywords_.add<FileAndFormatKeyword>("Format", "File / format for trajectory", trajectoryFormat_, "EndFormat");
}
