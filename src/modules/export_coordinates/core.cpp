// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileandformat.h"
#include "modules/export_coordinates/exportcoords.h"

ExportCoordinatesModule::ExportCoordinatesModule() : Module()
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Format
    keywords_.add<FileAndFormatKeyword>("Format", "Format", "File / format for coordinates", coordinatesFormat_, "EndFormat");
    keywords_.add<BoolKeyword>("Format", "TagWithIteration",
                               "Whether to tag (suffix) the filename with the current iteration index", tagWithIteration_);
}

// Return type of module
std::string_view ExportCoordinatesModule::type() const { return "ExportCoordinates"; }
