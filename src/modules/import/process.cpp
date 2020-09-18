/*
    *** Import Module
    *** src/modules/import.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "genericitems/listhelper.h"
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
                std::streampos trajPos =
                    GenericListHelper<std::streampos>::retrieve(dissolve.processingModuleData(), streamPosName, uniqueName());
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
            GenericListHelper<std::streampos>::realise(dissolve.processingModuleData(), streamPosName, uniqueName(),
                                                       GenericItem::InRestartFileFlag) = parser.tellg();
        }
    }

    return true;
}
