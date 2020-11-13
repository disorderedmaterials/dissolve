// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesinfo.h"
#include "classes/xrayweights.h"
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
            printf("TODO: NEED TO WEIGHT g(r) PROPERLY.\n");
            double weight = weights.weight(typeI, typeJ, 0);

            // Bound (intramolecular) partial (multiplied by the bound term weight)
            weightedgr.boundPartial(typeI, typeJ).copyArrays(unweightedgr.boundPartial(typeI, typeJ));
            weightedgr.boundPartial(typeI, typeJ).values() *= weight;

            // Unbound partial (multiplied by the full weight)
            weightedgr.unboundPartial(typeI, typeJ).copyArrays(unweightedgr.unboundPartial(typeI, typeJ));
            weightedgr.unboundPartial(typeI, typeJ).values() -= 1.0;
            weightedgr.unboundPartial(typeI, typeJ).values() *= weight;

            // Full partial, summing bound and unbound terms
            weightedgr.partial(typeI, typeJ).copyArrays(weightedgr.unboundPartial(typeI, typeJ));
            weightedgr.partial(typeI, typeJ) += weightedgr.boundPartial(typeI, typeJ);
        }
    }

    // Form total G(r)
    weightedgr.formTotal(false);
    printf("TODO: NEED TO WEIGHT TOTAL G(r) PROPERLY.\n");
    // 	if (normalisation == StructureFactors::AverageOfSquaresNormalisation) weightedgr.total().values() /=
    // weights.boundCoherentAverageOfSquares(); 	else if (normalisation == StructureFactors::SquareOfAverageNormalisation)
    // weightedgr.total().values() /= weights.boundCoherentSquareOfAverage();

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
            Array<double> qWeights = weights.weight(typeI, typeJ, unweightedsq.boundPartial(typeI, typeJ).constXAxis());

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
        Array<double> bbar = weights.boundCoherentSquareOfAverage(unweightedsq.boundPartial(0, 0).constXAxis());
        for (auto n = 0; n < bbar.nItems(); ++n)
            weightedsq.total().value(n) /= bbar[n];
    }
    else if (normalisation == StructureFactors::AverageOfSquaresNormalisation)
    {
        Array<double> bbar = weights.boundCoherentAverageOfSquares(unweightedsq.boundPartial(0, 0).constXAxis());
        for (auto n = 0; n < bbar.nItems(); ++n)
            weightedsq.total().value(n) /= bbar[n];
    }

    return true;
}

// Calculate Weights matrix summed over target Configurations
bool XRaySQModule::calculateSummedWeights(XRayWeights &summedWeights, XRayFormFactors::XRayFormFactorData formFactors) const
{
    summedWeights.clear();

    // Loop over Configurations
    for (Configuration *cfg : targetConfigurations_)
    {
        // Loop over Species used in this Configuration and add its natural isotopologue (we only care about the elements, not
        // the isotopic make-up)
        ListIterator<SpeciesInfo> speciesInfoIterator(cfg->usedSpecies());
        while (SpeciesInfo *spInfo = speciesInfoIterator.iterate())
        {
            Species *sp = spInfo->species();
            summedWeights.addSpecies(sp, spInfo->population());
        }
    }

    // Finalise the weights
    summedWeights.finalise(formFactors);

    return true;
}
