/*
	*** Calibration Module - Processing
	*** src/modules/calibration/process.cpp
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
#include "modules/energy/energy.h"
#include "modules/rdf/rdf.h"
#include "classes/partialset.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"
#include "math/praxis.h"

// Run main processing
bool CalibrationModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/* 
	 * Perform calibration of various aspects
	 */

	// Retrieve / print keyword setup
	const bool onlyWhenEnergyStable = keywords_.asBool("OnlyWhenEnergyStable");

	if (onlyWhenEnergyStable) Messenger::print("Calibration: Adjustments will only be performed if all related Configuration energies are stable.\n");


	/*
	 * IntraBroadening Calibration
	 */

	if (intraBroadeningModules_.nItems() > 0)
	{
		/*
		 * Make a list of all Configurations related to all RDF module
		 */
		RefList<Configuration,bool> configs;
		RefListIterator<Module,bool> rdfModuleIterator(intraBroadeningModules_);
		while (Module* module = rdfModuleIterator.iterate())
		{
			RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
			while (Configuration* cfg = configIterator.iterate()) configs.addUnique(cfg);
		}
		Messenger::print("%i Configuration(s) are involved over all RDF Module targets.\n", configs.nItems());


		/*
		 * Are the energies of all involved Configurations stable (if OnlyWhenEnergyStable option is on)
		 */
		if (onlyWhenEnergyStable)
		{
			int stabilityResult = EnergyModule::checkStability(configs);
			if (stabilityResult == -1) return false;
			else if (stabilityResult != 0)
			{
				Messenger::print("At least one Configuration energy is not yet stable. No adjustments will be made this iteration.\n");
				return true;
			}
		}


		/*
		 * Create master reference lists of NeutronSQ modules over any specified S(Q) and/or G(r) data reference targets
		 */
		RefList<Module,CalibrationModule::IntraBroadeningFitTarget> neutronReferences;

		RefListIterator<Module,bool> neutronSQIterator(intraBroadeningNeutronSQReferences_);
		while (Module* module = neutronSQIterator.iterate()) neutronReferences.addUnique(module, CalibrationModule::IntraBroadeningTargetSQ);

		RefListIterator<Module,bool> neutronGRIterator(intraBroadeningNeutronGRReferences_);
		while (Module* module = neutronGRIterator.iterate())
		{
			// If the Module target is already in the list, just set its data to 'both'
			RefListItem<Module,CalibrationModule::IntraBroadeningFitTarget>* oldItem = neutronReferences.contains(module);
			if (oldItem) oldItem->data = CalibrationModule::IntraBroadeningTargetBoth;
			else neutronReferences.add(module, CalibrationModule::IntraBroadeningTargetGR);
		}


		/*
		 * Assemble a list of fitting parameters from the associated RDF modules and their BroadeningFunctions
		 */
		
		CalibrationModuleCostFunctions costFunctions(dissolve, procPool, intraBroadeningModules_, neutronReferences);
		PrAxis<CalibrationModuleCostFunctions> broadeningMinimiser(costFunctions, &CalibrationModuleCostFunctions::intraBroadeningCost, true);
		rdfModuleIterator.restart();
		Array<bool> broadeningAdded(PairBroadeningFunction::nFunctionTypes);
		broadeningAdded = false;
		
		while (Module* module = rdfModuleIterator.iterate())
		{
			// Retrieve the PairBroadeningFunction
			PairBroadeningFunction& broadening = KeywordListHelper<PairBroadeningFunction>::retrieve(module->keywords(), "IntraBroadening", PairBroadeningFunction());

			// Add its parameters to our minimiser - only add broadening functions with global parameters once
			switch (broadening.function())
			{
				case (PairBroadeningFunction::NoFunction):
					return Messenger::error("No pair broadening function set, so nothing to optimise.\n");
				case (PairBroadeningFunction::GaussianElementPairFunction):
					if (broadeningAdded[broadening.function()])
					{
						Messenger::print("Broadening function of type '%s' used over multiple RDF modules, so parameters will only be added once.\n", PairBroadeningFunction::functionType(broadening.function()));
						continue;
					}
					break;
				default:
					break;
			}

			broadeningMinimiser.addTargets(broadening.parameters());

			// Flag that a broadening of this type has been added
			broadeningAdded[broadening.function()] = true;
		}

		// Optimise the parameters - the cost function will regenerate the UnweightedGR in the RDF modules, and reassemble the target NeutronSQ data
		double error = broadeningMinimiser.minimise(0.001);

		Messenger::print("Total RMSE over all specified datasets is %f.\n", error);

		// Make sure that we re-broaden the RDFs and NeutronSQ data by the correct (optimal) values before we leave
		// Store alpha parameters in the PairBroadeningFunction in the associated RDF modules
		rdfModuleIterator.restart();
		while (Module* rdfModule = rdfModuleIterator.iterate())
		{
			// Retrieve the PairBroadeningFunction
			PairBroadeningFunction& broadening = KeywordListHelper<PairBroadeningFunction>::retrieve(rdfModule->keywords(), "IntraBroadening", PairBroadeningFunction());

			Messenger::print("Optimal IntraBroadening parameters for '%s' are now: %s\n", rdfModule->uniqueName(), broadening.summary().get());

			// Recalculate the UnweightedGR for all Configurations targeted by the RDFModule
			int smoothing = rdfModule->keywords().asInt("Smoothing");
			RefListIterator<Configuration,bool> configIterator(rdfModule->targetConfigurations());
			while (Configuration* cfg = configIterator.iterate())
			{
				const PartialSet& originalGR = GenericListHelper<PartialSet>::value(cfg->moduleData(), "OriginalGR");
				PartialSet& unweightedGR = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedGR");
				RDFModule::calculateUnweightedGR(originalGR, unweightedGR, broadening, smoothing);
			}

			// Store the new broadening parameters in the restart file for info only (they won't be read in and used)
			GenericListHelper<PairBroadeningFunction>::realise(dissolve.processingModuleData(), CharString("%s-IntraBroadening", rdfModule->uniqueName()), uniqueName(), GenericItem::InRestartFileFlag) = broadening;
		}

		// Go over NeutronSQ Modules and run the processing
		RefListIterator<Module,CalibrationModule::IntraBroadeningFitTarget> neutronModuleIterator(neutronReferences);
		while (Module* module = neutronModuleIterator.iterate())
		{
			// Make sure the structure factors will be updated by the NeutronSQ module - set flag in the target Configurations
			RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
			while (Configuration* cfg = configIterator.iterate()) GenericListHelper<bool>::realise(cfg->moduleData(), "_ForceNeutronSQ") = true;

			// Run the NeutronSQModule (quietly)
			Messenger::mute();
			module->executeProcessing(dissolve, procPool);
			Messenger::unMute();
		}
	}

	return true;
}

