/*
	*** Calibration Module - Functions
	*** src/modules/calibration/functions.cpp
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

#include "modules/calibration/calibration.h"
#include "main/dissolve.h"
#include "modules/rdf/rdf.h"
#include "math/broadeningfunction.h"
#include "classes/configuration.h"
#include "classes/partialset.h"
#include "templates/genericlisthelper.h"

/*
 * CalibrationModuleCostFunctions
 */

// Constructor
CalibrationModuleCostFunctions::CalibrationModuleCostFunctions(Dissolve& dissolve, ProcessPool& procPool, RefList<Module,bool>& intraBroadeningModules, RefList<Module,CalibrationModule::IntraBroadeningFitTarget>& intraBroadeningReferences) : dissolve_(dissolve), processPool_(procPool), intraBroadeningModules_(intraBroadeningModules), intraBroadeningReferences_(intraBroadeningReferences)
{
}

// Cost function for intraBroadening minimisation
double CalibrationModuleCostFunctions::intraBroadeningCost(const Array<double>& alpha)
{
	// Store alpha parameters in the PairBroadeningFunction in the associated RDF modules
	int alphaIndex = 0;
	const int nAlpha = alpha.nItems();
	RefListIterator<Module,bool> rdfModuleIterator(intraBroadeningModules_);
	while (Module* rdfModule = rdfModuleIterator.iterate())
	{
		// Retrieve the PairBroadeningFunction
		PairBroadeningFunction& broadening = KeywordListHelper<PairBroadeningFunction>::retrieve(rdfModule->keywords(), "IntraBroadening", PairBroadeningFunction());

		// Set its parameters from the alpha array
		for (int n=0; n<broadening.nParameters(); ++n)
		{
			if (alphaIndex >= nAlpha)
			{
				Messenger::error("Parameters required by BroadeningFunctions exceeds number available in alpha array.\n");
				return 100.0;
			}
			broadening.parameters()[n] = alpha.constAt(alphaIndex);
			++alphaIndex;
		}

		// Recalculate the UnweightedGR for all Configurations targeted by the RDFModule
		int smoothing = rdfModule->keywords().asInt("Smoothing");
		RefListIterator<Configuration,bool> configIterator(rdfModule->targetConfigurations());
		while (Configuration* cfg = configIterator.iterate())
		{
			const PartialSet& originalGR = GenericListHelper<PartialSet>::value(cfg->moduleData(), "OriginalGR");
			PartialSet& unweightedGR = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedGR");
			RDFModule::calculateUnweightedGR(originalGR, unweightedGR, broadening, smoothing);
		}
	}

	// Go over NeutronSQ Modules, run the processing, and sum errors in an ReferenceData we have
	double totalError = 0.0;
	RefListIterator<Module,CalibrationModule::IntraBroadeningFitTarget> neutronModuleIterator(intraBroadeningReferences_);
	while (Module* module = neutronModuleIterator.iterate())
	{
		// Check for ReferenceData first
		if (!dissolve_.processingModuleData().contains("ReferenceData", module->uniqueName())) continue;

		// Make sure the structure factors will be updated by the NeutronSQ module - set flag in the target Configurations
		RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
		while (Configuration* cfg = configIterator.iterate()) GenericListHelper<bool>::realise(cfg->moduleData(), "_ForceNeutronSQ") = true;

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
			const XYData& referenceData = GenericListHelper<XYData>::value(dissolve_.processingModuleData(), "ReferenceData", module->uniqueName());
			totalError += weightedSQ.constTotal().error(referenceData);
		}
		if ((target == CalibrationModule::IntraBroadeningTargetBoth) || (target == CalibrationModule::IntraBroadeningTargetGR))
		{
			// Grab WeightedGR and ReferenceDataFT and compare
			const PartialSet& weightedGR = GenericListHelper<PartialSet>::value(dissolve_.processingModuleData(), "WeightedGR", module->uniqueName());
			const XYData& referenceDataFT = GenericListHelper<XYData>::value(dissolve_.processingModuleData(), "ReferenceDataFT", module->uniqueName());
			totalError += weightedGR.constTotal().error(referenceDataFT);
		}
	}

	return totalError;
}
