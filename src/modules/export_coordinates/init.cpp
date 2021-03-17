// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/export_coordinates/exportcoords.h"

// Perform any necessary initialisation for the Module
void ExportCoordinatesModule::initialise()
{
    // Format
    keywords_.add("Format", new FileAndFormatKeyword(coordinatesFormat_, "EndFormat"), "Format",
                  "File / format for coordinates");
    keywords_.add("Format", new BoolKeyword(false), "TagWithIteration",
                  "Whether to tag (suffix) the filename with the current iteration index");
}
