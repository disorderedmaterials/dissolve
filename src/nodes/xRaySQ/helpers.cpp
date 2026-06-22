// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#define _USE_MATH_DEFINES
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/xRayWeights.h"
#include "math/filters.h"
#include "math/ft.h"
#include "nodes/edge.h"
#include "nodes/sq.h"
#include "nodes/xRaySQ/xRaySQ.h"

bool XRaySQNode::setReferenceData()
{
    // Normalise reference data to be consistent with the calculated data
    if (referenceNormalisedTo_ != normaliseTo_)
    {
        auto bBarSquareOfAverage = weights_.boundCoherentSquareOfAverage(referenceFQ_->xAxis());
        auto bBarAverageOfSquares = weights_.boundCoherentAverageOfSquares(referenceFQ_->xAxis());
        std::vector<double> factors;

        // Set up the multiplication factors
        switch (referenceNormalisedTo_)
        {
            case (StructureFactors::NoNormalisation):
                factors =
                    normaliseTo_ == StructureFactors::SquareOfAverageNormalisation ? bBarSquareOfAverage : bBarAverageOfSquares;
                std::transform(factors.begin(), factors.end(), factors.begin(), [](const auto factor) { return 1.0 / factor; });
                break;
            case (StructureFactors::SquareOfAverageNormalisation):
                factors = bBarSquareOfAverage;
                if (normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation)
                    std::transform(factors.begin(), factors.end(), bBarAverageOfSquares.begin(), factors.begin(),
                                   std::divides<>());
                break;
            case (StructureFactors::AverageOfSquaresNormalisation):
                factors = bBarAverageOfSquares;
                if (normaliseTo_ == StructureFactors::SquareOfAverageNormalisation)
                    std::transform(factors.begin(), factors.end(), bBarSquareOfAverage.begin(), factors.begin(),
                                   std::divides<>());
                break;
            default:
                Messenger::exception("Unhandled StructureFactor::NormalisationType ({}).\n",
                                     StructureFactors::normalisationTypes().keyword(referenceNormalisedTo_));
        }

        // Apply normalisation factors to the data
        std::transform(referenceFQ_->values().begin(), referenceFQ_->values().end(), factors.begin(),
                       referenceFQ_->values().begin(), std::multiplies<>());
    }

    // Get Q-range and window function to use for transformation of F(Q) to G(r)
    auto ftQMin = referenceFTQMin_.value_or(0.0).asDouble();
    auto ftQMax = referenceFTQMax_.value_or(referenceFQ_->xAxis().back() + 1.0).asDouble();
    if (referenceWindowFunction_ == WindowFunction::Form::None)
        Node::message("[SETUP {}] No window function will be applied in Fourier transform of reference data to g(r).", name());
    else
        Node::message("[SETUP {}] Window function to be applied in Fourier transform of reference data is {}.", name(),
                      WindowFunction::forms().keyword(referenceWindowFunction_));

    referenceGR_ = *referenceFQ_;
    Filters::trim(referenceGR_, ftQMin, ftQMax);
    auto rho = unweightedGR_->effectiveDensity();
    if (rho)
        Node::message(
            "[SETUP {}] Effective atomic density used in Fourier transform of reference data is {} atoms/Angstrom3.\n", name(),
            rho);
    else
        Node::message("[SETUP {}] Effective atomic density used in Fourier transform of reference data not yet "
                      "available, so a default of 0.1 atoms/Angstrom3 used.\n",
                      name());
    Fourier::sineFT(referenceGR_, 1.0 / (2.0 * M_PI * M_PI * rho), referenceFTDeltaR_.asDouble(), referenceFTDeltaR_.asDouble(),
                    30.0, WindowFunction(referenceWindowFunction_));

    return true;
}

// Return xRay weights
const XRayWeights &XRaySQNode::weights() const { return weights_; }

// Calculate weighted g(r) from supplied unweighted g(r) and Weights
bool XRaySQNode::calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, const XRayWeights &weights,
                                     StructureFactors::NormalisationType normalisation)
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedgr.atomTypeFractions(),
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};

                                auto weight = weights.weight(popI.first, popJ.first, 0.0);

                                // Bound (intramolecular) partial (multiplied by the bound term weight)
                                weightedgr.boundPartials().get(key).copyArrays(unweightedgr.boundPartials().get(key));
                                weightedgr.boundPartials().get(key) *= weight;

                                // Unbound partial (multiplied by the full weight)
                                weightedgr.unboundPartials().get(key).copyArrays(unweightedgr.unboundPartials().get(key));
                                weightedgr.unboundPartials().get(key) -= 1.0;
                                weightedgr.unboundPartials().get(key) *= weight;

                                // Full partial, summing bound and unbound terms
                                weightedgr.partials().get(key).copyArrays(weightedgr.unboundPartials().get(key));
                                weightedgr.partials().get(key) += weightedgr.boundPartials().get(key);
                            });

    // Form total G(r)
    weightedgr.formTotals(false);

    // Normalise to Q=0.0 form factor if requested
    if (normalisation != StructureFactors::NoNormalisation)
    {
        auto norm = normalisation == StructureFactors::AverageOfSquaresNormalisation
                        ? weights.boundCoherentAverageOfSquares(0.0)
                        : weights.boundCoherentSquareOfAverage(0.0);

        weightedgr.total() /= norm;
        weightedgr.boundTotal() /= norm;
        weightedgr.unboundTotal() /= norm;
    }

    return true;
}

// Calculate weighted S(Q) from supplied unweighted S(Q) and Weights
bool XRaySQNode::calculateWeightedSQ(const PartialSet &unweightedsq, PartialSet &weightedsq, const XRayWeights &weights,
                                     StructureFactors::NormalisationType normalisation)
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedsq.atomTypeFractions(),
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};

                                // Weight bound and unbound S(Q) and sum into full partial
                                auto qWeights =
                                    weights.weight(popI.first, popJ.first, unweightedsq.boundPartials().get(key).xAxis());

                                // Bound (intramolecular) and unbound partials
                                weightedsq.boundPartials().get(key).copyArrays(unweightedsq.boundPartials().get(key));
                                weightedsq.boundPartials().get(key) *= qWeights;
                                weightedsq.unboundPartials().get(key).copyArrays(unweightedsq.unboundPartials().get(key));
                                weightedsq.unboundPartials().get(key) *= qWeights;

                                // Full partial (sum of bound and unbound terms)
                                weightedsq.partials().get(key).copyArrays(weightedsq.unboundPartials().get(key));
                                weightedsq.partials().get(key) += weightedsq.boundPartials().get(key);
                            });

    // Form total structure factor
    weightedsq.formTotals(false);

    // Apply normalisation to all totals
    if (normalisation != StructureFactors::NoNormalisation)
    {
        auto bbar = normalisation == StructureFactors::SquareOfAverageNormalisation
                        ? weights.boundCoherentSquareOfAverage(weightedsq.total().xAxis())
                        : weights.boundCoherentAverageOfSquares(weightedsq.total().xAxis());

        std::transform(weightedsq.total().values().begin(), weightedsq.total().values().end(), bbar.begin(),
                       weightedsq.total().values().begin(), std::divides<>());
        std::transform(weightedsq.boundTotal().values().begin(), weightedsq.boundTotal().values().end(), bbar.begin(),
                       weightedsq.boundTotal().values().begin(), std::divides<>());
        std::transform(weightedsq.unboundTotal().values().begin(), weightedsq.unboundTotal().values().end(), bbar.begin(),
                       weightedsq.unboundTotal().values().begin(), std::divides<>());
    }

    return true;
}

// Returns the unweighted SQ
const PartialSet *XRaySQNode::unweightedSQ() const { return unweightedSQ_; }

// Returns the unweighted GR
const PartialSet *XRaySQNode::unweightedGR() const { return unweightedGR_; }

// Returns the source configuration, belonging to the input SQ node
const Configuration *XRaySQNode::sourceConfiguration()
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