/*
	*** RDF Module - Processing
	*** src/modules/rdf/process.cpp
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

#include "modules/rdf/rdf.h"
#include "main/dissolve.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/weights.h"
#include "genericitems/listhelper.h"
#include "math/averaging.h"

// Run main processing
bool RDFModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * Calculate standard partial g(r)
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

	const int averaging = keywords_.asInt("Averaging");
	Averaging::AveragingScheme averagingScheme = Averaging::averagingSchemes().option(keywords_.asString("AveragingScheme"));
	if (averagingScheme == Averaging::nAveragingSchemes) return Messenger::error("Invalid averaging scheme '%s' found.\n", keywords_.asString("AveragingScheme"));
	PairBroadeningFunction& intraBroadening = KeywordListHelper<PairBroadeningFunction>::retrieve(keywords_, "IntraBroadening", PairBroadeningFunction());
	RDFModule::PartialsMethod method = RDFModule::partialsMethod(keywords_.asString("Method"));
	if (method == RDFModule::nPartialsMethods) return Messenger::error("RDF: Invalid calculation method '%s' found.\n", keywords_.asString("Method"));
	const bool allIntra = true;
	const bool internalTest = keywords_.asBool("InternalTest");
	const bool saveData = keywords_.asBool("Save");
	const int smoothing = keywords_.asInt("Smoothing");

	// Print argument/parameter summary
	if (averaging <= 1) Messenger::print("RDF: No averaging of partials will be performed.\n");
	else Messenger::print("RDF: Partials will be averaged over %i sets (scheme = %s).\n", averaging, Averaging::averagingSchemes().option(averagingScheme));
	if (intraBroadening.function() == PairBroadeningFunction::NoFunction) Messenger::print("RDF: No broadening will be applied to intramolecular g(r).");
	else Messenger::print("RDF: Broadening to be applied to intramolecular g(r) is %s.", intraBroadening.summary().get());
	Messenger::print("RDF: Calculation method is '%s'.\n", RDFModule::partialsMethod(method));
	Messenger::print("RDF: Save data is %s.\n", DissolveSys::onOff(saveData));
	Messenger::print("RDF: Degree of smoothing to apply to calculated partial g(r) is %i (%s).\n", smoothing, DissolveSys::onOff(smoothing > 0));
	Messenger::print("\n");


	/*
	 * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or multiple independent Configurations,
	 * we must loop over the specified targetConfigurations_ and calculate the partials for each.
	 */
	RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Calculate unweighted partials for this Configuration (under generic Module name 'Partials', rather than the uniqueName_)
		bool alreadyUpToDate;
		calculateGR(procPool, cfg, method, allIntra, alreadyUpToDate);
		PartialSet& originalgr = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "OriginalGR");

		// Perform averaging of unweighted partials if requested, and if we're not already up-to-date
		if ((averaging > 1) && (!alreadyUpToDate))
		{
			// Store the current fingerprint, since we must ensure we retain it in the averaged T.
			CharString currentFingerprint = originalgr.fingerprint();

			Averaging::average<PartialSet>(cfg->moduleData(), "OriginalGR", "", averaging, averagingScheme);

			// Need to rename data within the contributing datasets to avoid clashes with the averaged data
			for (int n=averaging; n>0; --n)
			{
				if (!cfg->moduleData().contains(CharString("OriginalGR_%i", n))) continue;
				PartialSet& p = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), CharString("OriginalGR_%i", n));
				p.setObjectTags(CharString("%s//OriginalGR", cfg->niceName()), CharString("Avg%i", n));
			}

			// Re-set the object names and fingerprints of the partials
			originalgr.setFingerprint(currentFingerprint);
		}

		// Set names of resources (Data1D) within the PartialSet
		originalgr.setObjectTags(CharString("%s//OriginalGR", cfg->niceName()));

		// Perform internal test of original g(r)?
		if (internalTest)
		{
			// Copy the already-calculated g(r), then calculate a new set using the Test method
			PartialSet referencePartials = originalgr;
			calculateGR(procPool, cfg, RDFModule::TestMethod, allIntra, alreadyUpToDate);
			if (!testReferencePartials(referencePartials, originalgr, 1.0e-6)) return false;
		}

		// Form unweighted g(r) from original g(r), applying any requested smoothing / intramolecular broadening
		PartialSet& unweightedgr = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedGR", NULL, GenericItem::InRestartFileFlag);
		calculateUnweightedGR(procPool, cfg, originalgr, unweightedgr, intraBroadening, smoothing);

		// Set names of resources and filename in Data1D within the PartialSet
		unweightedgr.setObjectTags(CharString("%s//UnweightedGR", cfg->niceName()));
		unweightedgr.setFileNames(cfg->niceName(), "unweighted", "rdf");

		// Save data if requested
		if (saveData && (!MPIRunMaster(procPool, unweightedgr.save()))) return false;
	}

	return true;
}
