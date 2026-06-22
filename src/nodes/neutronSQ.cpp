// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/neutronSQ.h"
#include "classes/configuration.h"
#include "classes/neutronWeights.h"
#include "main/dissolve.h"
#include "math/filters.h"
#include "math/ft.h"
#include "nodes/edge.h"
#include "nodes/exportData.h"
#include "nodes/sq/sq.h"

NeutronSQNode::NeutronSQNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<PartialSet *>("UnweightedSQ", "Unweighted partial S(Q)", unweightedSQ_);
    addInput<PartialSet *>("UnweightedGR", "Unweighted partials g(r)", unweightedGR_);
    addInput<std::optional<Data1D>>("ReferenceData", "Reference F(Q) data", referenceFQ_);

    // Outputs
    addOptionalPointerOutput<PartialSet>("WeightedSQ", "Weighted partial structure factors for target configuration",
                                         weightedSQ_);
    addOptionalPointerOutput<PartialSet>(
        "WeightedGR", "Weighted partial radial distribution functions for target configuration", weightedGR_);
    addOutput<Data1D>("ReferenceGR", "Fourier transform of reference data", referenceGR_);

    // Options
    addOption<IsotopologueSet>("Isotopologues", "Isotopologues to use when calculating weights matrix", isotopologues_);
    addOption<StructureFactors::NormalisationType>("NormaliseTo", "Normalisation to apply to total weighted F(Q)",
                                                   normaliseTo_);
    addOption<StructureFactors::NormalisationType>(
        "ReferenceNormalisedTo", "Normalisation that has been applied to the reference data", referenceNormalisedTo_);
    addOption<std::optional<Number>>("ReferenceFTQMin",
                                     "Minimum Q value to use when Fourier-transforming reference data (0.0 for no minimum)",
                                     referenceFTQMin_);
    addOption<std::optional<Number>>("ReferenceFTQMax",
                                     "Maximum Q value to use when Fourier-transforming reference data (0.0 for no maximum)",
                                     referenceFTQMax_);
    addOption<Number>("ReferenceFTDeltaR", "Spacing in r to use when generating the Fourier-transformed data",
                      referenceFTDeltaR_);
    addOption<WindowFunction::Form>("ReferenceWindowFunction",
                                    "Window function to apply when Fourier-transforming reference S(Q) to g(r)",
                                    referenceWindowFunction_);
    addOption<bool>("SaveGR", "Save weighted g(r) and G(r)", saveGR_);
    addOption<bool>("SaveReference", "Save the reference data and its Fourier transform", saveReference_);
    addOption<bool>("SaveRepresentativeGR", "Save representative G(r), obtained from Fourier transform of the calculated F(Q)",
                    saveRepresentativeGR_);
    addOption<bool>("SaveSQ", "Save weighted partial and total structure factors", saveSQ_);

    // Serialisables
    addSerialisable("weightedGR", weightedGR_);
    addSerialisable("weightedSQ", weightedSQ_);
    addSerialisable("representativeGR", representativeGR_);
}

/*
 * Definition
 */

std::string_view NeutronSQNode::type() const { return "NeutronSQ"; }

std::string_view NeutronSQNode::summary() const { return "Calculate neutron-weighted S(Q) and G(r)"; }

/*
 * Data
 */

// Return neutron weights
NeutronWeights NeutronSQNode::weights() const
{
    return NeutronWeights(unweightedSQ_->realSpeciesPopulations(), isotopologues_);
}

// Returns the unweighted SQ
const PartialSet *NeutronSQNode::unweightedSQ() const { return unweightedSQ_; }

// Returns the unweighted GR
const PartialSet *NeutronSQNode::unweightedGR() const { return unweightedGR_; }

// Returns the isotopologues
const IsotopologueSet &NeutronSQNode::isotopologues() const { return isotopologues_; }

// Returns the source configuration, belonging to the input SQ node
const Configuration *NeutronSQNode::sourceConfiguration()
{
    auto cfgInputEdge = inputEdges().find("UnweightedSQ");

    if (cfgInputEdge == inputEdges().end())
    {
        error("Could not find a valid input 'UnweightedSQ' associated with this node ({})", name());
        return nullptr;
    }

    auto &cfgSourceNode = cfgInputEdge->second[0]->sourceNode();
    auto sqNode = static_cast<SQNode *>(&cfgSourceNode);

    return sqNode->sourceConfiguration();
}

/*
 * Processing
 */

// Calculate weighted g(r)
bool NeutronSQNode::calculateWeightedGR(const NeutronWeights &weights)
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedGR_->atomTypeFractions(),
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};

                                auto weight = weights.weights().get(key);
                                auto intraWeight = weights.intramolecularWeights().get(key);

                                // Bound (intramolecular) partial (multiplied by the bound term weight)
                                weightedGR_->boundPartials().get(key).copyArrays(unweightedGR_->boundPartials().get(key));
                                weightedGR_->boundPartials().get(key) *= intraWeight;

                                // Unbound partial (multiplied by the full weight)
                                weightedGR_->unboundPartials().get(key).copyArrays(unweightedGR_->unboundPartials().get(key));
                                weightedGR_->unboundPartials().get(key) -= 1.0;
                                weightedGR_->unboundPartials().get(key) *= weight;

                                // Full partial, summing bound and unbound terms
                                weightedGR_->partials().get(key).copyArrays(weightedGR_->unboundPartials().get(key));
                                weightedGR_->partials().get(key) += weightedGR_->boundPartials().get(key);
                            });

    // Calculate and normalise total to form factor if requested
    weightedGR_->formTotals(false);

    // Normalise to Q=0.0 form factor if requested
    if (normaliseTo_ != StructureFactors::NoNormalisation)
    {
        auto norm = normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation ? weights.boundCoherentAverageOfSquares()
                                                                                    : weights.boundCoherentSquareOfAverage();

        weightedGR_->total() /= norm;
        weightedGR_->boundTotal() /= norm;
        weightedGR_->unboundTotal() /= norm;
    }

    return true;
}

// Calculate weighted S(Q)
bool NeutronSQNode::calculateWeightedSQ(const NeutronWeights &weights)
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedSQ_->atomTypeFractions(),
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};

                                // Weight bound and unbound S(Q) and sum into full partial
                                auto weight = weights.weights().get(key);
                                auto boundWeight = weights.intramolecularWeights().get(key);

                                // Bound (intramolecular) partial (multiplied by the bound term weight)
                                weightedSQ_->boundPartials().get(key).copyArrays(unweightedSQ_->boundPartials().get(key));
                                weightedSQ_->boundPartials().get(key) *= boundWeight;

                                // Unbound partial (multiplied by the full weight)
                                weightedSQ_->unboundPartials().get(key).copyArrays(unweightedSQ_->unboundPartials().get(key));
                                weightedSQ_->unboundPartials().get(key) *= weight;

                                // Full partial (sum of bound and unbound terms)
                                weightedSQ_->partials().get(key).copyArrays(weightedSQ_->unboundPartials().get(key));
                                weightedSQ_->partials().get(key) += weightedSQ_->boundPartials().get(key);
                            });

    // Form total structure factor
    weightedSQ_->formTotals(false);

    // Apply normalisation to all totals
    if (normaliseTo_ != StructureFactors::NoNormalisation)
    {
        auto norm = normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation ? weights.boundCoherentAverageOfSquares()
                                                                                    : weights.boundCoherentSquareOfAverage();

        weightedSQ_->total() /= norm;
        weightedSQ_->boundTotal() /= norm;
        weightedSQ_->unboundTotal() /= norm;
    }

    return true;
}

// Run main processing
NodeConstants::ProcessResult NeutronSQNode::process()
{
    auto referenceFTQMin = std::optional<double>(referenceFTQMin_->asDouble());
    auto referenceFTQMax = std::optional<double>(referenceFTQMax_->asDouble());
    auto referenceFTDeltaR = referenceFTDeltaR_.asDouble();

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
    NeutronWeights weights(unweightedSQ_->realSpeciesPopulations(), isotopologues_);

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
        auto ftQMin = referenceFTQMin.value_or(0.0);
        auto ftQMax = referenceFTQMax.value_or(referenceFQ_->xAxis().back() + 1.0);
        if (referenceWindowFunction_ == WindowFunction::Form::None)
            message("[SETUP {}] No window function will be applied in Fourier transform of reference data to g(r).", name());
        else
            message("[SETUP {}] Window function to be applied in Fourier transform of reference data is {}.", name(),
                    WindowFunction::forms().keyword(referenceWindowFunction_));

        // Calculate FT of the reference data
        referenceGR_ = *referenceFQ_;
        Filters::trim(referenceGR_, ftQMin, ftQMax);

        auto rho = unweightedGR_->effectiveDensity();
        Fourier::sineFT(referenceGR_, 1.0 / (2.0 * M_PI * M_PI * rho), referenceFTDeltaR, referenceFTDeltaR, 30.0,
                        WindowFunction(referenceWindowFunction_));

        // Save data?
        if (saveReference_)
        {
            if (!ExportDataNode::write(*referenceFQ_, std::format("{}-ReferenceData.q", name())))
                return NodeConstants::ProcessResult::Failed;
            if (!ExportDataNode::write(referenceGR_, std::format("{}-ReferenceData.r", name())))
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
    if (referenceFTQMax)
        ftQMax = referenceFTQMax.value();
    else if (referenceFQ_)
    {
        // Take FT max Q limit from reference data
        ftQMax = referenceFQ_->xAxis().back();
    }
    else
        ftQMax = weightedSQ_->total().xAxis().back();
    Filters::trim(representativeGR_, referenceFTQMin.value_or(0.0), ftQMax);
    auto rMin = weightedGR_->total().xAxis().front();
    auto rMax = weightedGR_->total().xAxis().back();
    WindowFunction window(referenceWindowFunction_);
    Fourier::sineFT(representativeGR_, 1.0 / (2.0 * M_PI * M_PI * unweightedGR_->effectiveDensity()), rMin, 0.05, rMax, window);

    // Save data if requested
    if (saveRepresentativeGR_)
    {
        if (!ExportDataNode::write(representativeGR_, std::format("{}-weighted-total.gr.broad", name())))
            return NodeConstants::ProcessResult::Failed;
    }

    return NodeConstants::ProcessResult::Success;
}
