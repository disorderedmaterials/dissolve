// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/isotopologueSet.h"
#include "classes/species.h"
#include "nodes/neutronSQ/neutronSQ.h"

// Calculate weighted g(r)
bool NeutronSQNode::calculateWeightedGR()
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedGR_->atomTypeMix(),
                            [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2)
                            {
                                auto key = DoubleKeyedMapKey{atd1.atomTypeName(), atd2.atomTypeName()};
                                double weight = weights_.weight(typeI, typeJ);
                                double intraWeight = weights_.intramolecularWeight(typeI, typeJ);

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
        auto norm = normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation ? weights_.boundCoherentAverageOfSquares()
                                                                                    : weights_.boundCoherentSquareOfAverage();

        weightedGR_->total() /= norm;
        weightedGR_->boundTotal() /= norm;
        weightedGR_->unboundTotal() /= norm;
    }

    return true;
}

// Calculate weighted S(Q)
bool NeutronSQNode::calculateWeightedSQ()
{
    dissolve::for_each_pair(ParallelPolicies::seq, unweightedSQ_->atomTypeMix(),
                            [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2)
                            {
                                auto key = DoubleKeyedMapKey{atd1.atomTypeName(), atd2.atomTypeName()};
                                // Weight bound and unbound S(Q) and sum into full partial
                                double weight = weights_.weight(typeI, typeJ);
                                double boundWeight = weights_.intramolecularWeight(typeI, typeJ);

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
        auto norm = normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation ? weights_.boundCoherentAverageOfSquares()
                                                                                    : weights_.boundCoherentSquareOfAverage();

        weightedSQ_->total() /= norm;
        weightedSQ_->boundTotal() /= norm;
        weightedSQ_->unboundTotal() /= norm;
    }

    return true;
}

// Calculate neutron weights matrix
void NeutronSQNode::calculateWeights(const std::map<const Species *, double> &realSpeciesPopulations)
{
    // Create a set of named Isotopologues to use
    IsotopologueSet topes;
    for (const auto &[species, _] : realSpeciesPopulations)
    {
        for (const auto &isotopologue : species->isotopologues())
        {
            auto iso = isotopologue.get();
            auto it = namedWeights_.find(iso->name());
            if (it != namedWeights_.end())
                topes.add(iso, it->second);
        }
    }

    weights_.clear();

    weights_.create(realSpeciesPopulations, topes, exchangeable_);
}
