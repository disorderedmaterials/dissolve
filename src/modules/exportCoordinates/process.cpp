// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineParser.h"
#include "classes/atom.h"
#include "classes/atomType.h"
#include "main/dissolve.h"
#include "modules/exportCoordinates/exportCoordinates.h"

// Run main processing
enum executionResult ExportCoordinatesModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    if (!coordinatesFormat_.hasFilename())
    {
        Messenger::error("No valid file/format set for coordinate export.\n");
        return failed;
    }

    std::string originalFilename{coordinatesFormat_.filename()};
    if (tagWithIteration_)
        coordinatesFormat_.setFilename(fmt::format("{}.{}", coordinatesFormat_.filename(), dissolve.iteration()));

    // Check for Configuration target
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return failed;
    }

    // Only the pool master saves the data
    if (procPool.isMaster())
    {
        Messenger::print("Export: Writing coordinates file ({}) for Configuration '{}'...\n",
                         coordinatesFormat_.formatDescription(), targetConfiguration_->name());

        if (!coordinatesFormat_.exportData(targetConfiguration_))
        {
            Messenger::print("Export: Failed to export coordinates file '{}'.\n", coordinatesFormat_.filename());
            procPool.decideFalse();
            return failed;
        }

        procPool.decideTrue();
    }
    else if (!procPool.decision())
        return failed;

    // Reset filename
    coordinatesFormat_.setFilename(originalFilename);

    return success;
}
