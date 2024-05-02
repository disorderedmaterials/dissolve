// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/isotopologueSet.h"
#include "classes/species.h"
#include "modules/gr/gr.h"
#include "modules/neutronSQ/neutronSQ.h"

// Calculate weighted g(r) from supplied unweighted g(r) and neutron weights
bool NeutronSQModule::calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, NeutronWeights &weights,
                                          StructureFactors::NormalisationType normalisation)
{
    int typeI, typeJ;
    for (typeI = 0; typeI < unweightedgr.nAtomTypes(); ++typeI)
    {
        for (typeJ = typeI; typeJ < unweightedgr.nAtomTypes(); ++typeJ)
        {
            double weight = weights.weight(typeI, typeJ);
            double intraWeight = weights.intramolecularWeight(typeI, typeJ);

            // Bound (intramolecular) partial (multiplied by the bound term weight)
            weightedgr.boundPartial(typeI, typeJ).copyArrays(unweightedgr.boundPartial(typeI, typeJ));
            weightedgr.boundPartial(typeI, typeJ) *= intraWeight;

            // Unbound partial (multiplied by the full weight)
            weightedgr.unboundPartial(typeI, typeJ).copyArrays(unweightedgr.unboundPartial(typeI, typeJ));
            weightedgr.unboundPartial(typeI, typeJ) -= 1.0;
            weightedgr.unboundPartial(typeI, typeJ) *= weight;

            // Full partial, summing bound and unbound terms
            weightedgr.partial(typeI, typeJ).copyArrays(weightedgr.unboundPartial(typeI, typeJ));
            weightedgr.partial(typeI, typeJ) += weightedgr.boundPartial(typeI, typeJ);
        }
    }

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
    int typeI, typeJ;
    for (typeI = 0; typeI < unweightedsq.nAtomTypes(); ++typeI)
    {
        for (typeJ = typeI; typeJ < unweightedsq.nAtomTypes(); ++typeJ)
        {
            // Weight bound and unbound S(Q) and sum into full partial
            double weight = weights.weight(typeI, typeJ);
            double boundWeight = weights.intramolecularWeight(typeI, typeJ);

            // Bound (intramolecular) partial (multiplied by the bound term weight)
            weightedsq.boundPartial(typeI, typeJ).copyArrays(unweightedsq.boundPartial(typeI, typeJ));
            weightedsq.boundPartial(typeI, typeJ) *= boundWeight;

            // Unbound partial (multiplied by the full weight)
            weightedsq.unboundPartial(typeI, typeJ).copyArrays(unweightedsq.unboundPartial(typeI, typeJ));
            weightedsq.unboundPartial(typeI, typeJ) *= weight;

            // Full partial (sum of bound and unbound terms)
            weightedsq.partial(typeI, typeJ).copyArrays(weightedsq.unboundPartial(typeI, typeJ));
            weightedsq.partial(typeI, typeJ) += weightedsq.boundPartial(typeI, typeJ);
        }
    }

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
            for (const auto &isoWeight : topes.mix())
                weights.addIsotopologue(sp, pop, isoWeight.isotopologue(), isoWeight.weight());
        }
        else
            weights.addIsotopologue(sp, pop, sp->naturalIsotopologue(), 1.0);
    }

    weights.createFromIsotopologues(exchangeable_);
}
