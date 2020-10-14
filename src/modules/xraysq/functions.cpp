/*
    *** XRay SQ Module - Functions
    *** src/modules/xraysq/functions.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesinfo.h"
#include "classes/xrayweights.h"
#include "modules/xraysq/xraysq.h"

// Calculate weighted g(r) from supplied unweighted g(r) and Weights
bool XRaySQModule::calculateWeightedGR(PartialSet &unweightedgr, PartialSet &weightedgr, const XRayWeights &weights,
                                       XRaySQModule::NormalisationType normalisation)
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
    // 	if (normalisation == XRaySQModule::AverageOfSquaresNormalisation) weightedgr.total().values() /=
    // weights.boundCoherentAverageOfSquares(); 	else if (normalisation == XRaySQModule::SquareOfAverageNormalisation)
    // weightedgr.total().values() /= weights.boundCoherentSquareOfAverage();

    return true;
}

// Calculate weighted S(Q) from supplied unweighted S(Q) and Weights
bool XRaySQModule::calculateWeightedSQ(PartialSet &unweightedsq, PartialSet &weightedsq, const XRayWeights &weights,
                                       XRaySQModule::NormalisationType normalisation)
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
    if (normalisation == XRaySQModule::SquareOfAverageNormalisation)
    {
        Array<double> bbar = weights.boundCoherentSquareOfAverage(unweightedsq.boundPartial(0, 0).constXAxis());
        for (int n = 0; n < bbar.nItems(); ++n)
            weightedsq.total().value(n) /= bbar[n];
    }
    else if (normalisation == XRaySQModule::AverageOfSquaresNormalisation)
    {
        Array<double> bbar = weights.boundCoherentAverageOfSquares(unweightedsq.boundPartial(0, 0).constXAxis());
        for (int n = 0; n < bbar.nItems(); ++n)
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
