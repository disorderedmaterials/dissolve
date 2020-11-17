// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesinfo.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/rdf/rdf.h"

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
            std::transform(
                weightedgr.boundPartial(typeI, typeJ).values().begin(), weightedgr.boundPartial(typeI, typeJ).values().end(),
                weightedgr.boundPartial(typeI, typeJ).values().begin(), [=](auto value) { return value * (intraWeight); });

            // Unbound partial (multiplied by the full weight)
            weightedgr.unboundPartial(typeI, typeJ).copyArrays(unweightedgr.unboundPartial(typeI, typeJ));
            std::transform(weightedgr.unboundPartial(typeI, typeJ).values().begin(),
                           weightedgr.unboundPartial(typeI, typeJ).values().end(),
                           weightedgr.unboundPartial(typeI, typeJ).values().begin(), [=](auto value) { return value - (1.0); });
            std::transform(weightedgr.unboundPartial(typeI, typeJ).values().begin(),
                           weightedgr.unboundPartial(typeI, typeJ).values().end(),
                           weightedgr.unboundPartial(typeI, typeJ).values().begin(),
                           [=](auto value) { return value * (weight); });

            // Full partial, summing bound and unbound terms
            weightedgr.partial(typeI, typeJ).copyArrays(weightedgr.unboundPartial(typeI, typeJ));
            weightedgr.partial(typeI, typeJ) += weightedgr.boundPartial(typeI, typeJ);
        }
    }

    // Calculate and normalise total to form factor if requested
    weightedgr.formTotal(false);
    if (normalisation == NeutronSQModule::AverageOfSquaresNormalisation)
        std::transform(weightedgr.total().values().begin(), weightedgr.total().values().end(),
                       weightedgr.total().values().begin(),
                       [=](auto value) { return value / (weights.boundCoherentAverageOfSquares()); });
    else if (normalisation == NeutronSQModule::SquareOfAverageNormalisation)
        std::transform(weightedgr.total().values().begin(), weightedgr.total().values().end(),
                       weightedgr.total().values().begin(),
                       [=](auto value) { return value / (weights.boundCoherentSquareOfAverage()); });

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
            std::transform(
                weightedsq.boundPartial(typeI, typeJ).values().begin(), weightedsq.boundPartial(typeI, typeJ).values().end(),
                weightedsq.boundPartial(typeI, typeJ).values().begin(), [=](auto value) { return value * (boundWeight); });

            // Unbound partial (multiplied by the full weight)
            weightedsq.unboundPartial(typeI, typeJ).copyArrays(unweightedsq.unboundPartial(typeI, typeJ));
            std::transform(weightedsq.unboundPartial(typeI, typeJ).values().begin(),
                           weightedsq.unboundPartial(typeI, typeJ).values().end(),
                           weightedsq.unboundPartial(typeI, typeJ).values().begin(),
                           [=](auto value) { return value * (weight); });

            // Full partial (sum of bound and unbound terms)
            weightedsq.partial(typeI, typeJ).copyArrays(weightedsq.unboundPartial(typeI, typeJ));
            weightedsq.partial(typeI, typeJ) += weightedsq.boundPartial(typeI, typeJ);
        }
    }

    // Calculate and normalise total to form factor if requested
    weightedsq.formTotal(false);
    if (normalisation == NeutronSQModule::AverageOfSquaresNormalisation)
        std::transform(weightedsq.total().values().begin(), weightedsq.total().values().end(),
                       weightedsq.total().values().begin(),
                       [=](auto value) { return value / (weights.boundCoherentAverageOfSquares()); });
    else if (normalisation == NeutronSQModule::SquareOfAverageNormalisation)
        std::transform(weightedsq.total().values().begin(), weightedsq.total().values().end(),
                       weightedsq.total().values().begin(),
                       [=](auto value) { return value / (weights.boundCoherentSquareOfAverage()); });

    return true;
}

// Calculate neutron weights for relevant Configuration targets
bool NeutronSQModule::calculateWeights(const RDFModule *rdfModule, NeutronWeights &weights) const
{
    // Construct weights matrix based on Isotopologue specifications and Species populations in the underlying configurations
    // TODO This info would be better calculated by the RDFModule and stored there / associated to it (#400)
    // TODO Following code should exist locally in RDFModule::sumUnweightedGR() when suitable class storage is available.
    weights.clear();
    for (auto *cfg : rdfModule->targetConfigurations())
    {
        // TODO Assume weight of 1.0 per configuration now, until #398/#400 are addressed.
        const auto CFGWEIGHT = 1.0;

        ListIterator<SpeciesInfo> spInfoIterator(cfg->usedSpecies());
        while (auto *spInfo = spInfoIterator.iterate())
        {
            auto *sp = spInfo->species();

            // Find the defined Isotopologue for this Species - if it doesn't exist, use the Natural one
            auto isoRef = isotopologues_.getIsotopologues(sp);
            if (isoRef)
            {
                const Isotopologues &topes = *isoRef;
                for (const auto &isoWeight : topes.constMix())
                    weights.addIsotopologue(spInfo->species(), spInfo->population() * CFGWEIGHT, isoWeight.isotopologue(),
                                            isoWeight.weight());
            }
            else
                weights.addIsotopologue(sp, spInfo->population() * CFGWEIGHT, sp->naturalIsotopologue(), 1.0);
        }
    }

    return true;
}
