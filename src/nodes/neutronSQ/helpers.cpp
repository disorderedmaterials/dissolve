// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/isotopologueSet.h"
#include "classes/species.h"
#include "nodes/neutronSQ/neutronSQ.h"

// Calculate weighted g(r)
bool NeutronSQNode::calculateWeightedGR()
{
    int typeI, typeJ;
    for (typeI = 0; typeI < unweightedGR_->nAtomTypes(); ++typeI)
    {
        for (typeJ = typeI; typeJ < unweightedGR_->nAtomTypes(); ++typeJ)
        {
            double weight = weights_.weight(typeI, typeJ);
            double intraWeight = weights_.intramolecularWeight(typeI, typeJ);

            // Bound (intramolecular) partial (multiplied by the bound term weight)
            weightedGR_->boundPartial(typeI, typeJ).copyArrays(unweightedGR_->boundPartial(typeI, typeJ));
            weightedGR_->boundPartial(typeI, typeJ) *= intraWeight;

            // Unbound partial (multiplied by the full weight)
            weightedGR_->unboundPartial(typeI, typeJ).copyArrays(unweightedGR_->unboundPartial(typeI, typeJ));
            weightedGR_->unboundPartial(typeI, typeJ) -= 1.0;
            weightedGR_->unboundPartial(typeI, typeJ) *= weight;

            // Full partial, summing bound and unbound terms
            weightedGR_->partial(typeI, typeJ).copyArrays(weightedGR_->unboundPartial(typeI, typeJ));
            weightedGR_->partial(typeI, typeJ) += weightedGR_->boundPartial(typeI, typeJ);
        }
    }

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
    int typeI, typeJ;
    for (typeI = 0; typeI < unweightedSQ_->nAtomTypes(); ++typeI)
    {
        for (typeJ = typeI; typeJ < unweightedSQ_->nAtomTypes(); ++typeJ)
        {
            // Weight bound and unbound S(Q) and sum into full partial
            double weight = weights_.weight(typeI, typeJ);
            double boundWeight = weights_.intramolecularWeight(typeI, typeJ);

            // Bound (intramolecular) partial (multiplied by the bound term weight)
            weightedSQ_->boundPartial(typeI, typeJ).copyArrays(unweightedSQ_->boundPartial(typeI, typeJ));
            weightedSQ_->boundPartial(typeI, typeJ) *= boundWeight;

            // Unbound partial (multiplied by the full weight)
            weightedSQ_->unboundPartial(typeI, typeJ).copyArrays(unweightedSQ_->unboundPartial(typeI, typeJ));
            weightedSQ_->unboundPartial(typeI, typeJ) *= weight;

            // Full partial (sum of bound and unbound terms)
            weightedSQ_->partial(typeI, typeJ).copyArrays(weightedSQ_->unboundPartial(typeI, typeJ));
            weightedSQ_->partial(typeI, typeJ) += weightedSQ_->boundPartial(typeI, typeJ);
        }
    }

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
