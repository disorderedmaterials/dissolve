/*
	*** Partials Module - Method
	*** src/modules/partials/method.cpp
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

#include "modules/partials/partials.h"
#include "main/duq.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/weights.h"
#include "templates/genericlisthelper.h"

// Perform set up tasks for module
bool PartialsModule::setUp(ProcessPool& procPool)
{
	return false;
}

// Execute pre-processing stage
bool PartialsModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool PartialsModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Calculate weighted or unweighted partials and total g(r)/G(r) or S(Q)/F(Q)
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

	GenericList& moduleData = configurationLocal_ ? targetConfigurations_.firstItem()->moduleData() : duq.processingModuleData();

	const bool allIntra = keywords_.asBool("AllIntra");
	const int averaging = keywords_.asInt("Averaging");
	PartialsModule::AveragingScheme averagingScheme = PartialsModule::averagingScheme(keywords_.asString("AveragingScheme"));
	if (averagingScheme == PartialsModule::nAveragingSchemes)
	{
		Messenger::error("Partials: Invalid averaging scheme '%s' found.\n", keywords_.asString("AveragingScheme"));
		return false;
	}
	const bool braggOn = keywords_.asBool("Bragg");
	const BroadeningFunction& qBroadening = KeywordListHelper<BroadeningFunction>::retrieve(keywords_, "QBroadening", BroadeningFunction::unity());
	PartialsModule::PartialsMethod method = PartialsModule::partialsMethod(keywords_.asString("Method"));
	if (method == PartialsModule::nPartialsMethods)
	{
		Messenger::error("Partials: Invalid calculation method '%s' found.\n", keywords_.asString("Method"));
		return false;
	}
	const bool internalTest = keywords_.asBool("InternalTest");
	PartialsModule::NormalisationType normalisation = normalisationType(keywords_.asString("Normalisation"));
	if (normalisation == PartialsModule::nNormalisationTypes)
	{
		Messenger::error("Partials: Invalid normalisation type '%s' found.\n", keywords_.asString("Normalisation"));
	}
	const double qDelta = keywords_.asDouble("QDelta");
	const double qMin = keywords_.asDouble("QMin");
	double qMax = keywords_.asDouble("QMax");
	if (qMax < 0.0) qMax = 30.0;
	const bool saveData = keywords_.asBool("Save");
	const int smoothing = keywords_.asInt("Smoothing");
	const bool sqCalculation = keywords_.asBool("StructureFactor");
	const bool testMode = keywords_.asBool("Test");
	const double testThreshold = keywords_.asDouble("TestThreshold");
	PartialsModule::WeightingType weightsType = PartialsModule::weightingType(keywords_.asString("Weights"));
	if (weightsType == PartialsModule::nWeightingTypes)
	{
		Messenger::error("Partials: Invalid weighting scheme '%s' found.\n", keywords_.asString("Weights"));
		return false;
	}

	// Print argument/parameter summary
	Messenger::print("Partials: Use of all pairs in intramolecular partials is %s.\n", DUQSys::onOff(allIntra));
	if (averaging <= 1) Messenger::print("Partials: No averaging of partials will be performed.\n");
	else Messenger::print("Partials: Partials will be averaged over %i sets (scheme = %s).\n", averaging, PartialsModule::averagingScheme(averagingScheme));
	Messenger::print("Partials: Calculation method is '%s'.\n", PartialsModule::partialsMethod(method));
	Messenger::print("Partials: Degree of smoothing to apply to calculated partial g(r) is %i (%s).\n", smoothing, DUQSys::onOff(smoothing > 0));
	Messenger::print("Partials: Structure factor calculation is %s.\n", DUQSys::onOff(sqCalculation));
	if (sqCalculation)
	{
		Messenger::print("Partials: Calculating S(Q)/F(Q) over %f < Q < %f Angstroms**-1 using step size of %f Angstroms**-1.\n", qMin, qMax, qDelta);
		if (normalisation == PartialsModule::NoNormalisation) Messenger::print("Partials: No normalisation will be applied to total F(Q).\n");
		else if (normalisation == PartialsModule::AverageOfSquaresNormalisation) Messenger::print("Partials: Total F(Q) will be normalised to <b>**2");
		else if (normalisation == PartialsModule::SquareOfAverageNormalisation) Messenger::print("Partials: Total F(Q) will be normalised to <b**2>");
		if (qBroadening.function() == BroadeningFunction::UnityFunction) Messenger::print("Partials: No broadening will be applied to calculated S(Q).");
		else Messenger::print("Partials: Broadening to be applied in calculated S(Q) is %s (%s).", BroadeningFunction::functionType(qBroadening.function()), qBroadening.parameterSummary().get());
		Messenger::print("Partials: Bragg calculation is %s.\n", DUQSys::onOff(braggOn));
		if (braggOn)
		{
		}
	}
	if (weightsType == PartialsModule::NoWeighting) Messenger::print("Partials: No weighted partials will be calculated.\n");
	else Messenger::print("Partials: Weighting scheme to employ is '%s'.\n", PartialsModule::weightingType(weightsType));
	Messenger::print("Partials: Save data is %s.\n", DUQSys::onOff(saveData));

	/*
	 * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or multiple independent Configurations,
	 * we must loop over the specified targetConfigurations_ and calculate the partials for each.
	 */
	Weights combinedWeights;
	AtomTypeList combinedAtomTypes, combinedExchangeableAtoms;
	RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Update total AtomTypeList
		combinedAtomTypes.add(cfg->usedAtomTypesList());

		// Calculate unweighted partials for this Configuration (under generic Module name 'Partials', rather than the uniqueName_)
		bool alreadyUpToDate;
		calculateUnweightedGR(procPool, cfg, method, allIntra, smoothing, alreadyUpToDate);
		PartialSet& unweightedgr = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedGR", "Partials");

		// Set names of resources (XYData) within the PartialSet
		unweightedgr.setResourceNames(CharString("%s//%s//%s", cfg->niceName(), "Partials", "UnweightedGR"));

		// Perform averaging of unweighted partials if requested, and if we're not already up-to-date
		if ((averaging > 1) && (!alreadyUpToDate)) performGRAveraging(cfg->moduleData(), "UnweightedGR", "Partials", averaging, averagingScheme);

		// If we are associated to a local Configuration, copy the partial data over to the processing module list
		if (configurationLocal_) GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "UnweightedGR", uniqueName_) = unweightedgr;

		// Save data if requested
		if (saveData && configurationLocal_ && (!MPIRunMaster(procPool, unweightedgr.save()))) return false;

		// Perform internal test of unweighted g(r)?
		if (internalTest)
		{
			// Copy the already-calculated g(r), then calculate a new set using the Test method
			PartialSet referencePartials = unweightedgr;
			calculateUnweightedGR(procPool, cfg, PartialsModule::TestMethod, allIntra, smoothing, alreadyUpToDate);
			if (!testReferencePartials(referencePartials, unweightedgr, 1.0e-6)) return false;
		}

		// Test unweighted g(r)?
		if (testMode && configurationLocal_)
		{
			Messenger::print("\nTesting calculated unweighted g(r) data against supplied datasets (if any)...\n");
			if (!testReferencePartials(moduleData, unweightedgr, "TestReferenceGR-unweighted", testThreshold)) return false;
		}

		// Calculate S(Q) if requested
		if (sqCalculation)
		{
			if (!calculateUnweightedSQ(procPool, cfg, qMin, qDelta, qMax, cfg->atomicDensity(), duq.windowFunction(), qBroadening, braggOn)) return false;
			PartialSet& unweightedsq = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedSQ", "Partials");

			// Set names of resources (XYData) within the PartialSet
			unweightedsq.setResourceNames(CharString("%s//%s//%s", cfg->niceName(), "Partials", "UnweightedSQ"));

			// Save data if requested
			if (saveData && configurationLocal_ && (!MPIRunMaster(procPool, unweightedsq.save()))) return false;

			// If we are associated to a local Configuration, copy the partial data over to the processing module list
			if (configurationLocal_) GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "UnweightedSQ", uniqueName_) = unweightedsq;

			// Test unweighted S(Q)?
			if (testMode && configurationLocal_)
			{
				Messenger::print("\nTesting calculated unweighted S(Q) data against supplied datasets (if any)...\n");
				if (!testReferencePartials(moduleData, unweightedsq, "TestReferenceSQ-unweighted", testThreshold)) return false;
			}
		}

		// Calculate weighted partials here if requested
		if (weightsType != PartialsModule::NoWeighting)
		{
			// Construct weights matrix based on Isotopologue specifications in some Module (specified by mixSource) and the populations of AtomTypes in the Configuration
			Weights weights;
			ListIterator<SpeciesInfo> speciesInfoIterator(cfg->usedSpecies());
			while (SpeciesInfo* spInfo = speciesInfoIterator.iterate())
			{
				Species* sp = spInfo->species();

				int speciesPopulation = spInfo->population() * cfg->multiplier();

				// Loop over available Isotopologues for Species
				for (Isotopologue* availableIso = sp->isotopologues(); availableIso != NULL; availableIso = availableIso->next)
				{
					// Construct variable name that we expect to find if the tope was used in the Module (variable is defined in the associated Configuration)
					varName.sprintf("Isotopologue/%s/%s", sp->name(), availableIso->name());
					if (moduleData.contains(varName, uniqueName_))
					{
						// This isotopologue is defined as being used, so add it (in the isotopic proportions defined in the Isotopologue) to the Weights.
						double proportion = GenericListHelper<double>::retrieve(moduleData, varName, uniqueName_);
						weights.addIsotopologue(sp, speciesPopulation, availableIso, proportion);
						combinedWeights.addIsotopologue(sp, speciesPopulation, availableIso, proportion);
					}
				}
			}

			// We will complain strongly if a species in the Configuration is not covered by at least one Isotopologue definition
			speciesInfoIterator.restart();
			while (SpeciesInfo* spInfo = speciesInfoIterator.iterate()) if (!weights.hasSpeciesIsotopologueMixture(spInfo->species())) 
			{
				Messenger::error("Isotopologue specification for Species '%s' in Configuration '%s' is missing.\n", spInfo->species()->name(), cfg->name());
				return false;
			}

			// Grab exchangeable atoms list if it is present (in the Module data, rather than the local Configuration data)
			// This is passed to Weights::finalise(), which uses the contained information when finalising its AtomTypeList
			AtomTypeList exchangeableAtoms = GenericListHelper<AtomTypeList>::retrieve(moduleData, "Exchangeable", uniqueName());
			combinedExchangeableAtoms.add(exchangeableAtoms);

			// Finalise, print, and store weights
			Messenger::print("Partials: Isotopologue and isotope composition for Configuration '%s':\n\n", cfg->name());
			weights.finalise(exchangeableAtoms);
			weights.print();
			GenericListHelper<Weights>::realise(cfg->moduleData(), "FullWeights", uniqueName_) = weights;

			// If we are defined in a Configuration, also set the PartialWeights in the Module
			if (configurationLocal_) GenericListHelper<Weights>::realise(duq.processingModuleData(), "FullWeights", uniqueName_) = weights;

			// Calculate weighted partials
			PartialSet& weightedgr = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "WeightedGR", uniqueName_);
			weightedgr.setUp(cfg, cfg->niceName(), "weighted", "rdf", "r, Angstroms");
			calculateWeightedGR(unweightedgr, weightedgr, weights);

			// Set names of resources (XYData) within the PartialSet
			weightedgr.setResourceNames(CharString("%s//%s//%s", cfg->niceName(), "Partials", "WeightedGR"));

			// If we are associated to a local Configuration, copy the partial data over to the processing module list
			if (configurationLocal_) GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "WeightedGR", uniqueName_) = weightedgr;

			// Save data if requested
			if (saveData && configurationLocal_ && (!MPIRunMaster(procPool, weightedgr.save()))) return false;

			// Test weighted g(r)?
			if (testMode && configurationLocal_)
			{
				Messenger::print("\nTesting calculated weighted g(r) data against supplied datasets (if any)...\n");
				if (!testReferencePartials(moduleData, weightedgr, "TestReferenceGR-weighted", testThreshold)) return false;
			}

			if (sqCalculation)
			{
				// Retrieve the unweighted S(Q), and realise the weighted S(Q)
				PartialSet& unweightedsq = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedSQ", "Partials");
				PartialSet& weightedsq = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "WeightedSQ", uniqueName_);

				// Set up weighted S(Q) PartialSet from the weighted G(R) PartialSet, and calculate the weighted S(Q)
				weightedsq.setUpPartials(weightedgr.atomTypes(), cfg->niceName(), "weighted", "sq", "Q, 1/Angstroms");
				calculateWeightedSQ(unweightedsq, weightedsq, weights, normalisation);

				// If we are associated to a local Configuration, copy the partial data over to the processing module list
				if (configurationLocal_) GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "WeightedSQ", uniqueName_) = weightedsq;

				// Save data if requested
				if (saveData && configurationLocal_ && (!MPIRunMaster(procPool, weightedsq.save()))) return false;

				// Test weighted S(Q)?
				if (testMode && configurationLocal_)
				{
					Messenger::print("\nTesting calculated weighted S(Q) data against supplied datasets (if any)...\n");
					if (!testReferencePartials(moduleData, weightedsq, "TestReferenceSQ-weighted", testThreshold)) return false;
				}
			}
		}

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

		// Set up partial set using the AtomTypeList we have just constructed.
		Configuration* refConfig = targetConfigurations_.firstItem();
		PartialSet& unweightedgr = GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "UnweightedGR", uniqueName_);
		unweightedgr.setUp(combinedAtomTypes, refConfig->rdfRange(), refConfig->rdfBinWidth(), uniqueName(), "unweighted", "rdf", "r, Angstroms");

		// Loop over Configurations again, summing into the PartialSet we have just set up
		// We will keep a running total of the weights associated with each Configuration, and re-weight the entire set of partials at the end.
		double totalWeight = 0.0, density = 0.0, volume = 0.0;
		configIterator.restart();
		while (Configuration* cfg = configIterator.iterate())
		{
			// Update fingerprint
			fingerprint += fingerprint.isEmpty() ? CharString("%i", cfg->coordinateIndex()) : CharString("_%i", cfg->coordinateIndex());

			// Get weighting factor for this Configuration to contribute to the summed partials
			double weight = GenericListHelper<double>::retrieve(moduleData, CharString("%s_Weight", cfg->name()), uniqueName_, 1.0);
			totalWeight += weight;
			Messenger::print("Partials: Weight for Configuration '%s' is %f (total weight is now %f).\n", cfg->name(), weight, totalWeight);

			// Grab partials for Configuration and add into our set
			PartialSet& cfgPartials = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedGR", "Partials");
			unweightedgr.addPartials(cfgPartials, weight);

			// Sum density weighted by volume, and total volume (both of which are multiplied by the overall weight)
			density += cfg->atomicDensity()*cfg->box()->volume()*weight;
			volume += cfg->box()->volume()*weight;
		}
		density /= volume;
		unweightedgr.setFingerprint(fingerprint);

		// Now must normalise our partials to the overall weight of the source configurations // TODO This will not be correct...
		unweightedgr.reweightPartials(1.0 / totalWeight);
		if (saveData) if (!MPIRunMaster(procPool, unweightedgr.save())) return false;

		// Store the blended density of our partials
		GenericListHelper<double>::realise(duq.processingModuleData(), "Density", uniqueName_) = density;

		// Test unweighted g(r)?
		if (testMode)
		{
			Messenger::print("\nTesting calculated unweighted g(r) data against supplied datasets (if any)...\n");
			if (!testReferencePartials(moduleData, unweightedgr, "TestReferenceGR-unweighted", testThreshold)) return false;
		}

		// Calculate S(Q) if requested
		if (sqCalculation)
		{
			// Realise a PartialSQ set
			PartialSet& unweightedsq = GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "UnweightedSQ", uniqueName_);
			unweightedsq.setUpPartials(unweightedgr.atomTypes(), uniqueName(), "unweighted", "sq", "Q, 1/Angstroms");

			// Sum in unweighted S(Q) partials
			configIterator.restart();
			while (Configuration* cfg = configIterator.iterate())
			{
				// Get unweighted S(Q) for this Configuration and their overall weight
				PartialSet& cfgunweightedsq = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedSQ", "Partials");
				double weight = GenericListHelper<double>::retrieve(moduleData, CharString("%s_Weight", cfg->name()), uniqueName_, 1.0);

				// Add into our set
				unweightedsq.addPartials(cfgunweightedsq, weight);
			}

			// Now must normalise our partials to the overall weight of the source configurations
			unweightedsq.reweightPartials(1.0 / totalWeight);
			if (saveData && (!MPIRunMaster(procPool, unweightedsq.save()))) return false;

			// Test unweighted S(Q)?
			if (testMode)
			{
				Messenger::print("\nTesting calculated unweighted S(Q) data against supplied datasets (if any)...\n");
				if (!testReferencePartials(moduleData, unweightedsq, "TestReferenceSQ-unweighted", testThreshold)) return false;
			}
		}

		// Calculate weighted Sample partials and total if requested
		if (weightsType != PartialsModule::NoWeighting)
		{
			Messenger::print("Partials: Isotopologue and isotope composition over all Configurations used in '%s':\n\n", uniqueName_.get());
			combinedWeights.finalise(combinedExchangeableAtoms);
			combinedWeights.print();
			GenericListHelper<Weights>::realise(duq.processingModuleData(), "FullWeights", uniqueName_) = combinedWeights;

			PartialSet& weightedgr = GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "WeightedGR", uniqueName_);
			weightedgr.setUp(combinedAtomTypes, refConfig->rdfRange(), refConfig->rdfBinWidth(), uniqueName(), "weighted", "rdf", "r, Angstroms");
			weightedgr.reset();

			// Loop over Configurations, adding in their weighted partial sets as we go
			configIterator.restart();
			while (Configuration* cfg = configIterator.iterate())
			{
				// Grab weighted partials for this Configuration and their associated weight
				PartialSet& cfgweightedgr = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "WeightedGR", uniqueName_);
				double weight = GenericListHelper<double>::retrieve(moduleData, CharString("%s_Weight", cfg->name()), uniqueName_, 1.0);

				weightedgr.addPartials(cfgweightedgr, weight);
			}

			// Re-weight partial set according to totalWeight (calculated earlier)
			weightedgr.reweightPartials(1.0 / totalWeight);
			if (saveData && (!MPIRunMaster(procPool, unweightedgr.save()))) return false;

			// Test weighted g(r)?
			if (testMode)
			{
				Messenger::print("\nTesting calculated weighted g(r) data against supplied datasets (if any)...\n");
				if (!testReferencePartials(moduleData, weightedgr, "TestReferenceGR-weighted", testThreshold)) return false;
			}

			// Calculate S(Q) if requested
			if (sqCalculation)
			{
				// Realise a PartialSQ set
				PartialSet& weightedsq = GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "WeightedSQ", uniqueName_);
				weightedsq.setUpPartials(unweightedgr.atomTypes(), uniqueName(), "weighted", "sq", "Q, 1/Angstroms");

				// Sum in weighted S(Q) partials
				configIterator.restart();
				while (Configuration* cfg = configIterator.iterate())
				{
					// Get unweighted SQ for this Configuration and their overall weight
					PartialSet& cfgweightedsq = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "WeightedSQ", uniqueName_);
					double weight = GenericListHelper<double>::retrieve(moduleData, CharString("%s_Weight", cfg->name()), uniqueName_, 1.0);

					// Add into our set
					weightedsq.addPartials(cfgweightedsq, weight);
				}

				// Now must normalise our partials to the overall weight of the source configurations
				weightedsq.reweightPartials(1.0 / totalWeight);
				if (saveData && (!MPIRunMaster(procPool, weightedsq.save()))) return false;

				// Test weighted S(Q)?
				if (testMode)
				{
					Messenger::print("\nTesting calculated weighted S(Q) data against supplied datasets (if any)...\n");
					if (!testReferencePartials(moduleData, weightedsq, "TestReferenceSQ-weighted", testThreshold)) return false;
				}
			}
		}
	}

	return true;
}

// Execute post-processing stage
bool PartialsModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
