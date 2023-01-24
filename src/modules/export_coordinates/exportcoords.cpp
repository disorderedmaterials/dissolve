// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/export_coordinates/exportcoords.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"

ExportCoordinatesModule::ExportCoordinatesModule() : Module("ExportCoordinates")
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("File");
    keywords_.add<FileAndFormatKeyword>("Format", "File / format for coordinates", coordinatesFormat_, "EndFormat");
    keywords_.add<BoolKeyword>("TagWithIteration", "Whether to tag (suffix) the filename with the current iteration index",
                               tagWithIteration_);
}
