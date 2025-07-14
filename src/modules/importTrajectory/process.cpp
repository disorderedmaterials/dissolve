// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/configuration.h"
#include "main/dissolve.h"
#include "modules/importTrajectory/importTrajectory.h"

// Run main processing
Module::ExecutionResult ImportTrajectoryModule::process(Dissolve &dissolve)
{
    Messenger::print("Import: Reading trajectory file frame from '{}' into Configuration '{}'...\n",
                     trajectoryFormat_.filename(), targetConfiguration_->name());

    // Open the file
    LineParser parser;
    if ((!parser.openInput(trajectoryFormat_.filename())) || (!parser.isFileGoodForReading()))
    {
        Messenger::error("Couldn't open trajectory file '{}'.\n", trajectoryFormat_.filename());
        return ExecutionResult::Failed;
    }

    // Does a seek position exist in the processing module info?
    std::string streamPosName = std::format("TrajectoryPosition_{}", targetConfiguration_->name());
    if (dissolve.processingModuleData().contains(streamPosName, name()))
    {
        // Retrieve the streampos and go to it in the file
        std::streampos trajPos = dissolve.processingModuleData().retrieve<std::streampos>(streamPosName, name());
        parser.seekg(trajPos);
    }

    // Read the frame
    std::optional<Matrix3> unitCell;
    if (!trajectoryFormat_.importData(parser, targetConfiguration_, unitCell))
    {
        Messenger::error("Failed to read trajectory frame data.\n");
        return ExecutionResult::Failed;
    }

    targetConfiguration_->incrementContentsVersion();

    // Set the trajectory file position in the restart file
    dissolve.processingModuleData().realise<std::streampos>(streamPosName, name(), GenericItem::InRestartFileFlag) =
        parser.tellg();

    // Handle the unit cell if one was provided
    auto clearExistingLocations = false;
    if (unitCell)
    {
        // Check that the unit cell has changed by an appreciable amount....
        if ((unitCell.value() - targetConfiguration_->box()->axes()).maxAbs() > 1.0e-8)
        {
            // Create new Box and cells for the configuration
            targetConfiguration_->createBoxAndCells(unitCell.value(), dissolve.pairPotentialRange());

            clearExistingLocations = true;
        }
    }

    // Make sure that the configuration contents are up-to-date w.r.t. cell locations etc.
    targetConfiguration_->updateAtomLocations(clearExistingLocations);

    return ExecutionResult::Success;
}
