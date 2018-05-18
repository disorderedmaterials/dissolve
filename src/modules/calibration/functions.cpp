/*
	*** Calibration Module - Functions
	*** src/modules/calibration/functions.cpp
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

#include "modules/calibration/calibration.h"
#include "main/duq.h"
#include "modules/rdf/rdf.h"
#include "math/broadeningfunction.h"
#include "classes/configuration.h"
#include "classes/partialset.h"
#include "templates/genericlisthelper.h"

/*
 * CalibrationModuleCostFunctions
 */

// Constructor
CalibrationModuleCostFunctions::CalibrationModuleCostFunctions(DUQ& duq, ProcessPool& procPool, RefList<Module,bool>& intraBroadeningModules, RefList<Module,bool>& intraBroadeningReferences) : duq_(duq), processPool_(procPool), intraBroadeningModules_(intraBroadeningModules), intraBroadeningReferences_(intraBroadeningReferences)
{
}

// Cost function for intraBroadening minimisation
double CalibrationModuleCostFunctions::intraBroadeningCost(double* alpha, int nAlpha)
{
	// Store alpha parameters in the BroadeningFunction in the associated RDF modules
	int alphaIndex = 0;
	RefListIterator<Module,bool> rdfModuleIterator(intraBroadeningModules_);
	while (Module* rdfModule = rdfModuleIterator.iterate())
	{
		// Retrieve the BroadeningFunction
		BroadeningFunction& broadening = KeywordListHelper<BroadeningFunction>::retrieve(rdfModule->keywords(), "IntraBroadening", BroadeningFunction());

		// Set its parameters from the alpha array
		for (int n=0; n<broadening.nParameters(); ++n)
		{
			if (alphaIndex >= nAlpha)
			{
				Messenger::error("Parameters required by BroadeningFunctions exceeds number available in alpha array.\n");
				return 100.0;
			}
			broadening.parameters()[n] = alpha[alphaIndex];
			++alphaIndex;
		}

		// Need to update any dependent values
		broadening.setUpDependentParameters();

		// Recalculate the UnweightedGR for all Configurations targeted by the RDFModule
		int smoothing = rdfModule->keywords().asInt("Smoothing");
		RefListIterator<Configuration,bool> configIterator(rdfModule->targetConfigurations());
		while (Configuration* cfg = configIterator.iterate())
		{
			PartialSet& originalGR = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "OriginalGR");
			PartialSet& unweightedGR = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedGR");
			RDFModule::calculateUnweightedGR(originalGR, unweightedGR, broadening, smoothing);

			// Make sure the structure factors will be updated by the NeutronSQ module
			GenericListHelper<bool>::realise(cfg->moduleData(), "_ForceNeutronSQ") = true;
		}
	}

	// Go over NeutronSQ Modules, run the processing, and sum errors in an ReferenceData we have
	double totalError = 0.0;
	RefListIterator<Module,bool> neutronModuleIterator(intraBroadeningReferences_);
	while (Module* module = neutronModuleIterator.iterate())
	{
		// Check for ReferenceData first
		if (!duq_.processingModuleData().contains("ReferenceData", module->uniqueName())) continue;

		// Run the NeutronSQModule (quietly)
		Messenger::mute();
		module->executeMainProcessing(duq_, processPool_);
		Messenger::unMute();

		// Grab WeightedSQ and ReferenceData and compare
		PartialSet& weightedSQ = GenericListHelper<PartialSet>::retrieve(duq_.processingModuleData(), "WeightedSQ", module->uniqueName());
		XYData& referenceData = GenericListHelper<XYData>::retrieve(duq_.processingModuleData(), "ReferenceData", module->uniqueName());

		totalError += weightedSQ.total().error(referenceData);
	}

	return totalError;
}
