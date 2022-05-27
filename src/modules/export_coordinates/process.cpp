// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/lineparser.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "main/dissolve.h"
#include "modules/export_coordinates/exportcoords.h"

// Run main processing
bool ExportCoordinatesModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    if (!coordinatesFormat_.hasFilename())
        Messenger::error("No valid file/format set for coordinate export.\n");

    std::string originalFilename{coordinatesFormat_.filename()};
    if (tagWithIteration_)
        coordinatesFormat_.setFilename(fmt::format("{}.{}", coordinatesFormat_.filename(), dissolve.iteration()));

    // Check for Configuration target
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", name());

    // Only the pool master saves the data
    if (procPool.isMaster())
    {
        Messenger::print("Export: Writing coordinates file ({}) for Configuration '{}'...\n", coordinatesFormat_.description(),
                         targetConfiguration_->name());

        if (!coordinatesFormat_.exportData(targetConfiguration_))
        {
            Messenger::print("Export: Failed to export coordinates file '{}'.\n", coordinatesFormat_.filename());
            procPool.decideFalse();
            return false;
        }

        procPool.decideTrue();
    }
    else if (!procPool.decision())
        return false;

    // Reset filename
    coordinatesFormat_.setFilename(originalFilename);

    return true;
}
