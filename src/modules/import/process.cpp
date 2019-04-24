/*
	*** Import Module
	*** src/modules/import.cpp
	Copyright T. Youngs 2012-2019

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

#include "modules/import/import.h"
#include "main/dissolve.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Run main processing
bool ImportModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * Import data to the target Configuration(s)
	 */

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0)
	{
		Messenger::warn("No Configuration targets for Module.\n");
		return true;
	}

	// Loop over target Configurations
	for (RefListItem<Configuration,bool>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next)
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item;

		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Retrieve control parameters
		bool readTrajectory = keywords_.asBool("ReadTrajectory");

		/*
		 * Import Trajectory Frame?
		 */
		if (readTrajectory)
		{
			Messenger::print("Import: Reading trajectory file frame from '%s' into Configuration '%s'...\n", trajectoryFile_.filename(), cfg->name());

			// Open the file
			LineParser parser(&procPool);
			if ((!parser.openInput(trajectoryFile_.filename())) || (!parser.isFileGoodForReading())) return Messenger::error("Couldn't open trajectory file '%s'.\n", trajectoryFile_.filename());

			// Does a seek position exist in the processing module info?
			CharString streamPosName("TrajectoryPosition_%s", cfg->niceName());
			if (dissolve.processingModuleData().contains(streamPosName, uniqueName()))
			{
				// Retrieve the streampos and go to it in the file
				streampos trajPos = GenericListHelper<streampos>::retrieve(dissolve.processingModuleData(), streamPosName, uniqueName());
				parser.seekg(trajPos);
			}

			// Read the frame
			switch (trajectoryFile_.trajectoryFormat())
			{
				case (TrajectoryImportFileFormat::XYZTrajectory):
					if (!cfg->loadCoordinates(parser, CoordinateImportFileFormat::XYZCoordinates)) return false;
					cfg->incrementContentsVersion();
					break;
				default:
					return Messenger::error("Bad TGAY - he hasn't implemented reading of trajectory frames of format %i.\n", trajectoryFile_.trajectoryFormat());
					break;
			}

			// Set the trajectory file position in the restart file
			GenericListHelper<streampos>::realise(dissolve.processingModuleData(), streamPosName, uniqueName(), GenericItem::InRestartFileFlag) = parser.tellg();
		}
	}

	return true;
}
