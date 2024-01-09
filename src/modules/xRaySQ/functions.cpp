// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/xRayWeights.h"
#include "modules/gr/gr.h"
#include "modules/xRaySQ/xRaySQ.h"

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
