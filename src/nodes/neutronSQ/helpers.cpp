// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/isotopologueSet.h"
#include "classes/species.h"
#include "nodes/neutronSQ/neutronSQ.h"

// Calculate weighted g(r)
bool NeutronSQNode::calculateWeightedGR()
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedGR_->atomTypeFractions(),
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};

                                auto weight = weights_.weights().get(key);
                                auto intraWeight = weights_.intramolecularWeights().get(key);

                                // Bound (intramolecular) partial (multiplied by the bound term weight)
                                weightedGR().boundPartials().get(key).copyArrays(unweightedGR_->boundPartials().get(key));
                                weightedGR().boundPartials().get(key) *= intraWeight;

                                // Unbound partial (multiplied by the full weight)
                                weightedGR().unboundPartials().get(key).copyArrays(unweightedGR_->unboundPartials().get(key));
                                weightedGR().unboundPartials().get(key) -= 1.0;
                                weightedGR().unboundPartials().get(key) *= weight;

                                // Full partial, summing bound and unbound terms
                                weightedGR().partials().get(key).copyArrays(weightedGR().unboundPartials().get(key));
                                weightedGR().partials().get(key) += weightedGR().boundPartials().get(key);
                            });

    // Calculate and normalise total to form factor if requested
    weightedGR().formTotals(false);

    // Normalise to Q=0.0 form factor if requested
    if (normaliseTo_ != StructureFactors::NoNormalisation)
    {
        auto norm = normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation ? weights_.boundCoherentAverageOfSquares()
                                                                                    : weights_.boundCoherentSquareOfAverage();

        weightedGR().total() /= norm;
        weightedGR().boundTotal() /= norm;
        weightedGR().unboundTotal() /= norm;
    }

    return true;
}

// Calculate weighted S(Q)
bool NeutronSQNode::calculateWeightedSQ()
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedSQ_->atomTypeFractions(),
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};

                                // Weight bound and unbound S(Q) and sum into full partial
                                auto weight = weights_.weights().get(key);
                                auto boundWeight = weights_.intramolecularWeights().get(key);

                                // Bound (intramolecular) partial (multiplied by the bound term weight)
                                weightedSQ().boundPartials().get(key).copyArrays(unweightedSQ_->boundPartials().get(key));
                                weightedSQ().boundPartials().get(key) *= boundWeight;

                                // Unbound partial (multiplied by the full weight)
                                weightedSQ().unboundPartials().get(key).copyArrays(unweightedSQ_->unboundPartials().get(key));
                                weightedSQ().unboundPartials().get(key) *= weight;

                                // Full partial (sum of bound and unbound terms)
                                weightedSQ().partials().get(key).copyArrays(weightedSQ().unboundPartials().get(key));
                                weightedSQ().partials().get(key) += weightedSQ().boundPartials().get(key);
                            });

    // Form total structure factor
    auto w = weightedSQ();
    w.formTotals(false);

    // Apply normalisation to all totals
    if (normaliseTo_ != StructureFactors::NoNormalisation)
    {
        auto norm = normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation ? weights_.boundCoherentAverageOfSquares()
                                                                                    : weights_.boundCoherentSquareOfAverage();

        weightedSQ().total() /= norm;
        weightedSQ().boundTotal() /= norm;
        weightedSQ().unboundTotal() /= norm;
    }

    return true;
}

// Calculate neutron weights matrix
void NeutronSQNode::calculateWeights(const KeyedVector<const Species *, double> &realSpeciesPopulations)
{
    weights_.clear();

    // Add simple isotopologues
    for (const auto &[species, _] : realSpeciesPopulations)
    {
        for (const auto &isotopologue : species->isotopologues())
        {
            auto iso = isotopologue.get();
            auto it = namedWeights_.find(iso->name());
            if (it != namedWeights_.end())
                weights_.addIsotopologue(species, 1.0, iso, it->second);
        }
    }

    weights_.createFromIsotopologues(exchangeable_);
}

// Return value of weighted SQ, emplacing if optional not initialised
PartialSet &NeutronSQNode::weightedSQ()
{
    // Set up the weighted SQ storage if needed
    if (!weightedSQ_)
    {
        weightedSQ_.emplace();
        weightedSQ_.value().initialise(*unweightedSQ_);
    }

    return *weightedSQ_;
}

// Return value of weighted GR, emplacing if optional not initialised
PartialSet &NeutronSQNode::weightedGR()
{
    // Set up weighted GR storage if we need it
    if (!weightedGR_)
    {
        weightedGR_.emplace();
        weightedGR_.value().initialise(*unweightedGR_);
    }

    return *weightedGR_;
}