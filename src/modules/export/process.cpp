/*
	*** Export Module - Processing
	*** src/modules/export/process.cpp
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

#include "modules/export/export.h"
#include "main/dissolve.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Run main processing
bool ExportModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * Export data
	 */

	/*
	 * Write coordinates
	 */

	if (coordinatesFormat_.hasValidFileAndFormat())
	{
		// Check for zero Configuration targets
		if (targetConfigurations_.nItems() == 0) Messenger::warn("No Configuration targets for Module.\n");
		else
		{
			// Loop over target Configurations
			for (RefListItem<Configuration,bool>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next)
			{
				// Grab Configuration pointer
				Configuration* cfg = ri->item;

				// Set up process pool - must do this to ensure we are using all available processes
				procPool.assignProcessesToGroups(cfg->processPool());

				// Only the pool master saves the data
				if (procPool.isMaster())
				{
					Messenger::print("Export: Writing coordinates file (%s) for Configuration '%s'...\n", coordinatesFormat_.niceFormat(), cfg->name());

					if (!ExportModule::writeCoordinates(coordinatesFormat_.coordinateFormat(), coordinatesFormat_.filename(), cfg))
					{
						Messenger::print("Export: Failed to export coordinates file '%s'.\n", coordinatesFormat_.filename());
						procPool.decideFalse();
						return false;
					}

					procPool.decideTrue();
				}
				else if (!procPool.decision()) return false;
			}
		}
	}

	/*
	 * Write PairPotentials
	 */

	if (pairPotentialFormat_.hasValidFileAndFormat())
	{
		// Only the pool master saves the data
		if (procPool.isMaster())
		{
			for (PairPotential* pp = dissolve.pairPotentials().first(); pp != NULL; pp = pp->next)
			{
				Messenger::print("Export: Writing pair potential file (%s) for %s-%s...\n", pairPotentialFormat_.niceFormat(), pp->atomTypeNameI(), pp->atomTypeNameJ());

				// Generate filename
				CharString ppName("%s-%s-%s.pp", pairPotentialFormat_.filename(), pp->atomTypeNameI(), pp->atomTypeNameJ());

				// Append pair potential
				if (!ExportModule::writePairPotential(pairPotentialFormat_.pairPotentialFormat(), ppName, pp))
				{
					Messenger::print("Export: Failed to export pair potential file '%s'.\n", ppName.get());
					procPool.decideFalse();
					return false;
				}

				procPool.decideTrue();
			}
		}
		else if (!procPool.decision()) return false; 
	}

	return true;
}

