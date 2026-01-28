// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

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

    // Set up the weighted SQ storage if needed
    if (!weightedSQ_)
    {
        weightedSQ_.emplace();
        weightedSQ_.value().initialise(*unweightedSQ_);
    }

    // Set up weighted GR storage if we need it
    if (!weightedGR_)
    {
        weightedGR_.emplace();
        weightedGR_.value().initialise(*unweightedGR_);
    }

    // Calculate weights
    NeutronWeights weights(unweightedSQ_->realSpeciesPopulations(), isotopologues_, exchangeable_);

    // Set up reference data if supplied
    if (referenceFQ_)
    {
        // Normalise reference data to be consistent with the calculated data
        if (referenceNormalisedTo_ != normaliseTo_)
        {
            auto factor = 1.0;

            // Set up the multiplication factors
            switch (referenceNormalisedTo_)
            {
                case (StructureFactors::NoNormalisation):
                    factor = 1.0 / (normaliseTo_ == StructureFactors::SquareOfAverageNormalisation
                                        ? weights.boundCoherentSquareOfAverage()
                                        : weights.boundCoherentAverageOfSquares());
                    break;
                case (StructureFactors::SquareOfAverageNormalisation):
                    factor = weights.boundCoherentSquareOfAverage();
                    if (normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation)
                        factor /= weights.boundCoherentAverageOfSquares();
                    break;
                case (StructureFactors::AverageOfSquaresNormalisation):
                    factor = weights.boundCoherentAverageOfSquares();
                    if (normaliseTo_ == StructureFactors::SquareOfAverageNormalisation)
                        factor /= weights.boundCoherentSquareOfAverage();
                    break;
                default:
                    Messenger::exception("Unhandled StructureFactor::NormalisationType ({}).\n",
                                         StructureFactors::normalisationTypes().keyword(referenceNormalisedTo_));
            }

            // Apply normalisation factor to the data
            *referenceFQ_ *= factor;
        }

        // Get Q-range and window function to use for transformation of F(Q) to G(r)
        auto ftQMin = referenceFTQMin_.value_or(0.0);
        auto ftQMax = referenceFTQMax_.value_or(referenceFQ_->xAxis().back() + 1.0);
        if (referenceWindowFunction_ == WindowFunction::Form::None)
            message("[SETUP {}] No window function will be applied in Fourier transform of reference data to g(r).", name());
        else
            message("[SETUP {}] Window function to be applied in Fourier transform of reference data is {}.", name(),
                    WindowFunction::forms().keyword(referenceWindowFunction_));

        // Calculate FT of the reference data
        referenceGR_ = *referenceFQ_;
        Filters::trim(referenceGR_, ftQMin, ftQMax);

        auto rho = unweightedGR_->effectiveDensity();
        Fourier::sineFT(referenceGR_, 1.0 / (2.0 * M_PI * M_PI * rho), referenceFTDeltaR_, referenceFTDeltaR_, 30.0,
                        WindowFunction(referenceWindowFunction_));

        // Save data?
        if (saveReference_)
        {
            Data1DExportFileFormat exportFormat(std::format("{}-ReferenceData.q", name()));
            if (!exportFormat.exportData(*referenceFQ_))
                return NodeConstants::ProcessResult::Failed;
            Data1DExportFileFormat exportFormatFT(std::format("{}-ReferenceData.r", name()));
            if (!exportFormatFT.exportData(referenceGR_))
                return NodeConstants::ProcessResult::Failed;
        }
    }

    /*
     * Transform UnweightedSQ from provided SQ data into WeightedSQ.
     */

    // Calculate weighted S(Q)
    calculateWeightedSQ(weights);

    // Save data if requested
    if (saveSQ_ && !weightedSQ_->save(name(), "WeightedSQ", "sq", "Q, 1/Angstroms"))
        return NodeConstants::ProcessResult::Failed;

    /*
     * Transform UnweightedGR into WeightedGR.
     */

    // Calculate weighted g(r)
    calculateWeightedGR(weights);

    // Save data if requested
    if (saveGR_ && !weightedGR_->save(name(), "WeightedGR", "gr", "r, Angstroms"))
        return NodeConstants::ProcessResult::Failed;

    // Calculate representative total g(r) from FT of calculated F(Q)
    representativeGR_ = weightedSQ_->total();
    auto ftQMax = 0.0;
    if (referenceFTQMax_)
        ftQMax = referenceFTQMax_.value();
    else if (referenceFQ_)
    {
        // Take FT max Q limit from reference data
        ftQMax = referenceFQ_->xAxis().back();
    }
    else
        ftQMax = weightedSQ_->total().xAxis().back();
    Filters::trim(representativeGR_, referenceFTQMin_.value_or(0.0), ftQMax);
    auto rMin = weightedGR_->total().xAxis().front();
    auto rMax = weightedGR_->total().xAxis().back();
    WindowFunction window(referenceWindowFunction_);
    Fourier::sineFT(representativeGR_, 1.0 / (2.0 * M_PI * M_PI * unweightedGR_->effectiveDensity()), rMin, 0.05, rMax, window);

    // Save data if requested
    if (saveRepresentativeGR_)
    {
        Data1DExportFileFormat exportFormat(std::format("{}-weighted-total.gr.broad", name()));
        if (!exportFormat.exportData(representativeGR_))
            return NodeConstants::ProcessResult::Failed;
    }

    return NodeConstants::ProcessResult::Success;
}
