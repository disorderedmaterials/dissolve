/*
	*** Checks Module - Processing
	*** src/modules/checks/process.cpp
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

#include "modules/checks/checks.h"
#include "main/dissolve.h"
#include "classes/box.h"
#include "base/sysfunc.h"

// Run main processing
bool ChecksModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * Perform simple checks for the target Configuration(s)
	 * 
	 * This is a serial routine, with each process calculating the same values and comparing for consistency.
	 */

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0) return Messenger::error("No configuration targets set for module '%s'.\n", uniqueName());

	// Loop over target Configurations
	for (RefListItem<Configuration>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next())
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item();

		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

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
		for (Geometry* d = distances_.first(); d != NULL; d = d->next())
		{
			actual = cfg->box()->minimumDistance(atoms[d->indices(0)], atoms[d->indices(1)]);
			delta = fabs(actual - d->value());
			ok = delta < distanceThreshold;
			Messenger::print("Distance between Atoms %i and %i is %f Angstroms, and is %s (delta = %f, tolerance = %f).\n", d->indices(0)+1, d->indices(1)+1, actual, ok ? "OK" : "NOT OK", delta, distanceThreshold);

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
		for (Geometry* a = angles_.first(); a != NULL; a = a->next())
		{
			actual = cfg->box()->angleInDegrees(atoms[a->indices(0)], atoms[a->indices(1)], atoms[a->indices(2)]);
			delta = fabs(actual - a->value());
			ok = delta < angleThreshold;
			Messenger::print("Angle between Atoms %i, %i and %i is %f degrees, and is %s (delta = %f, tolerance = %f).\n", a->indices(0)+1, a->indices(1)+1, a->indices(2)+1, actual, ok ? "OK" : "NOT OK", delta, angleThreshold);

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

