/*
	*** Calibration Module - Functions
	*** src/modules/calibration/functions.cpp
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

#include "modules/calibration/calibration.h"
#include "main/dissolve.h"
#include "modules/rdf/rdf.h"
#include "math/broadeningfunction.h"
#include "math/error.h"
#include "classes/configuration.h"
#include "classes/partialset.h"
#include "genericitems/listhelper.h"

/*
 * CalibrationModuleCostFunctions
 */

// Constructor
CalibrationModuleCostFunctions::CalibrationModuleCostFunctions(Dissolve& dissolve, ProcessPool& procPool, RefList<Module>& intraBroadeningModules, RefDataList<Module,CalibrationModule::IntraBroadeningFitTarget>& intraBroadeningReferences) : dissolve_(dissolve), processPool_(procPool), intraBroadeningModules_(intraBroadeningModules), intraBroadeningReferences_(intraBroadeningReferences)
{
}

// Cost function for intraBroadening minimisation
double CalibrationModuleCostFunctions::intraBroadeningCost(const Array<double>& alpha)
{
	// Store alpha parameters in the PairBroadeningFunction in the associated RDF modules
	int alphaIndex = 0;
	const int nAlpha = alpha.nItems();
	for (Module* rdfModule : intraBroadeningModules_)
	{
		// Retrieve the PairBroadeningFunction - new test values will already have been set (pokeBeforeCost = true)
		PairBroadeningFunction& broadening = rdfModule->keywords().retrieve<PairBroadeningFunction>("IntraBroadening", PairBroadeningFunction());

		// Recalculate the UnweightedGR for all Configurations targeted by the RDFModule
		int smoothing = rdfModule->keywords().asInt("Smoothing");
		for (Configuration* cfg : rdfModule->targetConfigurations())
		{
			const PartialSet& originalGR = GenericListHelper<PartialSet>::value(cfg->moduleData(), "OriginalGR");
			PartialSet& unweightedGR = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedGR");
			RDFModule::calculateUnweightedGR(processPool_, cfg, originalGR, unweightedGR, broadening, smoothing);
		}
	}

	// Go over NeutronSQ Modules, run the processing, and sum errors in an ReferenceData we have
	double totalError = 0.0;
	RefDataListIterator<Module,CalibrationModule::IntraBroadeningFitTarget> neutronModuleIterator(intraBroadeningReferences_);
	while (Module* module = neutronModuleIterator.iterate())
	{
		// Check for ReferenceData first
		if (!dissolve_.processingModuleData().contains("ReferenceData", module->uniqueName())) continue;

		// Make sure the structure factors will be updated by the NeutronSQ module - set flag in the target Configurations
		for (Configuration* cfg : module->targetConfigurations()) GenericListHelper<bool>::realise(cfg->moduleData(), "_ForceNeutronSQ") = true;

		// Run the NeutronSQModule (quietly)
		Messenger::mute();
		module->executeProcessing(dissolve_, processPool_);
		Messenger::unMute();

		// Grab target data and compare 
		CalibrationModule::IntraBroadeningFitTarget target = neutronModuleIterator.currentData();
		if ((target == CalibrationModule::IntraBroadeningTargetBoth) || (target == CalibrationModule::IntraBroadeningTargetSQ))
		{
			// Grab WeightedSQ and ReferenceData and compare
			const PartialSet& weightedSQ = GenericListHelper<PartialSet>::value(dissolve_.processingModuleData(), "WeightedSQ", module->uniqueName());
			const Data1D& referenceData = GenericListHelper<Data1D>::value(dissolve_.processingModuleData(), "ReferenceData", module->uniqueName());
			totalError += Error::rmse(weightedSQ.constTotal(), referenceData, true);
		}
		if ((target == CalibrationModule::IntraBroadeningTargetBoth) || (target == CalibrationModule::IntraBroadeningTargetGR))
		{
			// Grab WeightedGR and ReferenceDataFT and compare
			const PartialSet& weightedGR = GenericListHelper<PartialSet>::value(dissolve_.processingModuleData(), "WeightedGR", module->uniqueName());
			const Data1D& referenceDataFT = GenericListHelper<Data1D>::value(dissolve_.processingModuleData(), "ReferenceDataFT", module->uniqueName());
			totalError += Error::rmse(weightedGR.constTotal(), referenceDataFT, true);
		}
	}

	return totalError;
}
