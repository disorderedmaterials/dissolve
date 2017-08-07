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

// Perform setup tasks for module
bool PartialsModule::setup(ProcessPool& procPool)
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
	CharString varName;

	GenericList& moduleData = configurationLocal_ ? targetConfigurations_.firstItem()->moduleData() : duq.processingModuleData();
	const bool allIntra = GenericListHelper<bool>::retrieve(moduleData, "AllIntra", uniqueName(), options_.valueAsBool("AllIntra"));
	const bool braggOn = GenericListHelper<bool>::retrieve(moduleData, "Bragg", uniqueName(), options_.valueAsBool("Bragg"));
	const double braggQDepBroadening = GenericListHelper<double>::retrieve(moduleData, "BraggQDepBroadening", uniqueName(), options_.valueAsDouble("BraggQDepBroadening"));
	const double braggQIndepBroadening = GenericListHelper<double>::retrieve(moduleData, "BraggQIndepBroadening", uniqueName(), options_.valueAsDouble("BraggQIndepBroadening"));
	const double qDepBroadening = GenericListHelper<double>::retrieve(moduleData, "QDepBroadening", uniqueName(), options_.valueAsDouble("QDepBroadening"));
	const double qIndepBroadening = GenericListHelper<double>::retrieve(moduleData, "QIndepBroadening", uniqueName(), options_.valueAsDouble("QIndepBroadening"));
	PartialsModule::PartialsMethod method = PartialsModule::partialsMethod(GenericListHelper<CharString>::retrieve(moduleData, "Method", uniqueName_, options_.valueAsString("Method")));
	if (method == PartialsModule::nPartialsMethods)
	{
		Messenger::error("Partials: Invalid calculation method '%s' found.\n", GenericListHelper<CharString>::retrieve(moduleData, "Method", uniqueName_, options_.valueAsString("Method")).get());
		return false;
	}
	const bool internalTest = GenericListHelper<bool>::retrieve(moduleData, "InternalTest", uniqueName(), options_.valueAsBool("InternalTest"));
	const bool normaliseToAvSq = GenericListHelper<bool>::retrieve(moduleData, "NormaliseToAvSq", uniqueName(), options_.valueAsBool("NormaliseToAvSq"));
	const bool normaliseToSqAv = GenericListHelper<bool>::retrieve(moduleData, "NormaliseToSqAv", uniqueName(), options_.valueAsBool("NormaliseToSqAv"));
	const double qDelta = GenericListHelper<double>::retrieve(moduleData, "QDelta", uniqueName(), options_.valueAsDouble("QDelta"));
	const double qMin = GenericListHelper<double>::retrieve(moduleData, "QMin", uniqueName(), options_.valueAsDouble("QMin"));
	double qMax = GenericListHelper<double>::retrieve(moduleData, "QMax", uniqueName(), options_.valueAsDouble("QMax"));
	if (qMax < 0.0) qMax = 30.0;
	const bool saveData = GenericListHelper<bool>::retrieve(moduleData, "Save", uniqueName(), options_.valueAsBool("Save"));
	const int smoothing = GenericListHelper<int>::retrieve(moduleData, "Smoothing", uniqueName(), options_.valueAsInt("Smoothing"));
	const bool sqCalculation = GenericListHelper<bool>::retrieve(moduleData, "StructureFactor", uniqueName(), options_.valueAsBool("StructureFactor"));
	const bool testMode = GenericListHelper<bool>::retrieve(moduleData, "Test", uniqueName(), options_.valueAsBool("Test"));
	const double testThreshold = GenericListHelper<double>::retrieve(moduleData, "TestThreshold", uniqueName(), options_.valueAsDouble("TestThreshold"));
	PartialsModule::WeightingType weightsType = PartialsModule::weightingType(GenericListHelper<CharString>::retrieve(moduleData, "Weights", uniqueName_, options_.valueAsString("Weights")));
	if (weightsType == PartialsModule::nWeightingTypes)
	{
		Messenger::error("Partials: Invalid weighting scheme '%s' found.\n", GenericListHelper<CharString>::retrieve(moduleData, "Weights", uniqueName_, options_.valueAsString("Weights")).get());
		return false;
	}

	// Print argument/parameter summary
	Messenger::print("Partails: Use of all pairs in intramolecular partials is %s.\n", DUQSys::onOff(allIntra));
	Messenger::print("Partials: Calculation method for partials is '%s'.\n", PartialsModule::partialsMethod(method));
	Messenger::print("Partials: Degree of smoothing to apply to calculated partial g(r) is %i (%s).\n", smoothing, DUQSys::onOff(smoothing > 0));
	Messenger::print("Partials: Structure factor calculation is %s.\n", DUQSys::onOff(sqCalculation));
	if (sqCalculation)
	{
		Messenger::print("Partials: Calculating S(Q)/F(Q) over %f < Q < %f Angstroms**-1 using step size of %f Angstroms**-1.\n", qMin, qMax, qDelta);
		Messenger::print("Partials: Bragg calculation is %s.\n", DUQSys::onOff(braggOn));
		Messenger::print("Partials: Q-dependent FWHM broadening to use is %f, Q-independent FWHM broadening to use is %f.\n", qDepBroadening, qIndepBroadening);
		if (braggOn)
		{
			Messenger::print("Partials: Bragg Q-dependent FWHM broadening to use is %f, Q-independent FWHM broadening to use is %f.\n", braggQDepBroadening, braggQIndepBroadening);
		}
	}
	Messenger::print("Partials: Weighting scheme to employ is '%s'.\n", PartialsModule::weightingType(weightsType));
	Messenger::print("Partials: Save data is %s.\n", DUQSys::onOff(saveData));

	/*
	 * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or multiple independent Configurations,
	 * we must loop over the specified targetConfigurations_ and calculate the partials for each.
	 */
	RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Calculate unweighted partials for this Configuration (under generic Module name 'Partials', rather than the uniqueName_)
		calculateUnweightedGR(procPool, cfg, method, allIntra, smoothing);
		PartialSet& unweightedgr = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedGR", "Partials");
		if (saveData && configurationLocal_ && (!MPIRunMaster(procPool, unweightedgr.save()))) return false;

		// Perform internal test of unweighted g(r)?
		if (internalTest)
		{
			// Copy the already-calculated g(r), then calculate a new set using the Test method
			PartialSet referencePartials = unweightedgr;
			calculateUnweightedGR(procPool, cfg, PartialsModule::TestMethod, allIntra, smoothing);
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
			calculateUnweightedSQ(procPool, cfg, qMin, qDelta, qMax, cfg->atomicDensity(), duq.windowFunction(), qDepBroadening, qIndepBroadening, braggOn, braggQDepBroadening, braggQIndepBroadening);
			PartialSet& unweightedsq = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedSQ", "Partials");
			if (saveData && configurationLocal_ && (!MPIRunMaster(procPool, unweightedsq.save()))) return false;

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
			RefListIterator<Species,double> speciesIterator(cfg->usedSpecies());
			while (Species* sp = speciesIterator.iterate())
			{
				int speciesPopulation = speciesIterator.currentData() * cfg->multiplier();

				// Loop over available Isotopologues for Species
				for (Isotopologue* availableIso = sp->isotopologues(); availableIso != NULL; availableIso = availableIso->next)
				{
					// Construct variable name that we expect to find if the tope was used in the Module (variable is defined in the associated Configuration)
					varName.sprintf("Isotopologue/%s/%s", sp->name(), availableIso->name());
					if (moduleData.contains(varName, uniqueName_))
					{
						// This isotopologue is defined as being used, so add it (in the isotopic proportions defined in the Isotopologue) to the Weights.
						weights.addIsotopologue(sp, speciesPopulation, availableIso, GenericListHelper<double>::retrieve(moduleData, varName, uniqueName_));
					}
				}
			}

			// We will complain strongly if a species in the Configuration is not covered by at least one Isotopologue definition
			speciesIterator.restart();
			while (Species* sp = speciesIterator.iterate()) if (!weights.hasSpeciesIsotopologueMixture(sp)) 
			{
				Messenger::error("Isotopologue specification for Species '%s' in Configuration '%s' is missing.\n", sp->name(), cfg->name());
				return false;
			}

			// Grab exchangeable atoms list if it is present (in the Module data, rather than the local Configuration data)
			// This is passed to Weights::finalise(), which uses the contained information when finalising its AtomTypeList
			AtomTypeList exchangeableAtoms = GenericListHelper<AtomTypeList>::retrieve(moduleData, "Exchangeable", uniqueName());

			// Finalise, print, and store weights
			Messenger::print("Partials: Isotopologue and isotope composition for Configuration '%s':\n\n", cfg->name());
			weights.finalise(exchangeableAtoms);
			weights.print();
			GenericListHelper< Array2D<double> >::realise(cfg->moduleData(), "PartialWeights", uniqueName_) = weights.fullWeightsMatrix();

			// Calculate weighted partials
			PartialSet& weightedgr = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "WeightedGR", uniqueName_);
			PartialSet& weightedsq = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "WeightedSQ", uniqueName_);
			weightedgr.setup(cfg, cfg->niceName(), "weighted", "rdf", "r, Angstroms");
			calculateWeightedGR(unweightedgr, weightedgr, weights);

			// Test weighted g(r)?
			if (testMode && configurationLocal_)
			{
				Messenger::print("\nTesting calculated weighted g(r) data against supplied datasets (if any)...\n");
				if (!testReferencePartials(moduleData, weightedgr, "TestReferenceGR-weighted", testThreshold)) return false;
			}

			if (sqCalculation)
			{
				PartialSet& unweightedsq = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedSQ", "Partials");
				weightedsq.setup(weightedgr.atomTypes(), cfg->niceName(), "weighted", "sq", "Q, 1/Angstroms");
				calculateWeightedSQ(unweightedsq, weightedsq, weights);

				// Test weighted S(Q)?
				if (testMode && configurationLocal_)
				{
					Messenger::print("\nTesting calculated weighted S(Q) data against supplied datasets (if any)...\n");
					if (!testReferencePartials(moduleData, weightedsq, "TestReferenceSQ-weighted", testThreshold)) return false;
				}
			}

			if (saveData && configurationLocal_)
			{
				if (!MPIRunMaster(procPool, weightedgr.save())) return false;
				if (sqCalculation && (!MPIRunMaster(procPool, weightedsq.save()))) return false;
			}

		}

	}

	// If we are a main processing task, construct the weighted sum of Configuration
	if (!configurationLocal_)
	{
		// Assemble partials from all target Configurations specified, weighting them accordingly
		CharString varName;
		AtomTypeList atomTypes;

		// Loop over Configurations, creating a list of unique AtomTypes encountered over all.
		RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
		while (Configuration* cfg = configIterator.iterate())
		{
			// Add any missing atom types into our local list
			atomTypes.add(cfg->usedAtomTypesList());
		}
		atomTypes.finalise();
		atomTypes.print();

		// Setup partial set using the AtomTypeList we have just constructed.
		Configuration* refConfig = targetConfigurations_.firstItem();
		PartialSet& unweightedgr = GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "UnweightedGR", "Partials");
		unweightedgr.setup(atomTypes, refConfig->rdfRange(), refConfig->rdfBinWidth(), uniqueName(), "unweighted", "rdf", "r, Angstroms");

		// Loop over Configurations again, summing into the PartialSet we have just set up
		// We will keep a running total of the weights associated with each Configuration, and re-weight the entire set of partials at the end.
		double totalWeight = 0.0, density = 0.0, volume = 0.0;
		configIterator.restart();
		while (Configuration* cfg = configIterator.iterate())
		{
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

		// Now must normalise our partials to the overall weight of the source configurations
		unweightedgr.reweightPartials(1.0 / totalWeight);
		if (saveData) if (!MPIRunMaster(procPool, unweightedgr.save())) return false;

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
			PartialSet& unweightedsq = GenericListHelper<PartialSet>::realise(moduleData, "UnweightedSQ", uniqueName_);
			unweightedsq.setup(unweightedgr.atomTypes(), uniqueName(), "unweighted", "sq", "Q, 1/Angstroms");

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
			PartialSet& weightedgr = GenericListHelper<PartialSet>::realise(moduleData, "WeightedGR", uniqueName_);
			weightedgr.setup(atomTypes, refConfig->rdfRange(), refConfig->rdfBinWidth(), uniqueName(), "weighted", "rdf", "r, Angstroms");
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
				PartialSet& weightedsq = GenericListHelper<PartialSet>::realise(moduleData, "WeightedSQ", uniqueName_);
				weightedsq.setup(unweightedgr.atomTypes(), uniqueName(), "weighted", "sq", "Q, 1/Angstroms");

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

