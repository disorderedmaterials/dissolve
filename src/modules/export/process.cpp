/*
	*** Export Module - Processing
	*** src/modules/export/process.cpp
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

#include "modules/export/export.h"
#include "main/duq.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Run pre-processing stage
bool ExportModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Run main processing
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
		bool writeDLPOLY = keywords_.asBool("WriteDLPOLY");
		bool writeXYZ = keywords_.asBool("WriteXYZ");

		/*
		 * Write DL_POLY CONFIG
		 */
		if (writeDLPOLY)
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
					procPool.decideFalse();
					return false;
				}
				else if (!writeConfigurationDLPOLY(parser, cfg, cfg->name()))
				{
					Messenger::print("Export: Failed to export DL_POLY CONFIG file.\n");
					parser.closeFiles();
					procPool.decideFalse();
					return false;
				}

				procPool.decideTrue();
			}
			else if (!procPool.decision()) return false;

			Messenger::print("Export: Finished writing DL_POLY CONFIG file.\n");
		}

		/*
		 * Write XYZ
		 */
		if (writeXYZ)
		{
			Messenger::print("Export: Writing XYZ file for Configuration '%s'...\n", cfg->name());

			// Only the pool master saves the data
			if (procPool.isMaster())
			{
				// Construct the filename
				CharString filename("%s.xyz", cfg->niceName());

				// Open the file
				LineParser parser;
				if (!parser.openOutput(filename))
				{
					parser.closeFiles();
					procPool.decideFalse();
					return false;
				}
				else if (!writeConfigurationXYZ(parser, cfg, cfg->name()))
				{
					Messenger::print("Export: Failed to export XYZ file.\n");
					parser.closeFiles();
					procPool.decideFalse();
					return false;
				}

				procPool.decideTrue();
			}
			else if (!procPool.decision()) return false;

			Messenger::print("Export: Finished writing XYZ file.\n");
		}

	}

	return true;
}

// Run post-processing stage
bool ExportModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

