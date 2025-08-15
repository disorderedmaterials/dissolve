// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/isotopologueSet.h"
#include "classes/species.h"
#include "modules/gr/gr.h"
#include "modules/neutronSQ/neutronSQ.h"

// Calculate weighted g(r) from supplied unweighted g(r) and neutron weights
bool NeutronSQModule::calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, NeutronWeights &weights,
                                          StructureFactors::NormalisationType normalisation)
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedgr.atomTypeFractions(),
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};

                                double weight = weights.weight(indexI, indexJ);
                                double intraWeight = weights.intramolecularWeight(indexI, indexJ);

                                // Bound (intramolecular) partial (multiplied by the bound term weight)
                                weightedgr.boundPartials().get(key).copyArrays(unweightedgr.boundPartials().get(key));
                                weightedgr.boundPartials().get(key) *= intraWeight;

                                // Unbound partial (multiplied by the full weight)
                                weightedgr.unboundPartials().get(key).copyArrays(unweightedgr.unboundPartials().get(key));
                                weightedgr.unboundPartials().get(key) -= 1.0;
                                weightedgr.unboundPartials().get(key) *= weight;

                                // Full partial, summing bound and unbound terms
                                weightedgr.partials().get(key).copyArrays(weightedgr.unboundPartials().get(key));
                                weightedgr.partials().get(key) += weightedgr.boundPartials().get(key);
                            });

    // Calculate and normalise total to form factor if requested
    weightedgr.formTotals(false);

    // Normalise to Q=0.0 form factor if requested
    if (normalisation != StructureFactors::NoNormalisation)
    {
        auto norm = normalisation == StructureFactors::AverageOfSquaresNormalisation ? weights.boundCoherentAverageOfSquares()
                                                                                     : weights.boundCoherentSquareOfAverage();

        weightedgr.total() /= norm;
        weightedgr.boundTotal() /= norm;
        weightedgr.unboundTotal() /= norm;
    }

    return true;
}

// Calculate weighted S(Q) from supplied unweighted S(Q) and neutron weights
bool NeutronSQModule::calculateWeightedSQ(const PartialSet &unweightedsq, PartialSet &weightedsq, NeutronWeights &weights,
                                          StructureFactors::NormalisationType normalisation)
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedsq.atomTypeFractions(),
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};

                                // Weight bound and unbound S(Q) and sum into full partial
                                double weight = weights.weight(indexI, indexJ);
                                double boundWeight = weights.intramolecularWeight(indexI, indexJ);

                                // Bound (intramolecular) partial (multiplied by the bound term weight)
                                weightedsq.boundPartials().get(key).copyArrays(unweightedsq.boundPartials().get(key));
                                weightedsq.boundPartials().get(key) *= boundWeight;

                                // Unbound partial (multiplied by the full weight)
                                weightedsq.unboundPartials().get(key).copyArrays(unweightedsq.unboundPartials().get(key));
                                weightedsq.unboundPartials().get(key) *= weight;

                                // Full partial (sum of bound and unbound terms)
                                weightedsq.partials().get(key).copyArrays(weightedsq.unboundPartials().get(key));
                                weightedsq.partials().get(key) += weightedsq.boundPartials().get(key);
                            });

    // Form total structure factor
    weightedsq.formTotals(false);

    // Apply normalisation to all totals
    if (normalisation != StructureFactors::NoNormalisation)
    {
        auto norm = normalisation == StructureFactors::AverageOfSquaresNormalisation ? weights.boundCoherentAverageOfSquares()
                                                                                     : weights.boundCoherentSquareOfAverage();

        weightedsq.total() /= norm;
        weightedsq.boundTotal() /= norm;
        weightedsq.unboundTotal() /= norm;
    }

    return true;
}

// Calculate neutron weights for relevant Configuration targets
void NeutronSQModule::calculateWeights(const GRModule *rdfModule, NeutronWeights &weights) const
{
    // Clear weights and get species populations from GRModule
    weights.clear();
    auto populations = rdfModule->speciesPopulations();

    for (auto &[sp, pop] : populations)
    {
        // Find the defined Isotopologue for this Species - if it doesn't exist, use the Natural one
        auto isoRef = isotopologueSet_.getIsotopologues(sp);
        if (isoRef)
        {
            const Isotopologues &topes = *isoRef;
            for (const auto &[iso, weight] : topes.mix())
                weights.addIsotopologue(sp, pop, iso, weight);
        }
        else
            weights.addIsotopologue(sp, pop, sp->naturalIsotopologue(), 1.0);
    }

    weights.createFromIsotopologues(exchangeable_);
}
