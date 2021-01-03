// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesinfo.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/rdf/rdf.h"

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
    weightedgr.formTotal(false);
    if (normalisation == StructureFactors::AverageOfSquaresNormalisation)
        weightedgr.total() /= weights.boundCoherentAverageOfSquares();
    else if (normalisation == StructureFactors::SquareOfAverageNormalisation)
        weightedgr.total() /= weights.boundCoherentSquareOfAverage();

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

    // Calculate and normalise total to form factor if requested
    weightedsq.formTotal(false);
    if (normalisation == StructureFactors::AverageOfSquaresNormalisation)
        weightedsq.total() /= weights.boundCoherentAverageOfSquares();
    else if (normalisation == StructureFactors::SquareOfAverageNormalisation)
        weightedsq.total() /= weights.boundCoherentSquareOfAverage();

    return true;
}

// Calculate neutron weights for relevant Configuration targets
void NeutronSQModule::calculateWeights(const RDFModule *rdfModule, NeutronWeights &weights) const
{
    // Clear weights and get species populations from RDFModule
    weights.clear();
    auto populations = rdfModule->speciesPopulations();

    for (auto speciesPop : populations)
    {
        // Find the defined Isotopologue for this Species - if it doesn't exist, use the Natural one
        auto isoRef = isotopologues_.getIsotopologues(speciesPop.first);
        if (isoRef)
        {
            const Isotopologues &topes = *isoRef;
            for (const auto &isoWeight : topes.mix())
                weights.addIsotopologue(speciesPop.first, speciesPop.second, isoWeight.isotopologue(), isoWeight.weight());
        }
        else
            weights.addIsotopologue(speciesPop.first, speciesPop.second, speciesPop.first->naturalIsotopologue(), 1.0);
    }
}
