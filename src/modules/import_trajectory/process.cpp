// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "main/dissolve.h"
#include "modules/import_trajectory/importtraj.h"

// Run main processing
bool ImportTrajectoryModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    // Check for Configuration target
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", name());

    Messenger::print("Import: Reading trajectory file frame from '{}' into Configuration '{}'...\n",
                     trajectoryFormat_.filename(), targetConfiguration_->name());

    // Open the file
    LineParser parser(&procPool);
    if ((!parser.openInput(trajectoryFormat_.filename())) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open trajectory file '{}'.\n", trajectoryFormat_.filename());

    // Does a seek position exist in the processing module info?
    std::string streamPosName = fmt::format("TrajectoryPosition_{}", targetConfiguration_->niceName());
    if (dissolve.processingModuleData().contains(streamPosName, name()))
    {
        // Retrieve the streampos and go to it in the file
        std::streampos trajPos = dissolve.processingModuleData().retrieve<std::streampos>(streamPosName, name());
        parser.seekg(trajPos);
    }

    // Read the frame
    std::optional<Matrix3> unitCell;
    if (!trajectoryFormat_.importData(parser, targetConfiguration_, unitCell))
        return Messenger::error("Failed to read trajectory frame data.\n");
    targetConfiguration_->incrementContentsVersion();

    // Set the trajectory file position in the restart file
    dissolve.processingModuleData().realise<std::streampos>(streamPosName, name(), GenericItem::InRestartFileFlag) =
        parser.tellg();

    // Handle the unit cell if one was provided
    auto clearExistingAtoms = false;
    if (unitCell)
    {
        // Check that the unit cell has changed by an appreciable amount....
        if ((unitCell.value() - targetConfiguration_->box()->axes()).maxAbs() > 1.0e-8)
        {
            // Create new Box and cells for the configuration
            targetConfiguration_->createBoxAndCells(unitCell.value(), dissolve.pairPotentialRange());

            clearExistingAtoms = true;
        }
    }

    // Make sure that the configuration contents are up-to-date w.r.t. cell locations etc.
    targetConfiguration_->updateCellContents(clearExistingAtoms);

    return true;
}
