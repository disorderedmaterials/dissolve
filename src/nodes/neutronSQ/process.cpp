// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "math/mathFunc.h"

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/neutronWeights.h"
#include "classes/species.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "math/filters.h"
#include "math/ft.h"
#include "module/context.h"
#include "nodes/gr/gr.h"
#include "nodes/neutronSQ/neutronSQ.h"
#include "nodes/sq/sq.h"

// Run set-up stage
bool NeutronSQNode::setUp(Flags<KeywordBase::KeywordSignal> actionSignals)
{
    /*
     * Load and set up reference data (if a file/format was given)
     */
    if (referenceFQ_.hasFilename() && actionSignals.isSetOrNone(KeywordBase::ReloadExternalData))
    {
        // Load the data
        Data1D referenceData;
        if (!referenceFQ_.importData(referenceData, &processPool()))
        {
            error("[SETUP {}] Failed to load reference data '{}'.\n", name(), referenceFQ_.filename());
            return false;
        }

        // Normalise reference data to be consistent with the calculated data
        if (referenceNormalisedTo_ != normaliseTo_)
        {
            // We need the neutron weights_ in order to do the normalisation
            calculateWeights(weights_);
            auto factor = 1.0;

            // Set up the multiplication factors
            switch (referenceNormalisedTo_)
            {
                case (StructureFactors::NoNormalisation):
                    factor = 1.0 / (normaliseTo_ == StructureFactors::SquareOfAverageNormalisation
                                        ? weights_.boundCoherentSquareOfAverage()
                                        : weights_.boundCoherentAverageOfSquares());
                    break;
                case (StructureFactors::SquareOfAverageNormalisation):
                    factor = weights_.boundCoherentSquareOfAverage();
                    if (normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation)
                        factor /= weights_.boundCoherentAverageOfSquares();
                    break;
                case (StructureFactors::AverageOfSquaresNormalisation):
                    factor = weights_.boundCoherentAverageOfSquares();
                    if (normaliseTo_ == StructureFactors::SquareOfAverageNormalisation)
                        factor /= weights_.boundCoherentSquareOfAverage();
                    break;
                default:
                    Messenger::exception("Unhandled StructureFactor::NormalisationType ({}).\n",
                                         StructureFactors::normalisationTypes().keyword(referenceNormalisedTo_));
            }

            // Apply normalisation factors to the data
            referenceData *= factor;
        }

        // Get Q-range and window function to use for transformation of F(Q) to G(r)
        auto ftQMin = referenceFTQMin_.value_or(0.0);
        auto ftQMax = referenceFTQMax_.value_or(referenceData.xAxis().back() + 1.0);
        if (referenceWindowFunction_ == WindowFunction::Form::None)
            message("[SETUP {}] No window function will be applied in Fourier transform of reference data to g(r).", name());
        else
            message("[SETUP {}] Window function to be applied in Fourier transform of reference data is {}.", name(),
                    WindowFunction::forms().keyword(referenceWindowFunction_));

        // Store the reference data in processing
        referenceData.setTag(name());

        Data1D storedData;
        storedData = referenceData;

        // Calculate and store the FT of the reference data in processing
        referenceData.setTag(name());

        Data1D storedDataFT;
        storedDataFT = referenceData;
        Filters::trim(storedDataFT, ftQMin, ftQMax);

        auto rho = unweightedGR_->effectiveDensity();
        Fourier::sineFT(storedDataFT, 1.0 / (2.0 * M_PI * M_PI * rho), referenceFTDeltaR_, referenceFTDeltaR_, 30.0,
                        WindowFunction(referenceWindowFunction_));

        // Save data?
        if (saveReference_)
        {
            if (processPool().isMaster())
            {
                Data1DExportFileFormat exportFormat(std::format("{}-ReferenceData.q", name()));
                if (!exportFormat.exportData(storedData))
                    return processPool().decideFalse();
                Data1DExportFileFormat exportFormatFT(std::format("{}-ReferenceData.r", name()));
                if (!exportFormatFT.exportData(storedDataFT))
                    return processPool().decideFalse();
                processPool().decideTrue();
            }
            else if (!processPool().decision())
                return false;
        }
    }

    return true;
}

// Run main processing
NodeConstants::ProcessResult NeutronSQNode::process()
{
    // Print argument/parameter summary
    if (referenceWindowFunction_ == WindowFunction::Form::None)
        message("No window function will be applied when calculating representative g(r) from S(Q).");
    else
        message("Window function to be applied when calculating representative g(r) from S(Q) is {}.",
                WindowFunction::forms().keyword(referenceWindowFunction_));
    if (normaliseTo_ == StructureFactors::NoNormalisation)
        message("NeutronSQ: No normalisation will be applied to total F(Q).\n");
    else if (normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation)
        message("NeutronSQ: Total F(Q) will be normalised to <b**2>");
    else if (normaliseTo_ == StructureFactors::SquareOfAverageNormalisation)
        message("NeutronSQ: Total F(Q) will be normalised to <b>**2");
    if (saveSQ_)
        message("NeutronSQ: Weighted partial S(Q) and total F(Q) will be saved.\n");
    if (saveGR_)
        message("NeutronSQ: Weighted partial g(r) and total G(r) will be saved.\n");
    if (saveRepresentativeGR_)
        message("NeutronSQ: Representative G(r) will be saved.\n");
    message("\n");

    /*
     * Transform UnweightedSQ from provided SQ data into WeightedSQ.
     */
    // Calculate and store weights_
    /*
    auto& weights_ = moduleContext.dissolve().processingModuleData().realise<NeutronWeights>("FullWeights", name(),
        GenericItem::InRestartFileFlag);
    */
    calculateWeights(weights_);
    message("Isotopologue and isotope composition:\n\n");
    weights_.print();

    // Does a PartialSet for the weighted S(Q) already exist for this Configuration?
    /*
    auto [weightedSQ, wSQstatus] = moduleContext.dissolve().processingModuleData().realiseIf<PartialSet>(
        "WeightedSQ", name(), GenericItem::InRestartFileFlag);
    if (wSQstatus == GenericItem::ItemStatus::Created)
        weightedSQ.setUpPartials(unweightedSQ.atomTypeMix());
    */

    if (!weightedGR_)
    {
        weightedSQ_.emplace(unweightedSQ_->speciesPopulations());
        weightedSQ_->setUpPartials(unweightedSQ_->atomTypeMix());
    }

    auto &population = weightedGR_->speciesPopulations();
    for (const auto &[species, _] : population)
    {
        for (const auto &isotopologue : species->isotopologues())
        {
            auto iso = isotopologue.get();
            auto it = namedWeights_.find(iso->name());
            if (it != namedWeights_.end())
                isotopologueSet_.add(iso, it->second);
        }
    }

    // Calculate weighted S(Q)
    calculateWeightedSQ(*unweightedSQ_, *weightedSQ_, weights_, normaliseTo_);

    // Save data if requested
    /*
    if (saveSQ_ && (!MPIRunMaster(processPool(), weightedSQ.save(name(), "WeightedSQ", "sq", "Q, 1/Angstroms"))))
        return NodeConstants::ProcessResult::Failed;
    */

    /*
     * Transform UnweightedGR from underlying RDF data into WeightedGR.
     */

    // Get summed unweighted g(r) from the RDFMOdule
    /*
    if (!moduleContext.dissolve().processingModuleData().contains("UnweightedGR", rdfModule->name()))
    {
        error("Couldn't locate summed unweighted g(r) data.\n");
        return NodeConstants::ProcessResult::Failed;
    }
    */

    /*
    const auto& unweightedGR =
        moduleContext.dissolve().processingModuleData().value<PartialSet>("UnweightedGR", rdfModule->name());
    */

    // Create/retrieve PartialSet for summed weighted g(r)
    /*
    auto [weightedGR, wGRstatus] = moduleContext.dissolve().processingModuleData().realiseIf<PartialSet>(
        "WeightedGR", name(), GenericItem::InRestartFileFlag);
    if (wGRstatus == GenericItem::ItemStatus::Created)
        weightedGR.setUpPartials(unweightedGR.atomTypeMix());
    */

    if (!weightedGR_)
    {
        weightedGR_.emplace(unweightedGR_->speciesPopulations());
        weightedGR_->setUpPartials(unweightedGR_->atomTypeMix());
    }

    // Calculate weighted g(r)
    calculateWeightedGR(*unweightedGR_, *weightedGR_, weights_, normaliseTo_);

    // Save data if requested
    /*
    if (saveGR_ && (!MPIRunMaster(processPool(), weightedGR.save(name(), "WeightedGR", "gr", "r, Angstroms"))))
        return NodeConstants::ProcessResult::Failed;
    */
    // Calculate representative total g(r) from FT of calculated F(Q)
    /*
    auto& repGR = moduleContext.dissolve().processingModuleData().realise<Data1D>("RepresentativeTotalGR", name(),
        GenericItem::InRestartFileFlag);
    */
    Data1D repGR;
    repGR = weightedSQ_->total();
    auto ftQMax = 0.0;
    if (referenceFTQMax_)
        ftQMax = referenceFTQMax_.value();
    else if (referenceFQ_.hasFilename())
    {
        // Take FT max Q limit from reference data
        /*
        auto& referenceData = moduleContext.dissolve().processingModuleData().realise<Data1D>("ReferenceData", name(),
            GenericItem::ProtectedFlag);
        */
        Data1D referenceData;
        ftQMax = referenceData.xAxis().back();
    }
    else
        ftQMax = weightedSQ_->total().xAxis().back();
    Filters::trim(repGR, referenceFTQMin_.value_or(0.0), ftQMax);
    auto rMin = weightedGR_->total().xAxis().front();
    auto rMax = weightedGR_->total().xAxis().back();
    WindowFunction window(referenceWindowFunction_);
    Fourier::sineFT(repGR, 1.0 / (2.0 * M_PI * M_PI * unweightedGR_->effectiveDensity()), rMin, 0.05, rMax, window);

    // Save data if requested
    if (saveRepresentativeGR_)
    {
        if (processPool().isMaster())
        {
            Data1DExportFileFormat exportFormat(std::format("{}-weighted-total.gr.broad", name()));
            if (exportFormat.exportData(repGR))
                processPool().decideTrue();
            else
                processPool().decideFalse();
        }
        else if (!processPool().decision())
            return NodeConstants::ProcessResult::Failed;
    }

    return NodeConstants::ProcessResult::Success;
}
