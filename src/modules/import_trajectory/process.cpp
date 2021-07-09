// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "main/dissolve.h"
#include "modules/import_trajectory/importtraj.h"

// Run main processing
bool ImportTrajectoryModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (targetConfigurationsKeyword_.data().empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    auto *cfg = targetConfigurationsKeyword_.data().front();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    Messenger::print("Import: Reading trajectory file frame from '{}' into Configuration '{}'...\n",
                     trajectoryFormat_.filename(), cfg->name());

    // Open the file
    LineParser parser(&procPool);
    if ((!parser.openInput(trajectoryFormat_.filename())) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open trajectory file '{}'.\n", trajectoryFormat_.filename());

    // Does a seek position exist in the processing module info?
    std::string streamPosName = fmt::format("TrajectoryPosition_{}", cfg->niceName());
    if (dissolve.processingModuleData().contains(streamPosName, uniqueName()))
    {
        // Retrieve the streampos and go to it in the file
        std::streampos trajPos = dissolve.processingModuleData().retrieve<std::streampos>(streamPosName, uniqueName());
        parser.seekg(trajPos);
    }

    // Read the frame
    std::optional<Matrix3> unitCell;
    if (!trajectoryFormat_.importData(parser, cfg, unitCell))
        return Messenger::error("Failed to read trajectory frame data.\n");
    cfg->incrementContentsVersion();

    // Set the trajectory file position in the restart file
    dissolve.processingModuleData().realise<std::streampos>(streamPosName, uniqueName(), GenericItem::InRestartFileFlag) =
        parser.tellg();

    // Handle the unit cell if one was provided
    if (unitCell)
    {
        // Check that the unit cell has changed by an appreciable amount....
        if ((unitCell.value() - cfg->box()->axes()).maxAbs() > 1.0e-8)
        {
            // Create new Box and cells for the configuration
            cfg->createBox(unitCell.value());
            cfg->cells().generate(cfg->box(), cfg->requestedCellDivisionLength(), dissolve.pairPotentialRange());

            // Remove all atoms from cells
            for (auto &i : cfg->atoms())
                i->setCell(nullptr);
        }
    }

    // Make sure that the configuration contents are up-to-date w.r.t. cell locations etc.
    cfg->updateCellContents();

    return true;
}
