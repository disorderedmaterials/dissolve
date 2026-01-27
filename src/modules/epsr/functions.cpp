// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#define _USE_MATH_DEFINES
#include "main/dissolve.h"
#include "math/gaussFit.h"
#include "math/mathFunc.h"
#include "math/poissonFit.h"
#include "modules/epsr/epsr.h"
#include "templates/algorithms.h"

// Create / update delta S(Q) information
void EPSRModule::updateDeltaSQ(GenericList &processingData,
                               OptionalReferenceWrapper<const DoubleKeyedMap<Data1D>> optCalculatedSQ,
                               OptionalReferenceWrapper<const DoubleKeyedMap<Data1D>> optEstimatedSQ)
{
    // Find the relevant data if we were not provided them
    if (!optCalculatedSQ)
        optCalculatedSQ = processingData.valueIf<DoubleKeyedMap<Data1D>>("UnweightedSQ", name_);
    if (!optCalculatedSQ)
        return;
    if (!optEstimatedSQ)
        optEstimatedSQ = processingData.valueIf<DoubleKeyedMap<Data1D>>("EstimatedSQ", name_);
    if (!optEstimatedSQ)
        return;

    const auto &calculatedSQ = optCalculatedSQ->get();
    const auto &estimatedSQ = optEstimatedSQ->get();
    assert(calculatedSQ.size() == estimatedSQ.size());

    // Realise the DeltaSQ array
    auto deltaSQ = processingData.realise<DoubleKeyedMap<Data1D>>("DeltaSQ", name_, GenericItem::ItemFlag::NoFlags);
    deltaSQ.clear(true);

    for (auto &[key, calcSQ] : calculatedSQ)
    {
        deltaSQ[key] = estimatedSQ[key];
        Interpolator::addInterpolated(calcSQ, deltaSQ[key], -1.0);

        // Copy the tag
        deltaSQ[key].setTag(calcSQ.tag());
    }
}

// Create / retrieve arrays for storage of empirical potential coefficients
Array2D<std::vector<double>> &EPSRModule::potentialCoefficients(GenericList &moduleData, const int nAtomTypes,
                                                                std::optional<int> ncoeffp)
{
    auto &coefficients =
        moduleData.realise<Array2D<std::vector<double>>>("PotentialCoefficients", name_, GenericItem::InRestartFileFlag);

    auto arrayNCoeffP = (coefficients.nRows() && coefficients.nColumns() ? coefficients[{0, 0}].size() : 0);
    if ((coefficients.nRows() != nAtomTypes) || (coefficients.nColumns() != nAtomTypes) ||
        (ncoeffp && ncoeffp.value() != arrayNCoeffP))
    {
        coefficients.initialise(nAtomTypes, nAtomTypes, true);
        for (auto &n : coefficients)
        {
            n.clear();
            if (ncoeffp && ncoeffp.value() > 0)
                n.resize(ncoeffp.value(), 0);
        }
    }

    return coefficients;
}

// Generate empirical potentials from current coefficients
bool EPSRModule::generateEmpiricalPotentials(Dissolve &dissolve, const std::vector<const AtomType *> &atomTypes,
                                             double averagedRho, std::optional<int> ncoeffp, double rminpt, double rmaxpt,
                                             double sigma1, double sigma2)
{
    // Get coefficients array
    Array2D<std::vector<double>> &coefficients =
        potentialCoefficients(dissolve.processingModuleData(), atomTypes.size(), ncoeffp);

    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int i, auto at1, int j, auto at2)
                            {
                                auto &potCoeff = coefficients[{i, j}];

                                // Regenerate empirical potential from the stored coefficients
                                Data1D ep;
                                if (expansionFunction_ == EPSRModule::GaussianExpansionFunction)
                                {
                                    // Construct our fitting object and generate the potential using it
                                    GaussFit generator(ep);
                                    generator.set(rmaxpt, potCoeff, sigma1);
                                    ep = generator.approximation(FunctionSpace::RealSpace, 1.0, 0.0,
                                                                 dissolve.pairPotentialDelta(), dissolve.pairPotentialRange(),
                                                                 sigma2 / sigma1);
                                }
                                else if (expansionFunction_ == EPSRModule::PoissonExpansionFunction)
                                {
                                    // Construct our fitting object and generate the potential using it
                                    // We pass 1.0/rho as the factor to PossonFit::approximation() - this is the factor of rho
                                    // not present in our denominator
                                    PoissonFit generator(ep);
                                    generator.set(FunctionSpace::ReciprocalSpace, rmaxpt, potCoeff, sigma1, sigma2);
                                    ep = generator.approximation(FunctionSpace::RealSpace, 1.0 / averagedRho, 0.0,
                                                                 dissolve.pairPotentialDelta(), dissolve.pairPotentialRange());
                                }

                                // Multiply by truncation function
                                truncate(ep, rminpt, rmaxpt);

                                // Put potentials in vector
                                empiricalPotentials_.emplace_back(at1, at2, ep);

                                // Apply potentials?
                                if (applyPotentials_)
                                {
                                    // Set the additional potential in the main processing data
                                    dissolve.processingModuleData().realise<Data1D>(
                                        std::format("Potential_{}-{}_Additional", at1->name(), at2->name()), "Dissolve",
                                        GenericItem::InRestartFileFlag) = ep;

                                    // Grab pointer to the relevant pair potential (if it exists)
                                    auto *pp = dissolve.pairPotential(at1, at2);
                                    if (pp)
                                        pp->setAdditionalPotential(ep);
                                }
                            });

    return true;
}

// Generate and return single empirical potential function
Data1D EPSRModule::generateEmpiricalPotentialFunction(Dissolve &dissolve, int nAtomTypes, int i, int j, int n)
{
    // EPSR constants
    const auto mcoeff = 200;

    // Calculate some values if they were not provided
    auto rmaxpt = rMaxPT_ ? rMaxPT_.value() : dissolve.pairPotentialRange();
    auto rminpt = rMinPT_ ? rMinPT_.value() : rmaxpt - 2.0;
    nCoeffP_ = nCoeffP_ <= 0 ? std::min(int(10.0 * rmaxpt + 0.0001), mcoeff) : nCoeffP_;

    // Get coefficients array
    auto &coefficients = potentialCoefficients(dissolve.processingModuleData(), nAtomTypes);
    auto &potCoeff = coefficients[{i, j}];

    // Regenerate empirical potential from the stored coefficients
    Data1D result;
    if (expansionFunction_ == EPSRModule::GaussianExpansionFunction)
    {
        // Construct our fitting object and generate the potential using it
        GaussFit generator(result);
        generator.set(rmaxpt, potCoeff, gSigma1_);
        result = generator.singleFunction(n, FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(),
                                          dissolve.pairPotentialRange(), gSigma2_ / gSigma1_);
    }
    else if (expansionFunction_ == EPSRModule::PoissonExpansionFunction)
    {
        // Construct our fitting object and generate the potential using it
        PoissonFit generator(result);
        generator.set(FunctionSpace::ReciprocalSpace, rmaxpt, potCoeff, pSigma1_, pSigma2_);
        result = generator.singleFunction(n, FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(),
                                          dissolve.pairPotentialRange());
    }

    // Multiply by truncation function
    truncate(result, rminpt, rmaxpt);

    return result;
}

// Calculate absolute energy of empirical potentials
double EPSRModule::absEnergyEP(GenericList &moduleData, const std::vector<const AtomType *> &atomTypes)
{
    /*
     * Routine from EPSR25.
     * Loop over sets of empirical potential coefficients and find the maximal/minimal values.
     * Return the largest range we find.
     */

    // Get coefficients array
    auto &coefficients = potentialCoefficients(moduleData, atomTypes.size());
    if (coefficients.empty())
        return 0.0;

    auto absEnergyEP = 0.0;

    auto unaryOp = [&](auto pair)
    {
        auto [i, j] = pair;
        auto &potCoeff = coefficients[{i, j}];

        auto cMin = potCoeff.empty() ? 0.0 : *std::min_element(potCoeff.begin(), potCoeff.end());
        auto cMax = potCoeff.empty() ? 0.0 : *std::max_element(potCoeff.begin(), potCoeff.end());

        auto range = cMax - cMin;
        if (range > absEnergyEP)
            absEnergyEP = range;

        // Output information
        Messenger::print("  abs_energy_ep>    {:4} {:4} {:12.6f}\n", atomTypes[i]->name(), atomTypes[j]->name(), range);
    };

    PairIterator pairs(atomTypes.size());
    dissolve::for_each(ParallelPolicies::seq, pairs.begin(), pairs.end(), unaryOp);

    return absEnergyEP;
}

// Truncate the supplied data
void EPSRModule::truncate(Data1D &data, double rMin, double rMax)
{
    // Replicates the EPSR25 truncate(xx,rminpt,rmaxpt) function applied over a whole dataset.
    double x;
    auto &y = data.values();
    const auto decay = rMax - rMin;
    for (auto n = 0; n < data.nValues(); ++n)
    {
        x = data.xAxis(n);

        if (x >= rMax)
            y[n] = 0.0;
        else if (x > rMin)
            y[n] *= 0.5 * (1.0 + cos(((x - rMin) * M_PI) / decay));
    }
}

std::vector<std::tuple<const AtomType *, const AtomType *, Data1D>> EPSRModule::empiricalPotentials()
{
    return empiricalPotentials_;
}
