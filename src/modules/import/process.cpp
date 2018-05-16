/*
	*** Import Module
	*** src/modules/import.cpp
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "modules/import/import.h"
#include "main/duq.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Return whether the Module has a processing stage
bool ImportModule::hasProcessing()
{
	return true;
}

// Run main processing
bool ImportModule::process(DUQ& duq, ProcessPool& procPool)
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

		// Retrieve control parameters from Configuration
		// bool writeConfig = GenericListHelper<bool>::retrieve(cfg->moduleData(), "WriteConfig", uniqueName(), keywords_.asBool("WriteConfig"));

		/*
		 * Import Something
		 */
// 		if (writeConfig)
// 		{
// 			Messenger::print("Import: Writing DL_POLY CONFIG file for Configuration '%s'...\n", cfg->name());
// 
// 			// Only the pool master saves the data
// 			if (procPool.isMaster())
// 			{
// 				// Construct the filename
// 				CharString filename("%s.CONFIG", cfg->niceName());
// 
// 				// Open the file
// 				LineParser parser;
// 				if (!parser.openOutput(filename))
// 				{
// 					parser.closeFiles();
// 					procPool.stop();
// 					return false;
// 				}
// 				else if (!writeConfigurationDLPOLY(parser, cfg, cfg->name()))
// 				{
// 					Messenger::print("Import: Failed to export DL_POLY CONFIG file.\n");
// 					parser.closeFiles();
// 					procPool.stop();
// 					return false;
// 				}
// 
// 				procPool.proceed();
// 			}
// 			else if (!procPool.decision()) return false;
// 
// 			Messenger::print("Import: Finished writing DL_POLY CONFIG file.\n");
// 		}
	}

	return true;
}

