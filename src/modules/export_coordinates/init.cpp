// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/export_coordinates/exportcoords.h"

// Perform any necessary initialisation for the Module
void ExportCoordinatesModule::initialise()
{
    // Targets
    keywords_.add<ConfigurationKeyword>("Target", "Configuration", "Set target configuration for the module",
                                        targetConfiguration_);

    // Format
    keywords_.add<FileAndFormatKeyword>("Format", "Format", "File / format for coordinates", coordinatesFormat_, "EndFormat");
    keywords_.add<BoolKeyword>("Format", "TagWithIteration",
                               "Whether to tag (suffix) the filename with the current iteration index", tagWithIteration_);
}
