/*
	*** SQ Module - Functions
	*** src/modules/sq/functions.cpp
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

#include "modules/sq/sq.h"
#include "math/ft.h"
#include "classes/configuration.h"
#include "classes/box.h"
#include "genericitems/listhelper.h"

/*
 * Public Functions
 */

// Generate S(Q) from supplied g(r)
bool SQModule::calculateUnweightedSQ(ProcessPool& procPool, Configuration* cfg, const PartialSet& unweightedgr, PartialSet& unweightedsq, double qMin, double qDelta, double qMax, double rho, const WindowFunction& windowFunction, const BroadeningFunction& broadening)
{
	// Copy partial g(r) into our new S(Q) object - it should have been initialised already, so we will just check its size
	if (unweightedgr.nAtomTypes() != unweightedsq.nAtomTypes()) return Messenger::error("SQModule::calculateUnweightedSQ - sizes of supplied partial sets are different.\n");

	// Subtract 1.0 from the full and unbound partials so as to give (g(r)-1) and FT into S(Q)
	// Don't subtract 1.0 from the bound partials
	// TODO Parallelise this
	procPool.resetAccumulatedTime();
	Timer timer;
	timer.start();
	int nTypes = unweightedgr.nAtomTypes();
	for (int n=0; n<nTypes; ++n)
	{
		for (int m=n; m<nTypes; ++m)
		{
			// Total partial
			unweightedsq.partial(n,m).copyArrays(unweightedgr.constPartial(n,m));
			unweightedsq.partial(n,m).values() -= 1.0;
			if (!Fourier::sineFT(unweightedsq.partial(n,m), 4.0*PI*rho, qMin, qDelta, qMax, windowFunction, broadening)) return false;

			// Bound partial
			unweightedsq.boundPartial(n,m).copyArrays(unweightedgr.constBoundPartial(n,m));
			if (!Fourier::sineFT(unweightedsq.boundPartial(n,m), 4.0*PI*rho, qMin, qDelta, qMax, windowFunction, broadening)) return false;

			// Unbound partial
			unweightedsq.unboundPartial(n,m).copyArrays(unweightedgr.constUnboundPartial(n,m));
			unweightedsq.unboundPartial(n,m).values() -= 1.0;
			if (!Fourier::sineFT(unweightedsq.unboundPartial(n,m), 4.0*PI*rho, qMin, qDelta, qMax, windowFunction, broadening)) return false;
		}
	}

	// Sum into total
	unweightedsq.formTotal(true);

	timer.stop();
	Messenger::print("Finished Fourier transform and summation of partial g(r) into partial S(Q) (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());

	return true;
}

// Sum unweighted S(Q) over the supplied Module's target Configurations
bool SQModule::sumUnweightedSQ(ProcessPool& procPool, Module* module, GenericList& moduleData, PartialSet& summedUnweightedSQ)
{
	// Create an AtomTypeList containing all AtomTypes present in all target configurations
	AtomTypeList combinedAtomTypes;
	for (Configuration* cfg : module->targetConfigurations()) combinedAtomTypes.add(cfg->usedAtomTypesList());
	combinedAtomTypes.finalise();

	// Set up PartialSet container
	summedUnweightedSQ.setUpPartials(combinedAtomTypes, module->uniqueName(), "unweighted", "sq", "Q, 1/Angstroms");
	summedUnweightedSQ.setObjectTags(CharString("%s//UnweightedSQ", module->uniqueName()));

	// Loop over Configurations again, summing into the PartialSet we have just set up
	// We will keep a running total of the weights associated with each Configuration, and re-weight the entire set of partials at the end.
	double totalWeight = 0.0;
	CharString fingerprint;
	for (Configuration* cfg : module->targetConfigurations())
	{
		// Update fingerprint
		fingerprint += fingerprint.isEmpty() ? CharString("%i", cfg->contentsVersion()) : CharString("_%i", cfg->contentsVersion());

		// Get weighting factor for this Configuration to contribute to the summed partials
		double weight = GenericListHelper<double>::value(moduleData, CharString("ConfigurationWeight_%s", cfg->niceName()), module->uniqueName(), 1.0);
		totalWeight += weight;
		Messenger::print("Weight for Configuration '%s' is %f (total weight is now %f).\n", cfg->name(), weight, totalWeight);

		// Grab partials for Configuration and add into our set
		if (!cfg->moduleData().contains("UnweightedSQ")) return Messenger::error("Couldn't find UnweightedSQ data for Configuration '%s'.\n", cfg->name());
		PartialSet cfgPartialSQ = GenericListHelper<PartialSet>::value(cfg->moduleData(), "UnweightedSQ");
		summedUnweightedSQ.addPartials(cfgPartialSQ, weight);
	}
	summedUnweightedSQ.setFingerprint(fingerprint);

	// Now must normalise our partials to the overall weight of the source configurations
	// TODO CHECK Is this correct?
	summedUnweightedSQ *= 1.0 / totalWeight;

	return true;
}
