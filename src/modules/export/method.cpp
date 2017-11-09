/*
	*** Export Module - Method
	*** src/modules/export/method.cpp
	Copyright T. Youngs 2012-2017

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

#include "modules/export/export.h"
#include "main/duq.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Perform set up tasks for module
bool ExportModule::setUp(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool ExportModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool ExportModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Export data from the target Configuration(s)
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
		bool writeConfig = keywords_.asBool("WriteConfig");

		/*
		 * Write DL_POLY Config
		 */
		if (writeConfig)
		{
			Messenger::print("Export: Writing DL_POLY CONFIG file for Configuration '%s'...\n", cfg->name());

			// Only the pool master saves the data
			if (procPool.isMaster())
			{
				// Construct the filename
				CharString filename("%s.CONFIG", cfg->niceName());

				// Open the file
				LineParser parser;
				if (!parser.openOutput(filename))
				{
					parser.closeFiles();
					procPool.stop();
					return false;
				}
				else if (!writeConfigurationDLPOLY(parser, cfg, cfg->name()))
				{
					Messenger::print("Export: Failed to export DL_POLY CONFIG file.\n");
					parser.closeFiles();
					procPool.stop();
					return false;
				}

				procPool.proceed();
			}
			else if (!procPool.decision()) return false;

			Messenger::print("Export: Finished writing DL_POLY CONFIG file.\n");
		}
	}

	return true;
}

// Execute post-processing stage
bool ExportModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

