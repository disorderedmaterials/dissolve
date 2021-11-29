// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/configuration.h"
#include "keywords/fileandformat.h"
#include "modules/import_trajectory/importtraj.h"

ImportTrajectoryModule::ImportTrajectoryModule() : Module()
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Format
    keywords_.add<FileAndFormatKeyword>("Format", "Format", "File / format for trajectory", trajectoryFormat_, "EndFormat");
}

// Return type of module
std::string_view ImportTrajectoryModule::type() const { return "ImportTrajectory"; }
