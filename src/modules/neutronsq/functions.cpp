// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesinfo.h"
#include "modules/neutronsq/neutronsq.h"

// Calculate weighted g(r) from supplied unweighted g(r) and neutron weights
bool NeutronSQModule::calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, NeutronWeights &weights,
                                          NeutronSQModule::NormalisationType normalisation)
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
            weightedgr.boundPartial(typeI, typeJ).values() *= intraWeight;

            // Unbound partial (multiplied by the full weight)
            weightedgr.unboundPartial(typeI, typeJ).copyArrays(unweightedgr.unboundPartial(typeI, typeJ));
            weightedgr.unboundPartial(typeI, typeJ).values() -= 1.0;
            weightedgr.unboundPartial(typeI, typeJ).values() *= weight;

            // Full partial, summing bound and unbound terms
            weightedgr.partial(typeI, typeJ).copyArrays(weightedgr.unboundPartial(typeI, typeJ));
            weightedgr.partial(typeI, typeJ) += weightedgr.boundPartial(typeI, typeJ);
        }
    }

    // Calculate and normalise total to form factor if requested
    weightedgr.formTotal(false);
    if (normalisation == NeutronSQModule::AverageOfSquaresNormalisation)
        weightedgr.total().values() /= weights.boundCoherentAverageOfSquares();
    else if (normalisation == NeutronSQModule::SquareOfAverageNormalisation)
        weightedgr.total().values() /= weights.boundCoherentSquareOfAverage();

    return true;
}

// Calculate weighted S(Q) from supplied unweighted S(Q) and neutron weights
bool NeutronSQModule::calculateWeightedSQ(const PartialSet &unweightedsq, PartialSet &weightedsq, NeutronWeights &weights,
                                          NeutronSQModule::NormalisationType normalisation)
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
            weightedsq.boundPartial(typeI, typeJ).values() *= boundWeight;

            // Unbound partial (multiplied by the full weight)
            weightedsq.unboundPartial(typeI, typeJ).copyArrays(unweightedsq.unboundPartial(typeI, typeJ));
            weightedsq.unboundPartial(typeI, typeJ).values() *= weight;

            // Full partial (sum of bound and unbound terms)
            weightedsq.partial(typeI, typeJ).copyArrays(weightedsq.unboundPartial(typeI, typeJ));
            weightedsq.partial(typeI, typeJ) += weightedsq.boundPartial(typeI, typeJ);
        }
    }

    // Calculate and normalise total to form factor if requested
    weightedsq.formTotal(false);
    if (normalisation == NeutronSQModule::AverageOfSquaresNormalisation)
        weightedsq.total().values() /= weights.boundCoherentAverageOfSquares();
    else if (normalisation == NeutronSQModule::SquareOfAverageNormalisation)
        weightedsq.total().values() /= weights.boundCoherentSquareOfAverage();

    return true;
}

// Calculate neutron weights summed over target Configurations
bool NeutronSQModule::calculateSummedWeights(NeutronWeights &summedWeights) const
{
    summedWeights.clear();

    // Loop over Configurations
    for (Configuration *cfg : targetConfigurations_)
    {
        // Loop over Species used in this Configuration and find its entry in the defined Isotopologues for the Module
        ListIterator<SpeciesInfo> speciesInfoIterator(cfg->usedSpecies());
        while (SpeciesInfo *spInfo = speciesInfoIterator.iterate())
        {
            // Find the Isotopologues for the Configuration/Species, if they have been defined
            auto data = isotopologues_.getIsotopologues(cfg, spInfo->species());

            // Use the natural isotopologue if a species in the Configuration is not covered by at least one
            // explicit Isotopologue definition
            if (!data)
            {
                Messenger::print("Isotopologue specification for Species '{}' in Configuration '{}' is "
                                 "missing, so the natural isotopologue will be used.\n",
                                 spInfo->species()->name(), cfg->name());

                Species *sp = spInfo->species();
                summedWeights.addIsotopologue(sp, spInfo->population(), sp->naturalIsotopologue(), 1.0);
            }
            else
            {
                const Isotopologues &topes = *data;

                // Add defined isotopologues, in the relative isotopic proportions defined, to the weights.
                for (auto isoWeight : topes.constMix())
                    summedWeights.addIsotopologue(spInfo->species(), spInfo->population(), isoWeight.isotopologue(),
                                                  isoWeight.weight());
            }
        }
    }

    // Finalise the Weights
    summedWeights.createFromIsotopologues(exchangeableTypes_);

    return true;
}
