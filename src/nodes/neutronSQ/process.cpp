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
#include "nodes/gr/gr.h"
#include "nodes/neutronSQ/neutronSQ.h"
#include "nodes/sq/sq.h"

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

    // Get the real species populations from the input unweightedSQ
    auto &realSpeciesPopulations = unweightedSQ_->realSpeciesPopulations();

    // Calculate weights
    calculateWeights(realSpeciesPopulations);
    message("Isotopologue and isotope composition:\n\n");
    weights_.print();

    /*
     * Load and set up reference data (if a file/format was given)
     */
    if (referenceFQ_.hasFilename())
    {
        // Load the data
        Data1D referenceData;
        if (!referenceFQ_.importData(referenceData))
        {
            error("[SETUP {}] Failed to load reference data '{}'.\n", name(), referenceFQ_.filename());
            return NodeConstants::ProcessResult::Failed;
        }

        // Normalise reference data to be consistent with the calculated data
        if (referenceNormalisedTo_ != normaliseTo_)
        {
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
            Data1DExportFileFormat exportFormat(std::format("{}-ReferenceData.q", name()));
            if (!exportFormat.exportData(storedData))
                return NodeConstants::ProcessResult::Failed;
            Data1DExportFileFormat exportFormatFT(std::format("{}-ReferenceData.r", name()));
            if (!exportFormatFT.exportData(storedDataFT))
                return NodeConstants::ProcessResult::Failed;
        }
    }

    /*
     * Transform UnweightedSQ from provided SQ data into WeightedSQ.
     */

    // Does a PartialSet for the weighted S(Q) already exist for this Configuration?
    /*
    auto [weightedSQ, wSQstatus] = dissolve.processingModuleData().realiseIf<PartialSet>(
        "WeightedSQ", name(), GenericItem::InRestartFileFlag);
    if (wSQstatus == GenericItem::ItemStatus::Created)
        weightedSQ.setUpPartials(unweightedSQ.atomTypeMix());
    */

    // Set up the weighted SQ storage if needed
    if (!weightedSQ_)
    {
        weightedSQ_.emplace();
        weightedSQ_->initialise(*unweightedSQ_);
    }

    // Calculate weighted S(Q)
    calculateWeightedSQ();

    // Save data if requested
    if (saveSQ_ && !weightedSQ_->save(name(), "WeightedSQ", "sq", "Q, 1/Angstroms"))
        return NodeConstants::ProcessResult::Failed;

    /*
     * Transform UnweightedGR from into WeightedGR.
     */

    // Create/retrieve PartialSet for summed weighted g(r)
    /*
    auto [weightedGR, wGRstatus] = dissolve.processingModuleData().realiseIf<PartialSet>(
        "WeightedGR", name(), GenericItem::InRestartFileFlag);
    if (wGRstatus == GenericItem::ItemStatus::Created)
        weightedGR.setUpPartials(unweightedGR.atomTypeMix());
    */

    // Set up weighted GR storage if we need it
    if (!weightedGR_)
    {
        weightedGR_.emplace();
        weightedGR_->initialise(*unweightedGR_);
    }

    // Calculate weighted g(r)
    calculateWeightedGR();

    // Save data if requested
    if (saveGR_ && !weightedGR_->save(name(), "WeightedGR", "gr", "r, Angstroms"))
        return NodeConstants::ProcessResult::Failed;

    // Calculate representative total g(r) from FT of calculated F(Q)
    /*
    auto& repGR = dissolve.processingModuleData().realise<Data1D>("RepresentativeTotalGR", name(),
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
        auto& referenceData = dissolve.processingModuleData().realise<Data1D>("ReferenceData", name(),
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
        Data1DExportFileFormat exportFormat(std::format("{}-weighted-total.gr.broad", name()));
        if (!exportFormat.exportData(repGR))
            return NodeConstants::ProcessResult::Failed;
    }

    return NodeConstants::ProcessResult::Success;
}
