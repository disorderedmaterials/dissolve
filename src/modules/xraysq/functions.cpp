// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesinfo.h"
#include "classes/xrayweights.h"
#include "modules/rdf/rdf.h"
#include "modules/xraysq/xraysq.h"

// Calculate weighted g(r) from supplied unweighted g(r) and Weights
bool XRaySQModule::calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, const XRayWeights &weights,
                                       StructureFactors::NormalisationType normalisation)
{
    int typeI, typeJ;
    for (typeI = 0; typeI < unweightedgr.nAtomTypes(); ++typeI)
    {
        for (typeJ = typeI; typeJ < unweightedgr.nAtomTypes(); ++typeJ)
        {
            auto weight = weights.weight(typeI, typeJ, 0.0);

            // Bound (intramolecular) partial (multiplied by the bound term weight)
            weightedgr.boundPartial(typeI, typeJ).copyArrays(unweightedgr.boundPartial(typeI, typeJ));
            weightedgr.boundPartial(typeI, typeJ) *= weight;

            // Unbound partial (multiplied by the full weight)
            weightedgr.unboundPartial(typeI, typeJ).copyArrays(unweightedgr.unboundPartial(typeI, typeJ));
            weightedgr.unboundPartial(typeI, typeJ) -= 1.0;
            weightedgr.unboundPartial(typeI, typeJ) *= weight;

            // Full partial, summing bound and unbound terms
            weightedgr.partial(typeI, typeJ).copyArrays(weightedgr.unboundPartial(typeI, typeJ));
            weightedgr.partial(typeI, typeJ) += weightedgr.boundPartial(typeI, typeJ);
        }
    }

    // Form total G(r)
    weightedgr.formTotal(false);
    if (normalisation == StructureFactors::AverageOfSquaresNormalisation)
        weightedgr.total() /= weights.boundCoherentAverageOfSquares(0.0);
    else if (normalisation == StructureFactors::SquareOfAverageNormalisation)
        weightedgr.total() /= weights.boundCoherentSquareOfAverage(0.0);

    return true;
}

// Calculate weighted S(Q) from supplied unweighted S(Q) and Weights
bool XRaySQModule::calculateWeightedSQ(const PartialSet &unweightedsq, PartialSet &weightedsq, const XRayWeights &weights,
                                       StructureFactors::NormalisationType normalisation)
{
    int typeI, typeJ;
    for (typeI = 0; typeI < unweightedsq.nAtomTypes(); ++typeI)
    {
        for (typeJ = typeI; typeJ < unweightedsq.nAtomTypes(); ++typeJ)
        {
            // Weight bound and unbound S(Q) and sum into full partial
            auto qWeights = weights.weight(typeI, typeJ, unweightedsq.boundPartial(typeI, typeJ).xAxis());

            // Bound (intramolecular) and unbound partials
            weightedsq.boundPartial(typeI, typeJ).copyArrays(unweightedsq.boundPartial(typeI, typeJ));
            weightedsq.boundPartial(typeI, typeJ) *= qWeights;
            weightedsq.unboundPartial(typeI, typeJ).copyArrays(unweightedsq.unboundPartial(typeI, typeJ));
            weightedsq.unboundPartial(typeI, typeJ) *= qWeights;

            // Full partial (sum of bound and unbound terms)
            weightedsq.partial(typeI, typeJ).copyArrays(weightedsq.unboundPartial(typeI, typeJ));
            weightedsq.partial(typeI, typeJ) += weightedsq.boundPartial(typeI, typeJ);
        }
    }

    // Form total structure factor
    weightedsq.formTotal(false);
    if (normalisation == StructureFactors::SquareOfAverageNormalisation)
    {
        auto bbar = weights.boundCoherentSquareOfAverage(unweightedsq.boundPartial(0, 0).xAxis());
        for (auto n = 0; n < bbar.size(); ++n)
            weightedsq.total().value(n) /= bbar[n];
    }
    else if (normalisation == StructureFactors::AverageOfSquaresNormalisation)
    {
        auto bbar = weights.boundCoherentAverageOfSquares(unweightedsq.boundPartial(0, 0).xAxis());
        for (auto n = 0; n < bbar.size(); ++n)
            weightedsq.total().value(n) /= bbar[n];
    }

    return true;
}

// Calculate xray weights for relevant Configuration targets
void XRaySQModule::calculateWeights(const RDFModule *rdfModule, XRayWeights &weights,
                                    XRayFormFactors::XRayFormFactorData formFactors) const
{
    // Clear weights and get species populations from RDFModule
    weights.clear();
    auto populations = rdfModule->speciesPopulations();

    for (auto speciesPop : populations)
        weights.addSpecies(speciesPop.first, speciesPop.second);

    weights.finalise(formFactors);
}
