// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/export_coordinates/exportcoords.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"

ExportCoordinatesModule::ExportCoordinatesModule() : Module("ExportCoordinates")
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Format
    keywords_.add<FileAndFormatKeyword>("Format", "Format", "File / format for coordinates", coordinatesFormat_, "EndFormat");
    keywords_.add<BoolKeyword>("Format", "TagWithIteration",
                               "Whether to tag (suffix) the filename with the current iteration index", tagWithIteration_);
}
