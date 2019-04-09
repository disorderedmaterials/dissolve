/*
	*** Neutron SQ Module - Processing
	*** src/modules/neutronsq/process.cpp
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

#include "modules/neutronsq/neutronsq.h"
#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/weights.h"
#include "math/filters.h"
#include "math/ft.h"
#include "modules/import/import.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"
#include "templates/genericlisthelper.h"

// Run set-up stage
bool NeutronSQModule::setUp(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * Load and set up reference data (if a file/format was given)
	 */
	if (referenceFQ_.hasValidFileAndFormat())
	{
		// Load the data
		Data1D referenceData;
		LineParser parser(&procPool);
		if (!parser.openInput(referenceFQ_.filename())) return 0;
		if (!ImportModule::readData1D(referenceFQ_.data1DFormat(), parser, referenceData))
		{
			Messenger::error("Failed to load reference data '%s'.\n", referenceFQ_.filename());
			return false;
		}

		// Truncate data beyond QMax
		const double qMax = keywords_.asDouble("QMax") < 0.0 ? 30.0 : keywords_.asDouble("QMax");
		if (referenceData.constXAxis().lastValue() < qMax) Messenger::warn("Qmax limit of %e Angstroms**-1 for calculated NeutronSQ (%s) is beyond limit of reference data (Qmax = %e Angstroms**-1).\n", qMax, uniqueName(), referenceData.constXAxis().lastValue());
		else while (referenceData.constXAxis().lastValue() > qMax) referenceData.removeLastPoint();

		// Subtract average level from data?
		double removeAverage = keywords_.asDouble("ReferenceRemoveAverage");
		if (removeAverage >= 0.0)
		{
			double level = Filters::subtractAverage(referenceData, removeAverage);
			Messenger::print("NeutronSQ: Removed average level of %f from reference data, forming average over x >= %f.\n", level, removeAverage);
		}

		// Remove normalisation factor from data
		NeutronSQModule::NormalisationType normType = NeutronSQModule::normalisationType(keywords_.asString("ReferenceNormalisation"));
		if (normType != NeutronSQModule::NoNormalisation)
		{
			// We need the summed Weights in order to do the normalisation
			Weights summedWeights;
			if (!calculateSummedWeights(summedWeights))
			{
				Messenger::error("Couldn't get summed Weights for reference data in NeutronSQ module '%s', and so can't perform requested normalisation.\n", uniqueName());
				return false;
			}

			// Remove normalisation of data
			if (normType == NeutronSQModule::AverageOfSquaresNormalisation)
			{
				referenceData.values() *= summedWeights.boundCoherentAverageOfSquares();
				Messenger::print("NeutronSQ: Removed <b>**2 normalisation from reference data ('%s'), factor = %f.\n", uniqueName(), summedWeights.boundCoherentAverageOfSquares());
			}
			else if (normType == NeutronSQModule::SquareOfAverageNormalisation)
			{
				referenceData.values() *= summedWeights.boundCoherentSquareOfAverage();
				Messenger::print("NeutronSQ: Removed <b**2> normalisation from reference data ('%s'), factor = %f.\n", uniqueName(), summedWeights.boundCoherentSquareOfAverage());
			}
		}

		// Store the reference data in processing
		referenceData.setName(uniqueName());
		Data1D& storedData = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), "ReferenceData", uniqueName(), GenericItem::InRestartFileFlag);
		storedData.setObjectTag(CharString("%s//ReferenceData", uniqueName()));
		storedData = referenceData;

		// Calculate and store the FT of the reference data in processing
		referenceData.setName(uniqueName());
		Data1D& storedDataFT = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), "ReferenceDataFT", uniqueName(), GenericItem::InRestartFileFlag);
		storedDataFT.setObjectTag(CharString("%s//ReferenceDataFT", uniqueName()));
		storedDataFT = referenceData;
		Fourier::sineFT(storedDataFT, 1.0 / (2.0 * PI * PI * RDFModule::summedRho(this, dissolve.processingModuleData())), 0.0, 0.05, 30.0, WindowFunction(WindowFunction::Lorch0Window));

		// Save data?
		if (keywords_.asBool("SaveReferenceData"))
		{
			if (!storedData.save(CharString("%s-ReferenceData.q", uniqueName()))) return false;
			if (!storedDataFT.save(CharString("%s-ReferenceData.r", uniqueName()))) return false;
		}
	}

	return true;
}

// Run main processing
bool NeutronSQModule::process(Dissolve& dissolve, ProcessPool& procPool)
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

	NeutronSQModule::NormalisationType normalisation = normalisationType(keywords_.asString("Normalisation"));
	if (normalisation == NeutronSQModule::nNormalisationTypes) return Messenger::error("NeutronSQ: Invalid normalisation type '%s' found.\n", keywords_.asString("Normalisation"));
	const BroadeningFunction& qBroadening = KeywordListHelper<BroadeningFunction>::retrieve(keywords_, "QBroadening", BroadeningFunction());
	const double qDelta = keywords_.asDouble("QDelta");
	const double qMin = keywords_.asDouble("QMin");
	double qMax = keywords_.asDouble("QMax");
	if (qMax < 0.0) qMax = 30.0;
	const bool saveUnweighted = keywords_.asBool("SaveUnweighted");
	const bool saveWeighted = keywords_.asBool("SaveWeighted");
	const WindowFunction& windowFunction = KeywordListHelper<WindowFunction>::retrieve(keywords_, "WindowFunction", WindowFunction());

	// Print argument/parameter summary
	Messenger::print("NeutronSQ: Calculating S(Q)/F(Q) over %f < Q < %f Angstroms**-1 using step size of %f Angstroms**-1.\n", qMin, qMax, qDelta);
	if (windowFunction.function() == WindowFunction::NoWindow) Messenger::print("NeutronSQ: No window function will be applied in Fourier transforms of g(r) to S(Q).");
	else Messenger::print("NeutronSQ: Window function to be applied in Fourier transforms is %s (%s).", WindowFunction::functionType(windowFunction.function()), windowFunction.parameterSummary().get());
	if (normalisation == NeutronSQModule::NoNormalisation) Messenger::print("NeutronSQ: No normalisation will be applied to total F(Q).\n");
	else if (normalisation == NeutronSQModule::AverageOfSquaresNormalisation) Messenger::print("NeutronSQ: Total F(Q) will be normalised to <b>**2");
	else if (normalisation == NeutronSQModule::SquareOfAverageNormalisation) Messenger::print("NeutronSQ: Total F(Q) will be normalised to <b**2>");
	if (qBroadening.function() == BroadeningFunction::NoFunction) Messenger::print("NeutronSQ: No broadening will be applied to calculated S(Q).");
	else Messenger::print("NeutronSQ: Broadening to be applied in calculated S(Q) is %s (%s).", BroadeningFunction::functionType(qBroadening.function()), qBroadening.parameterSummary().get());
	if (saveUnweighted) Messenger::print("NeutronSQ: Unweighted partials and totals will be saved.\n");
	if (saveWeighted) Messenger::print("NeutronSQ: Weighted partials and totals will be saved.\n");
	Messenger::print("\n");


	/*
	 * Loop over target Configurations and Fourier transform their UnweightedGR into the corresponding UnweightedSQ.
	 */

	bool created;

	RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Get unweighted g(r) for this Configuration - we don't supply a specific Module prefix, since the unweighted g(r) may come from one of many RDF-type modules
		if (!cfg->moduleData().contains("UnweightedGR")) return Messenger::error("Couldn't locate UnweightedGR for Configuration '%s'.\n", cfg->name());
		const PartialSet& unweightedgr = GenericListHelper<PartialSet>::value(cfg->moduleData(), "UnweightedGR");

		// Does a PartialSet for the unweighted S(Q) already exist for this Configuration?
		PartialSet& unweightedsq = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedSQ", NULL, GenericItem::InRestartFileFlag, &created);
		if (created) unweightedsq.setUpPartials(unweightedgr.atomTypes(), CharString("%s-%s", cfg->niceName(), uniqueName()), "unweighted", "sq", "Q, 1/Angstroms");

		// Is the PartialSet already up-to-date? Do we force its calculation anyway?
		bool& forceCalculation = GenericListHelper<bool>::retrieve(cfg->moduleData(), "_ForceNeutronSQ", NULL, false);
		if ((!forceCalculation) && DissolveSys::sameString(unweightedsq.fingerprint(), CharString("%i", cfg->moduleData().version("UnweightedGR"))))
		{
			Messenger::print("Unweighted partial S(Q) are up-to-date for Configuration '%s'.\n", cfg->name());
			continue;
		}
		forceCalculation = false;

		// Transform g(r) into S(Q)
		if (!SQModule::calculateUnweightedSQ(procPool, cfg, unweightedgr, unweightedsq, qMin, qDelta, qMax, cfg->atomicDensity(), windowFunction, qBroadening)) return false;

		// Set names of resources (Data1D) within the PartialSet, and tag it with the fingerprint from the source unweighted g(r)
		unweightedsq.setObjectTags(CharString("%s//%s//%s", cfg->niceName(), "NeutronSQ", "UnweightedSQ"));
		unweightedsq.setFingerprint(CharString("%i", cfg->moduleData().version("UnweightedGR")));

		// Save data if requested
		if (saveUnweighted && (!MPIRunMaster(procPool, unweightedsq.save()))) return false;

		// Construct weights matrix based on Isotopologue specifications in some Module (specified by mixSource) and the populations of AtomTypes in the Configuration
		Weights weights;
		ListIterator<IsotopologueReference> refIterator(isotopologues_);
		while (IsotopologueReference* ref = refIterator.iterate())
		{
			// If this reference does not target the current Configuration we are considering, continue the loop
			if (ref->configuration() != cfg) continue;

			// Find the referenced Species in our SpeciesInfo list
			SpeciesInfo* spInfo = cfg->usedSpeciesInfo(ref->species());
			int speciesPopulation = spInfo->population() * cfg->multiplier();

			// Add the isotopologue, in the isotopic proportions defined in the Isotopologue, to the weights.
			weights.addIsotopologue(ref->species(), speciesPopulation, ref->isotopologue(), ref->weight());
		}

		// We will complain strongly if a species in the Configuration is not covered by at least one Isotopologue definition
		ListIterator<SpeciesInfo> speciesInfoIterator(cfg->usedSpecies());
		while (SpeciesInfo* spInfo = speciesInfoIterator.iterate()) if (!weights.hasSpeciesIsotopologueMixture(spInfo->species())) 
		{
			Messenger::error("Isotopologue specification for Species '%s' in Configuration '%s' is missing.\n", spInfo->species()->name(), cfg->name());
			return false;
		}

		// Create, print, and store weights
		Messenger::print("Isotopologue and isotope composition for Configuration '%s':\n\n", cfg->name());
		weights.createFromIsotopologues(exchangeableTypes_);
		weights.print();

		// Does a PartialSet for the unweighted S(Q) already exist for this Configuration?
		PartialSet& weightedsq = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "WeightedSQ", NULL, GenericItem::InRestartFileFlag, &created);
		if (created) weightedsq.setUpPartials(unweightedsq.atomTypes(), CharString("%s-%s", cfg->niceName(), uniqueName()), "weighted", "sq", "Q, 1/Angstroms");

		// Calculate weighted S(Q)
		calculateWeightedSQ(unweightedsq, weightedsq, weights, normalisation);

		// Set names of resources (Data1D) within the PartialSet
		weightedsq.setObjectTags(CharString("%s//%s//%s", cfg->niceName(), uniqueName_.get(), "WeightedSQ"));

		// Save data if requested
		if (saveWeighted && (!MPIRunMaster(procPool, weightedsq.save()))) return false;
	}

	/*
	 * Construct the weighted sum of Configuration partials and store in the processing module data list
	 */

	// Create/retrieve PartialSet for summed partial S(Q)
	PartialSet& summedUnweightedSQ = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "UnweightedSQ", uniqueName_, GenericItem::InRestartFileFlag);

	// Sum the partials from the associated Configurations
	if (!SQModule::sumUnweightedSQ(procPool, this, dissolve.processingModuleData(), summedUnweightedSQ)) return false;

	// Save data if requested
	if (saveUnweighted && (!MPIRunMaster(procPool, summedUnweightedSQ.save()))) return false;

	// Create/retrieve PartialSet for summed partial S(Q)
	PartialSet& summedWeightedSQ = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "WeightedSQ", uniqueName_, GenericItem::InRestartFileFlag);
	summedWeightedSQ = summedUnweightedSQ;
	summedWeightedSQ.setFileNames(uniqueName_, "weighted", "sq");
	summedWeightedSQ.setObjectTags(CharString("%s//%s", uniqueName_.get(), "WeightedSQ"));

	// Calculate weighted S(Q)
	Messenger::print("Isotopologue and isotope composition over all Configurations used in '%s':\n\n", uniqueName_.get());
	Weights summedWeights;
	if (!calculateSummedWeights(summedWeights)) return false;
	summedWeights.print();
	GenericListHelper<Weights>::realise(dissolve.processingModuleData(), "FullWeights", uniqueName_, GenericItem::InRestartFileFlag) = summedWeights;
	calculateWeightedSQ(summedUnweightedSQ, summedWeightedSQ, summedWeights, normalisation);

	// Create/retrieve PartialSet for summed unweighted g(r)
	PartialSet& summedUnweightedGR = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "UnweightedGR", uniqueName_, GenericItem::InRestartFileFlag);

	// Sum the partials from the associated Configurations
	if (!RDFModule::sumUnweightedGR(procPool, this, dissolve.processingModuleData(), summedUnweightedGR)) return false;

	// Create/retrieve PartialSet for summed weighted g(r)
	PartialSet& summedWeightedGR = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "WeightedGR", uniqueName_, GenericItem::InRestartFileFlag, &created);
	if (created) summedWeightedGR.setUpPartials(summedUnweightedSQ.atomTypes(), uniqueName_, "weighted", "gr", "r, Angstroms");
	summedWeightedGR.setObjectTags(CharString("%s//%s", uniqueName_.get(), "WeightedGR"));

	// Calculate weighted g(r)
	calculateWeightedGR(summedUnweightedGR, summedWeightedGR, summedWeights, normalisation);

	// Save data if requested
	if (saveWeighted && (!MPIRunMaster(procPool, summedWeightedSQ.save()))) return false;

	return true;
}
