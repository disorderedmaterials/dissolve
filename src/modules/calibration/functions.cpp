// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/partialset.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include "math/broadeningfunction.h"
#include "math/error.h"
#include "modules/calibration/calibration.h"
#include "modules/rdf/rdf.h"

/*
 * CalibrationModuleCostFunctions
 */

CalibrationModuleCostFunctions::CalibrationModuleCostFunctions(
    Dissolve &dissolve, ProcessPool &procPool, RefList<Module> &intraBroadeningModules,
    RefDataList<Module, CalibrationModule::IntraBroadeningFitTarget> &intraBroadeningReferences)
    : dissolve_(dissolve), processPool_(procPool), intraBroadeningModules_(intraBroadeningModules),
      intraBroadeningReferences_(intraBroadeningReferences)
{
}

// Cost function for intraBroadening minimisation
double CalibrationModuleCostFunctions::intraBroadeningCost(const std::vector<double> &alpha)
{
    // Store alpha parameters in the PairBroadeningFunction in the associated RDF modules
    for (Module *rdfModule : intraBroadeningModules_)
    {
        // Retrieve the PairBroadeningFunction - new test values will already have been set (pokeBeforeCost = true)
        auto &broadening = rdfModule->keywords().retrieve<PairBroadeningFunction>("IntraBroadening", PairBroadeningFunction());

        // Recalculate the UnweightedGR for all Configurations targeted by the RDFModule
        auto smoothing = rdfModule->keywords().asInt("Smoothing");
        for (Configuration *cfg : rdfModule->targetConfigurations())
        {
            const auto &originalGR = GenericListHelper<PartialSet>::value(cfg->moduleData(), "OriginalGR");
            auto &unweightedGR = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedGR");
            RDFModule::calculateUnweightedGR(processPool_, cfg, originalGR, unweightedGR, broadening, smoothing);
        }
    }

    // Go over NeutronSQ Modules, run the processing, and sum errors in an ReferenceData we have
    double totalError = 0.0;
    RefDataListIterator<Module, CalibrationModule::IntraBroadeningFitTarget> neutronModuleIterator(intraBroadeningReferences_);
    while (Module *module = neutronModuleIterator.iterate())
    {
        // Check for ReferenceData first
        if (!dissolve_.processingModuleData().contains("ReferenceData", module->uniqueName()))
            continue;

        // Make sure the structure factors will be updated by the NeutronSQ module - set flag in the target
        // Configurations
        for (Configuration *cfg : module->targetConfigurations())
            GenericListHelper<bool>::realise(cfg->moduleData(), "_ForceNeutronSQ") = true;

        // Run the NeutronSQModule (quietly)
        Messenger::mute();
        module->executeProcessing(dissolve_, processPool_);
        Messenger::unMute();

        // Grab target data and compare
        CalibrationModule::IntraBroadeningFitTarget target = neutronModuleIterator.currentData();
        if ((target == CalibrationModule::IntraBroadeningTargetBoth) || (target == CalibrationModule::IntraBroadeningTargetSQ))
        {
            // Grab WeightedSQ and ReferenceData and compare
            const PartialSet &weightedSQ =
                GenericListHelper<PartialSet>::value(dissolve_.processingModuleData(), "WeightedSQ", module->uniqueName());
            const Data1D &referenceData =
                GenericListHelper<Data1D>::value(dissolve_.processingModuleData(), "ReferenceData", module->uniqueName());
            totalError += Error::rmse(weightedSQ.total(), referenceData, true);
        }
        if ((target == CalibrationModule::IntraBroadeningTargetBoth) || (target == CalibrationModule::IntraBroadeningTargetGR))
        {
            // Grab WeightedGR and ReferenceDataFT and compare
            const PartialSet &weightedGR =
                GenericListHelper<PartialSet>::value(dissolve_.processingModuleData(), "WeightedGR", module->uniqueName());
            const Data1D &referenceDataFT =
                GenericListHelper<Data1D>::value(dissolve_.processingModuleData(), "ReferenceDataFT", module->uniqueName());
            totalError += Error::rmse(weightedGR.total(), referenceDataFT, true);
        }
    }

    return totalError;
}
