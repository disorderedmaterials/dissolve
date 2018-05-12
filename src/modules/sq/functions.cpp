/*
	*** SQ Module - Functions
	*** src/modules/sq/functions.cpp
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

#include "modules/sq/sq.h"
#include "classes/configuration.h"
// #include "classes/atom.h"
#include "classes/box.h"
// #include "classes/braggpeak.h"
// #include "classes/cell.h"
// #include "classes/kvector.h"
// #include "classes/species.h"
// #include "classes/weights.h"
// #include "math/broadeningfunction.h"
// #include "templates/array3d.h"
#include "templates/genericlisthelper.h"

/*
 * Public Functions
 */

// Generate S(Q) from supplied g(r)
bool SQModule::calculateUnweightedSQ(ProcessPool& procPool, Configuration* cfg, PartialSet& unweightedgr, PartialSet& unweightedsq, double qMin, double qDelta, double qMax, double rho, const WindowFunction& windowFunction, const BroadeningFunction& broadening)
{
	// Copy partial g(r) into our new S(Q) matrices
	Messenger::printVerbose("  --> Copying partial g(r) into S(Q) matrices...\n");
	int nTypes = unweightedgr.nAtomTypes();
	for (int n=0; n<nTypes; ++n)
	{
		for (int m=n; m<nTypes; ++m)
		{
			// Copy g(r) data into our arrays
			// Subtract 1.0 from the full and unbound partials so as to give (g(r)-1)
			// Don't subtract 1.0 from the bound partials
			unweightedsq.partial(n,m).copyData(unweightedgr.partial(n,m));
			unweightedsq.partial(n,m).arrayY() -= 1.0;
			unweightedsq.boundPartial(n,m).copyData(unweightedgr.boundPartial(n,m));
// 			unweightedsq.boundPartial(n,m).arrayY() -= 1.0;
			unweightedsq.unboundPartial(n,m).copyData(unweightedgr.unboundPartial(n,m));
			unweightedsq.unboundPartial(n,m).arrayY() -= 1.0;
		}
	}

	// Perform FT of partial g(r) into S(Q)
	// TODO Parallelise this
	procPool.resetAccumulatedTime();
	Timer timer;
	timer.start();
	for (int n=0; n<nTypes; ++n)
	{
		for (int m=n; m<nTypes; ++m)
		{
			if (!unweightedsq.partial(n,m).sineFT(4.0*PI*rho, qMin, qDelta, qMax, windowFunction, broadening)) return false;
			if (!unweightedsq.boundPartial(n,m).sineFT(4.0*PI*rho, qMin, qDelta, qMax, windowFunction, broadening)) return false;
			if (!unweightedsq.unboundPartial(n,m).sineFT(4.0*PI*rho, qMin, qDelta, qMax, windowFunction, broadening)) return false;

			// Zero Bragg partial, leave x array intact for use if needed
			unweightedsq.braggPartial(n,m).templateFrom(unweightedsq.partial(n,m));
		}
	}

	// Sum into total
	unweightedsq.formTotal(true);

	timer.stop();
	Messenger::print("SQ: Finished Fourier transform and summation of partial g(r) into partial S(Q) (%s elapsed, %s comms).\n", timer.totalTimeString(), procPool.accumulatedTimeString());

	/*
	 * S(Q) are now up-to-date
	 */

	unweightedsq.setFingerprint(unweightedgr.fingerprint());

	return true;
}

// Sum unweighted S(Q) over supplied Module's target Configurations
bool SQModule::sumUnweightedSQ(ProcessPool& procPool, Module* module, GenericList& moduleData, PartialSet& summedSQ)
{
	// Create an AtomTypeList containing the sum of atom types over all target configurations
	AtomTypeList combinedAtomTypes;
	RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
	while (Configuration* cfg = configIterator.iterate()) combinedAtomTypes.add(cfg->usedAtomTypesList());

	// Finalise and print the combined AtomTypes matrix
	combinedAtomTypes.finalise();
	combinedAtomTypes.print();

	// Set up partial S(Q) container
	summedSQ.setUpPartials(combinedAtomTypes, module->uniqueName(), "unweighted", "sq", "Q, 1/Angstroms");
	summedSQ.setObjectNames(CharString("%s//UnweightedSQ", module->uniqueName()));

	// Loop over Configurations again, summing into the PartialSet we have just set up
	// We will keep a running total of the weights associated with each Configuration, and re-weight the entire set of partials at the end.
	double totalWeight = 0.0, density = 0.0, volume = 0.0;
	CharString fingerprint;
	configIterator.restart();
	while (Configuration* cfg = configIterator.iterate())
	{
		// Update fingerprint
		fingerprint += fingerprint.isEmpty() ? CharString("%i", cfg->coordinateIndex()) : CharString("_%i", cfg->coordinateIndex());

		// Get weighting factor for this Configuration to contribute to the summed partials
		double weight = GenericListHelper<double>::retrieve(moduleData, CharString("%s_Weight", cfg->niceName()), module->uniqueName(), 1.0);
		totalWeight += weight;
		Messenger::print("SQ: Weight for Configuration '%s' is %f (total weight is now %f).\n", cfg->name(), weight, totalWeight);

		// Grab partials for Configuration and add into our set
		if (!cfg->moduleData().contains("UnweightedSQ", "SQ")) return Messenger::error("Couldn't find UnweightedSQ data for Configuration '%s'.\n", cfg->name());
		PartialSet& cfgPartialSQ = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "", "SQ");
		summedSQ.addPartials(cfgPartialSQ, weight);

		// Sum density weighted by volume, and total volume (both of which are multiplied by the overall weight)
		density += cfg->atomicDensity()*cfg->box()->volume()*weight;
		volume += cfg->box()->volume()*weight;
	}
	density /= volume;
	summedSQ.setFingerprint(fingerprint);

	// Now must normalise our partials to the overall weight of the source configurations
	// TODO Is this correct?
	summedSQ.reweightPartials(1.0 / totalWeight);

	// Store the blended density of our partials
	GenericListHelper<double>::realise(moduleData, "Density", module->uniqueName(), GenericItem::InRestartFileFlag) = density;

	return true;
}
