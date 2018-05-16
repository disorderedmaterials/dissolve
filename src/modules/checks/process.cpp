/*
	*** Checks Module - Processing
	*** src/modules/checks/process.cpp
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

#include "modules/checks/checks.h"
#include "main/duq.h"
#include "classes/box.h"
#include "base/sysfunc.h"

// Return whether the Module has a processing stage
bool ChecksModule::hasProcessing()
{
	return true;
}

// Run main processing
bool ChecksModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Perform simple checks for the target Configuration(s)
	 * 
	 * This is a serial routine, with each process calculating the same values and comparing for consistency.
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

		// Get reference to relevant module data
		GenericList& moduleData = configurationLocal_ ? cfg->moduleData() : duq.processingModuleData();

		// Retrieve control parameters from Configuration
		const double angleThreshold = keywords_.asDouble("AngleThreshold");
		const double distanceThreshold = keywords_.asDouble("DistanceThreshold");

		Messenger::print("Checks: Threshold for distance checks is %f Angstroms\n", distanceThreshold);
		Messenger::print("Checks: Threshold for angle checks is %f degrees\n", angleThreshold);

		Atom** atoms = cfg->atoms().array();

		double actual, delta;
		bool ok;

		/*
		 * Check Distances
		 */

		// Loop over distances to check
		for (Geometry* d = distances_.first(); d != NULL; d = d->next)
		{
			actual = cfg->box()->minimumDistance(atoms[d->a()], atoms[d->b()]);
			delta = fabs(actual - d->value());
			ok = delta < distanceThreshold;
			Messenger::print("Distance between Atoms %i and %i is %f Angstroms, and is %s (delta = %f, tolerance = %f).\n", d->a()+1, d->b()+1, actual, ok ? "OK" : "NOT OK", delta, distanceThreshold);

			// Check consistency between processes
			if (!procPool.allTrue(ok))
			{
				Messenger::error("Failed consistency check between processes.\n");
				return false;
			}
		}

		/*
		 * Check Angles
		 */

		// Loop over angles to check
		for (Geometry* a = angles_.first(); a != NULL; a = a->next)
		{
			actual = cfg->box()->angleInDegrees(atoms[a->a()], atoms[a->b()], atoms[a->c()]);
			delta = fabs(actual - a->value());
			ok = delta < angleThreshold;
			Messenger::print("Angle between Atoms %i, %i and %i is %f degrees, and is %s (delta = %f, tolerance = %f).\n", a->a()+1, a->b()+1, a->c()+1, actual, ok ? "OK" : "NOT OK", delta, angleThreshold);

			// Check consistency between processes
			if (!procPool.allTrue(ok))
			{
				Messenger::error("Failed consistency check between processes.\n");
				return false;
			}
		}
	}

	return true;
}

