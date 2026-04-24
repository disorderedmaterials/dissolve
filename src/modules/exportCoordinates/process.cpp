// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/lineParser.h"
#include "classes/atomType.h"
#include "classes/configurationAtom.h"
#include "main/dissolve.h"
#include "modules/exportCoordinates/exportCoordinates.h"

// Run main processing
Module::ExecutionResult ExportCoordinatesModule::process(Dissolve &dissolve)
{
    if (!coordinatesFormat_.hasFilename())
    {
        Messenger::error("No valid file/format set for coordinate export.\n");
        return ExecutionResult::Failed;
    }

    std::string originalFilename{coordinatesFormat_.filename()};
    if (tagWithIteration_)
        coordinatesFormat_.setFilename(std::format("{}.{}", coordinatesFormat_.filename(), dissolve.iteration()));

    Messenger::print("Export: Writing coordinates file ({}) for Configuration '{}'...\n",
                     coordinatesFormat_.formatDescription(), targetConfiguration_->name());

    if (!coordinatesFormat_.exportData(targetConfiguration_))
    {
        Messenger::print("Export: Failed to export coordinates file '{}'.\n", coordinatesFormat_.filename());
        return ExecutionResult::Failed;
    }

    // Reset filename
    coordinatesFormat_.setFilename(originalFilename);

    return ExecutionResult::Success;
}
