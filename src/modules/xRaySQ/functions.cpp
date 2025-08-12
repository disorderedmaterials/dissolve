// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/xRayWeights.h"
#include "modules/gr/gr.h"
#include "modules/xRaySQ/xRaySQ.h"

// Calculate weighted g(r) from supplied unweighted g(r) and Weights
bool XRaySQModule::calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, const XRayWeights &weights,
                                       StructureFactors::NormalisationType normalisation)
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedgr.atomTypeFractions(),
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};

                                auto weight = weights.weight(indexI, indexJ, 0.0);

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
bool XRaySQModule::calculateWeightedSQ(const PartialSet &unweightedsq, PartialSet &weightedsq, const XRayWeights &weights,
                                       StructureFactors::NormalisationType normalisation)
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedsq.atomTypeFractions(),
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};

                                // Weight bound and unbound S(Q) and sum into full partial
                                auto qWeights = weights.weight(indexI, indexJ, unweightedsq.boundPartials().get(key).xAxis());

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

// Calculate xray weights for relevant Configuration targets
void XRaySQModule::calculateWeights(const GRModule *rdfModule, XRayWeights &weights,
                                    XRayFormFactors::XRayFormFactorData formFactors) const
{
    // Clear weights and get species populations from GRModule
    weights.clear();
    auto populations = rdfModule->speciesPopulations();

    for (auto speciesPop : populations)
        weights.addSpecies(speciesPop.first, speciesPop.second);

    weights.finalise(formFactors);
}
