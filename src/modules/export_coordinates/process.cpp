// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "main/dissolve.h"
#include "modules/export_coordinates/exportcoords.h"

// Run main processing
bool ExportCoordinatesModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    const auto tagWithIteration = keywords_.asBool("TagWithIteration");

    // Copy coordinates format
    CoordinateExportFileFormat format(coordinatesFormat_.filename(), coordinatesFormat_.coordinateFormat());
    if (tagWithIteration)
        format.setFilename(fmt::format("{}.{}", coordinatesFormat_.filename(), dissolve.iteration()));

    if (!format.hasValidFileAndFormat())
        Messenger::error("No valid file/format set for coordinate export.\n");

    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    auto *cfg = targetConfigurations_.firstItem();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    // Only the pool master saves the data
    if (procPool.isMaster())
    {
        Messenger::print("Export: Writing coordinates file ({}) for Configuration '{}'...\n", format.description(),
                         cfg->name());

        if (!format.exportData(cfg))
        {
            Messenger::print("Export: Failed to export coordinates file '{}'.\n", format.filename());
            procPool.decideFalse();
            return false;
        }

        procPool.decideTrue();
    }
    else if (!procPool.decision())
        return false;

    return true;
}
