// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include <numeric>

#include "classes/configuration.h"
#include "classes/isotopologueSet.h"
#include "classes/species.h"
#include "modules/gr/gr.h"
#include "modules/cgNeutronSQ/cgNeutronSQ.h"

// Calculate weighted g(r) from supplied unweighted g(r) and neutron weights
bool CGNeutronSQModule::calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, NeutronWeights &weights,
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
bool CGNeutronSQModule::calculateWeightedSQ(const PartialSet &unweightedsq, PartialSet &weightedsq, NeutronWeights &weights,
                                            const std::vector<Data1D> &ff, const std::vector<Data1D> &singleBead, 
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
            weightedsq.boundPartial(typeI, typeJ) *= ff[typeI].values();
            weightedsq.boundPartial(typeI, typeJ) *= ff[typeJ].values();

            // Unbound partial (multiplied by the full weight)
            weightedsq.unboundPartial(typeI, typeJ).copyArrays(unweightedsq.unboundPartial(typeI, typeJ));
            weightedsq.unboundPartial(typeI, typeJ) *= weight;
            weightedsq.unboundPartial(typeI, typeJ) *= ff[typeI].values();
            weightedsq.unboundPartial(typeI, typeJ) *= ff[typeJ].values();

            // Full partial (sum of bound and unbound terms)
            weightedsq.partial(typeI, typeJ).copyArrays(weightedsq.unboundPartial(typeI, typeJ));
            weightedsq.partial(typeI, typeJ) += weightedsq.boundPartial(typeI, typeJ);
        }
    }

    // Form total structure factor
    weightedsq.formTotals(false);
    for (typeI = 0; typeI < unweightedsq.nAtomTypes(); ++typeI) {
        weightedsq.total() += singleBead[typeI];
    }

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
void CGNeutronSQModule::calculateWeights(const GRModule *rdfModule, NeutronWeights &weights) const
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

//
bool CGNeutronSQModule::calculateBeadFormFactor(const std::vector<double> &qvals, std::vector<Data1D> &ff, const  NeutronWeights &weights) const
{
    int typeI;
    int dens{0};
    //std::array<double, 2> sigma{1.63, 1.57};
    std::array<double, 2> sigma{3.0376754356895721, 3.7573162947024161};
    double f;
    ff.clear();
    ff.reserve(qvals.size());

    for (typeI = 0; typeI < weights.atomTypes().nItems(); ++typeI)
    {
        ff.emplace_back(Data1D());
        for (double q : qvals)
        {
            double qs = q * sigma[typeI];
            if (dens == 0) // gaussian
            {
                f = -0.5 * std::pow((0.51 * qs), 2);
                f = std::exp(f);
                ff[typeI].addPoint(q, f);
            }
            else if (dens == 1) // uniform
            {
                f = 3.0 / std::pow((qs), 3);
                f *= (std::sin(qs) - qs * std::cos(qs));
                ff[typeI].addPoint(q, f);
            }
            else // no form factor
            {
                ff[typeI].addPoint(q, 1.0);
            }
        }
    }
    return true;
}

//
bool CGNeutronSQModule::calculateSingleBead(std::vector<Data1D> &singleBead, const std::vector<Data1D> &ff, const NeutronWeights &weights) const { 
    
    singleBead.clear();
    singleBead.reserve(weights.atomTypes().nItems());
    std::array<double, 2> b{0.6646, -0.3739};
    std::array<std::array<double, 2>, 2> n;
    n[0][0] = 3;
    n[0][1] = 7;
    n[1][0] = 3;
    n[1][1] = 6;
    int atmI, atmJ;
    for (auto bead = weights.atomTypes().begin(); bead != weights.atomTypes().end(); ++bead)
    {
        int s = bead - weights.atomTypes().begin();
        double innerSum{0.0};
        singleBead.emplace_back(Data1D());
        for (atmI = 0; atmI < 2; ++atmI)
        {
            for (atmJ = atmI; atmJ < 2; ++atmJ)
            {
                innerSum += n[s][atmI] * n[s][atmJ] * b[atmI] * b[atmJ] * (atmI == atmJ ? 1 : 2);
            }
        }
        innerSum -= ((n[s][0] * b[0] * b[0]) + (n[s][1] * b[1] * b[1])); 
        innerSum *= 0.5; //  (*bead).fraction();
        singleBead[s].copyArrays(ff[s]);
        singleBead[s] *= ff[s].values();
        singleBead[s] *= innerSum * (1/9.5);
    }
    
    return true; 
}