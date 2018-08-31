/*
	*** Bragg Module - Processing
	*** src/modules/bragg/process.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/bragg/bragg.h"
#include "main/dissolve.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/weights.h"
#include "templates/genericlisthelper.h"

// Run main processing
bool BraggModule::process(Dissolve& dissolve, ProcessPool& procPool)
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

	CharString varName;

	//GenericList& moduleData = configurationLocal_ ? targetConfigurations_.firstItem()->moduleData() : dissolve.processingModuleData();

	// Print argument/parameter summary
	// TODO

	/*
	 * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or multiple independent Configurations,
	 * we must loop over the specified targetConfigurations_ and calculate the Bragg contributions for each.
	 */
	Weights combinedWeights;
	AtomTypeList combinedAtomTypes;
	RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Update total AtomTypeList
		combinedAtomTypes.add(cfg->usedAtomTypesList());

		// Calculate unweighted partials for this Configuration (under generic Module name 'Bragg', rather than the uniqueName_)
		//bool alreadyUpToDate;
// 		calculateBraggTerms(procPool, cfg, method, allIntra, alreadyUpToDate);
//		const PartialSet& unweightedgr = GenericListHelper<PartialSet>::value(cfg->moduleData(), "Intensities", "Bragg");

// 		// Set names of resources (Data1D) within the PartialSet
// 		unweightedgr.setObjectTags(CharString("%s//%s//%s", cfg->niceName(), "Bragg", "UnweightedGR"));
// 
// 		// If we are associated to a local Configuration, copy the partial data over to the processing module list
// 		if (configurationLocal_) GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "UnweightedGR", uniqueName_) = unweightedgr;
	}

	// If we are a main processing task, construct the weighted sum of Configuration partials and store in the processing module data list
	if (!configurationLocal_)
	{
		// Assemble partials from all target Configurations specified, weighting them accordingly
		CharString varName;
		CharString fingerprint;

		// Finalise and print the combined AtomTypes matrix
		Messenger::print("Partials: AtomTypes used over all source Configurations:\n");
		combinedAtomTypes.finalise();
		combinedAtomTypes.print();

// // 		// Set up partial set using the AtomTypeList we have just constructed.
// // 		Configuration* refConfig = targetConfigurations_.firstItem();
// // 		PartialSet& unweightedgr = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "UnweightedGR", uniqueName_, GenericItem::InRestartFileFlag);
// // 		unweightedgr.setUp(combinedAtomTypes, refConfig->rdfRange(), refConfig->rdfBinWidth(), uniqueName(), "unweighted", "rdf", "r, Angstroms");
// // 		unweightedgr.setObjectTags(CharString("%s//UnweightedGR", uniqueName_.get()));
// // 
// // 		// Loop over Configurations again, summing into the PartialSet we have just set up
// // 		// We will keep a running total of the weights associated with each Configuration, and re-weight the entire set of partials at the end.
// // 		double totalWeight = 0.0, density = 0.0, volume = 0.0;
// // 		configIterator.restart();
// // 		while (Configuration* cfg = configIterator.iterate())
// // 		{
// // 			// Update fingerprint
// // 			fingerprint += fingerprint.isEmpty() ? CharString("%i", cfg->coordinateIndex()) : CharString("_%i", cfg->coordinateIndex());
// // 
// // 			// Get weighting factor for this Configuration to contribute to the summed partials
// // 			double weight = GenericListHelper<double>::retrieve(moduleData, CharString("Weight_%s", cfg->niceName()), uniqueName_, 1.0);
// // 			totalWeight += weight;
// // 			Messenger::print("Partials: Weight for Configuration '%s' is %f (total weight is now %f).\n", cfg->name(), weight, totalWeight);
// // 
// // 			// Grab partials for Configuration and add into our set
// // 			PartialSet& cfgPartials = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedGR", "Partials");
// // 			unweightedgr.addPartials(cfgPartials, weight);
// // 
// // 			// Sum density weighted by volume, and total volume (both of which are multiplied by the overall weight)
// // 			density += cfg->atomicDensity()*cfg->box()->volume()*weight;
// // 			volume += cfg->box()->volume()*weight;
// // 		}
// // 		density /= volume;
// // 		unweightedgr.setFingerprint(fingerprint);
// // 
// // 		// Now must normalise our partials to the overall weight of the source configurations
// // 		// TODO This will not be correct - need to do proper weighting of configurations, unfolding g(r) normalisation etc.
// // 		unweightedgr.reweightPartials(1.0 / totalWeight);
// // 		if (saveData) if (!MPIRunMaster(procPool, unweightedgr.save())) return false;
// // 
// // 		// Store the blended density of our partials
// // 		GenericListHelper<double>::realise(dissolve.processingModuleData(), "Density", uniqueName_, GenericItem::InRestartFileFlag) = density;
	}

	return true;
}

