/*
	*** BraggSQ Module - Processing
	*** src/modules/braggsq/process.cpp
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

#include "modules/braggsq/braggsq.h"
#include "main/dissolve.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/weights.h"
#include "templates/genericlisthelper.h"

// Run main processing
bool BraggSQModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * Calculate Bragg contributions.
	 * 
	 * This is a serial routine, with each process constructing its own copy of the data.
	 * Partial calculation routines called by this routine are parallel.
	 */

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0)
	{
		Messenger::warn("No Configuration targets for Module.\n");
		return true;
	}

	const double qDelta = keywords_.asDouble("QDelta");
	const double qMax = keywords_.asDouble("QMax");
	const double qMin = keywords_.asDouble("QMin");
	const Vec3<int> multiplicity = keywords_.asVec3Int("Multiplicity");
	// TODO Could look for this value in the Configuration's module data (could be set/stored if a known crystal repeat was used in the generation of the Configuration).
	const bool savePartials = keywords_.asBool("SavePartials");
	const bool saveReflections = keywords_.asBool("SaveReflections");

	// Print argument/parameter summary
	Messenger::print("BraggSQ: Calculating Bragg S(Q) over %f < Q < %f Angstroms**-1 using bin size of %f Angstroms**-1.\n", qMin, qMax, qDelta);


	/*
	 * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or multiple independent Configurations,
	 * we must loop over the specified targetConfigurations_ and calculate the partials for each.
	 */
	RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Calculate Bragg vectors and intensities for the current Configuration
		bool alreadyUpToDate;
		if (!calculateBraggTerms(procPool, cfg, qMin, 0.001, qMax, multiplicity, alreadyUpToDate)) return false;

		// If we are already up-to-date, then theres nothing more to do for this Configuration
		if (alreadyUpToDate) continue;

		// Form partial Bragg S(Q)
		formBraggSQ(procPool, cfg, qMin, qDelta, qMax);

		// Save partials data?
		if (savePartials)
		{
			
		}

		// Save reflection data?
		if (saveReflections)
		{
			// Retrieve BraggPeak data from the Configuration's module data
			bool found = false;
			const Array<BraggPeak>& braggPeaks = GenericListHelper< Array<BraggPeak> >::value(cfg->moduleData(), "BraggPeaks", "", Array<BraggPeak>(), &found);
			if (!found) return Messenger::error("Failed to find BraggPeak array in module data for Configuration '%s'.\n", cfg->name());

			// Open a file and save the data
			LineParser braggParser(&procPool);
			if (!braggParser.openOutput(CharString("Bragg-%s.txt", cfg->niceName()))) return false;
			braggParser.writeLineF("#   ID      Q       nKVecs    Intensity(0,0)\n");
			for (int n = 0; n<braggPeaks.nItems(); ++n)
			{
				if (!braggParser.writeLineF("%6i  %10.6f  %8i  %10.6e\n", n, braggPeaks.constAt(n).q(), braggPeaks.constAt(n).nKVectors(), braggPeaks.constAt(n).intensity(0, 0))) return false;
			}
		}
	}

	return true;
}
