// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "classes/atom.h"
#include "classes/atomType.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/exportCoordinates/exportCoordinates.h"

// Run main processing
Module::ExecutionResult ExportCoordinatesModule::process(ModuleContext &moduleContext)
{
    if (!coordinatesFormat_.hasFilename())
    {
        Messenger::error("No valid file/format set for coordinate export.\n");
        return ExecutionResult::Failed;
    }

    std::string originalFilename{coordinatesFormat_.filename()};
    if (tagWithIteration_)
        coordinatesFormat_.setFilename(
            fmt::format("{}.{}", coordinatesFormat_.filename(), moduleContext.dissolve().iteration()));

    // Check for Configuration target
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Only the pool master saves the data
    if (moduleContext.processPool().isMaster())
    {
        Messenger::print("Export: Writing coordinates file ({}) for Configuration '{}'...\n",
                         coordinatesFormat_.formatDescription(), targetConfiguration_->name());

        if (!coordinatesFormat_.exportData(targetConfiguration_))
        {
            Messenger::print("Export: Failed to export coordinates file '{}'.\n", coordinatesFormat_.filename());
            moduleContext.processPool().decideFalse();
            return ExecutionResult::Failed;
        }

        moduleContext.processPool().decideTrue();
    }
    else if (!moduleContext.processPool().decision())
        return ExecutionResult::Failed;

    // Reset filename
    coordinatesFormat_.setFilename(originalFilename);

    return ExecutionResult::Success;
}
