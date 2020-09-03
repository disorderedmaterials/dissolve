/*
    *** Calibration Module - Processing
    *** src/modules/calibration/process.cpp
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

#include "base/sysfunc.h"
#include "classes/partialset.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include "math/praxis.h"
#include "modules/calibration/calibration.h"
#include "modules/energy/energy.h"
#include "modules/rdf/rdf.h"

// Run main processing
bool CalibrationModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Perform calibration of various aspects
     */

    // Retrieve / print keyword setup
    const bool onlyWhenEnergyStable = keywords_.asBool("OnlyWhenEnergyStable");

    if (onlyWhenEnergyStable)
        Messenger::print("Calibration: Adjustments will only be performed if all related Configuration energies are stable.\n");

    /*
     * IntraBroadening Calibration
     */

    if (intraBroadeningModules_.nItems() > 0)
    {
        /*
         * Make a list of all Configurations related to all RDF module
         */
        RefList<Configuration> configs;
        for (Module *module : intraBroadeningModules_)
        {
            for (Configuration *cfg : module->targetConfigurations())
                configs.addUnique(cfg);
        }
        Messenger::print("{} Configuration(s) are involved over all RDF Module targets.\n", configs.nItems());

        /*
         * Are the energies of all involved Configurations stable (if OnlyWhenEnergyStable option is on)
         */
        if (onlyWhenEnergyStable)
        {
            auto stabilityResult = EnergyModule::nUnstable(configs);
            if (stabilityResult == EnergyModule::NotAssessable)
                return false;
            else if (stabilityResult > 0)
            {
                Messenger::print("At least one Configuration energy is not yet stable. No adjustments will be "
                                 "made this iteration.\n");
                return true;
            }
        }

        /*
         * Create master reference lists of NeutronSQ modules over any specified S(Q) and/or G(r) data reference targets
         */
        RefDataList<Module, CalibrationModule::IntraBroadeningFitTarget> neutronReferences;

        for (Module *module : intraBroadeningNeutronSQReferences_)
            neutronReferences.addUnique(module, CalibrationModule::IntraBroadeningTargetSQ);

        for (Module *module : intraBroadeningNeutronGRReferences_)
        {
            // If the Module target is already in the list, just set its data to 'both'
            RefDataItem<Module, CalibrationModule::IntraBroadeningFitTarget> *oldItem = neutronReferences.contains(module);
            if (oldItem)
                oldItem->data() = CalibrationModule::IntraBroadeningTargetBoth;
            else
                neutronReferences.append(module, CalibrationModule::IntraBroadeningTargetGR);
        }

        /*
         * Assemble a list of fitting parameters from the associated RDF modules and their BroadeningFunctions
         */

        CalibrationModuleCostFunctions costFunctions(dissolve, procPool, intraBroadeningModules_, neutronReferences);
        PrAxisMinimiser<CalibrationModuleCostFunctions> broadeningMinimiser(
            costFunctions, &CalibrationModuleCostFunctions::intraBroadeningCost);
        broadeningMinimiser.setTolerance(0.001);
        broadeningMinimiser.setPokeBeforeCost(true);
        Array<bool> broadeningAdded(PairBroadeningFunction::nFunctionTypes);
        broadeningAdded = false;

        for (Module *module : intraBroadeningModules_)
        {
            // Retrieve the PairBroadeningFunction
            auto &broadening = module->keywords().retrieve<PairBroadeningFunction>("IntraBroadening", PairBroadeningFunction());

            // Add its parameters to our minimiser - only add broadening functions with global parameters once
            switch (broadening.function())
            {
                case (PairBroadeningFunction::NoFunction):
                    return Messenger::error("No pair broadening function set, so nothing to optimise.\n");
                case (PairBroadeningFunction::GaussianElementPairFunction):
                    if (broadeningAdded[broadening.function()])
                    {
                        Messenger::print("Broadening function of type '{}' used over multiple RDF modules, so "
                                         "parameters will only be added once.\n",
                                         PairBroadeningFunction::functionType(broadening.function()));
                        continue;
                    }
                    break;
                default:
                    break;
            }

            broadeningMinimiser.addTargets(broadening.parameters(), true, 0.02, true, 0.5);

            // Flag that a broadening of this type has been added
            broadeningAdded[broadening.function()] = true;
        }

        // Optimise the parameters - the cost function will regenerate the UnweightedGR in the RDF modules, and
        // reassemble the target NeutronSQ data
        double error = broadeningMinimiser.minimise();

        Messenger::print("Total RMSE over all specified datasets is {}.\n", error);

        // Make sure that we re-broaden the RDFs and NeutronSQ data by the correct (optimal) values before we leave
        // Store alpha parameters in the PairBroadeningFunction in the associated RDF modules
        for (Module *rdfModule : intraBroadeningModules_)
        {
            // Retrieve the PairBroadeningFunction
            auto &broadening =
                rdfModule->keywords().retrieve<PairBroadeningFunction>("IntraBroadening", PairBroadeningFunction());

            Messenger::print("Optimal IntraBroadening parameters for '{}' are now: {}\n", rdfModule->uniqueName(),
                             broadening.summary());

            // Recalculate the UnweightedGR for all Configurations targeted by the RDFModule
            auto smoothing = rdfModule->keywords().asInt("Smoothing");
            for (Configuration *cfg : rdfModule->targetConfigurations())
            {
                const auto &originalGR = GenericListHelper<PartialSet>::value(cfg->moduleData(), "OriginalGR");
                auto &unweightedGR = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedGR");
                RDFModule::calculateUnweightedGR(procPool, cfg, originalGR, unweightedGR, broadening, smoothing);
            }

            // Store the new broadening parameters in the restart file for info only (they won't be read in and
            // used)
            GenericListHelper<PairBroadeningFunction>::realise(dissolve.processingModuleData(),
                                                               fmt::format("{}-IntraBroadening", rdfModule->uniqueName()),
                                                               uniqueName(), GenericItem::InRestartFileFlag) = broadening;
        }

        // Go over NeutronSQ Modules and run the processing
        RefDataListIterator<Module, CalibrationModule::IntraBroadeningFitTarget> neutronModuleIterator(neutronReferences);
        while (Module *module = neutronModuleIterator.iterate())
        {
            // Make sure the structure factors will be updated by the NeutronSQ module - set flag in the target
            // Configurations
            for (Configuration *cfg : module->targetConfigurations())
                GenericListHelper<bool>::realise(cfg->moduleData(), "_ForceNeutronSQ") = true;

            // Run the NeutronSQModule (quietly)
            Messenger::mute();
            module->executeProcessing(dissolve, procPool);
            Messenger::unMute();
        }
    }

    return true;
}
