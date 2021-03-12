// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "main/dissolve.h"
#include "modules/import/import.h"

// Run main processing
bool ImportModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Import data to the target Configuration(s)
     */

    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Loop over target Configurations
    for (RefListItem<Configuration> *ri = targetConfigurations_.first(); ri != nullptr; ri = ri->next())
    {
        // Grab Configuration pointer
        Configuration *cfg = ri->item();

        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Retrieve control parameters
        auto readTrajectory = keywords_.asBool("ReadTrajectory");

        /*
         * Import Trajectory Frame?
         */
        if (readTrajectory)
        {
            Messenger::print("Import: Reading trajectory file frame from '{}' into Configuration '{}'...\n",
                             trajectoryFile_.filename(), cfg->name());

            // Open the file
            LineParser parser(&procPool);
            if ((!parser.openInput(trajectoryFile_.filename())) || (!parser.isFileGoodForReading()))
                return Messenger::error("Couldn't open trajectory file '{}'.\n", trajectoryFile_.filename());

            // Does a seek position exist in the processing module info?
            std::string streamPosName = fmt::format("TrajectoryPosition_{}", cfg->niceName());
            if (dissolve.processingModuleData().contains(streamPosName, uniqueName()))
            {
                // Retrieve the streampos and go to it in the file
                std::streampos trajPos = dissolve.processingModuleData().retrieve<std::streampos>(streamPosName, uniqueName());
                parser.seekg(trajPos);
            }

            // Read the frame
            switch (trajectoryFile_.trajectoryFormat())
            {
                case (TrajectoryImportFileFormat::XYZTrajectory):
                    if (!cfg->loadCoordinates(parser, CoordinateImportFileFormat::XYZCoordinates))
                        return false;
                    cfg->incrementContentsVersion();
                    break;
                default:
                    return Messenger::error("Bad TGAY - he hasn't implemented reading of trajectory frames of format {}.\n",
                                            trajectoryFile_.trajectoryFormat());
                    break;
            }

            // Set the trajectory file position in the restart file
            dissolve.processingModuleData().realise<std::streampos>(streamPosName, uniqueName(),
                                                                    GenericList::InRestartFileFlag) = parser.tellg();
        }
    }

    return true;
}
