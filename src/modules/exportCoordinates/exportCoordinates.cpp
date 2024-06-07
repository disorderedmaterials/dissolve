// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/exportCoordinates/exportCoordinates.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"

ExportCoordinatesModule::ExportCoordinatesModule() : Module(ModuleTypes::ExportCoordinates)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "File");
    keywords_.add<FileAndFormatKeyword>("Format", "File / format for coordinates", coordinatesFormat_, "EndFormat");
    keywords_.add<BoolKeyword>("TagWithIteration", "Whether to tag (suffix) the filename with the current iteration index",
                               tagWithIteration_);

    executeIfTargetsUnchanged_ = true;
}
