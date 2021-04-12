// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "main/dissolve.h"
#include "math/gaussfit.h"
#include "math/poissonfit.h"
#include "modules/epsr/epsr.h"
#include "templates/algorithms.h"

// Create / update delta S(Q) information
void EPSRModule::updateDeltaSQ(GenericList &processingData, OptionalReferenceWrapper<const Array2D<Data1D>> optCalculatedSQ,
                               OptionalReferenceWrapper<const Array2D<Data1D>> optEstimatedSQ)
{
    // Find the relevant data if we were not provided them
    if (!optCalculatedSQ)
        optCalculatedSQ = processingData.valueIf<Array2D<Data1D>>("UnweightedSQ", uniqueName_);
    if (!optCalculatedSQ)
        return;
    if (!optEstimatedSQ)
        optEstimatedSQ = processingData.valueIf<Array2D<Data1D>>("EstimatedSQ", uniqueName_);
    if (!optEstimatedSQ)
        return;

    const auto &calculatedSQ = optCalculatedSQ->get();
    const auto &estimatedSQ = optEstimatedSQ->get();
    assert(calculatedSQ.nRows() == estimatedSQ.nRows() && calculatedSQ.nColumns() == estimatedSQ.nColumns());

    // Realise the DeltaSQ array
    auto [deltaSQ, status] = processingData.realiseIf<Array2D<Data1D>>("DeltaSQ", uniqueName_, GenericItem::ItemFlag::NoFlags);
    if (status == GenericItem::ItemStatus::Created)
        deltaSQ.initialise(calculatedSQ.nRows(), calculatedSQ.nRows(), true);

    // Copy the tags from the calculated data (so we avoid requiring the source AtomTypeList) and create the data
    for (auto &&[delta, calc, est] : zip(deltaSQ, calculatedSQ, estimatedSQ))
    {
        delta.setTag(calc.tag());
        delta = est;
        Interpolator::addInterpolated(delta, calc, -1.0);
    }
}

// Return list of target Modules / data for refinement
const std::vector<Module *> &EPSRModule::targets() const { return keywords_.retrieve<std::vector<Module *>>("Target"); }

// Create / retrieve arrays for storage of empirical potential coefficients
Array2D<std::vector<double>> &EPSRModule::potentialCoefficients(Dissolve &dissolve, const int nAtomTypes, const int ncoeffp)
{
    auto &coefficients = dissolve.processingModuleData().realise<Array2D<std::vector<double>>>(
        "PotentialCoefficients", uniqueName_, GenericItem::InRestartFileFlag);

    auto arrayNCoeffP = (coefficients.nRows() && coefficients.nColumns() ? coefficients[{0, 0}].size() : 0);
    if ((coefficients.nRows() != nAtomTypes) || (coefficients.nColumns() != nAtomTypes) ||
        ((ncoeffp != -1) && (ncoeffp != arrayNCoeffP)))
    {
        coefficients.initialise(nAtomTypes, nAtomTypes, true);
        for (auto &n : coefficients)
        {
            n.clear();
            if (ncoeffp > 0)
                n.resize(ncoeffp, 0);
        }
    }

    return coefficients;
}

// Generate empirical potentials from current coefficients
bool EPSRModule::generateEmpiricalPotentials(Dissolve &dissolve, EPSRModule::ExpansionFunctionType functionType,
                                             double averagedRho, int ncoeffp, double rminpt, double rmaxpt, double sigma1,
                                             double sigma2)
{
    const auto nAtomTypes = dissolve.nAtomTypes();

    // Get coefficients array
    Array2D<std::vector<double>> &coefficients = potentialCoefficients(dissolve, nAtomTypes, ncoeffp);

    auto result = for_each_pair_early(
        dissolve.atomTypes().begin(), dissolve.atomTypes().end(), [&](int i, auto at1, int j, auto at2) -> EarlyReturn<bool> {
            auto &potCoeff = coefficients[{i, j}];

            // Regenerate empirical potential from the stored coefficients
            Data1D ep;
            if (functionType == EPSRModule::GaussianExpansionFunction)
            {
                // Construct our fitting object and generate the potential using it
                GaussFit generator(ep);
                generator.set(rmaxpt, potCoeff, sigma1);
                ep = generator.approximation(FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(),
                                             dissolve.pairPotentialRange(), sigma2 / sigma1);
            }
            else if (functionType == EPSRModule::PoissonExpansionFunction)
            {
                // Construct our fitting object and generate the potential using it
                // We pass 1.0/rho as the factor to PossonFit::approximation() - this is the factor of rho not
                // present in our denominator
                PoissonFit generator(ep);
                generator.set(FunctionSpace::ReciprocalSpace, rmaxpt, potCoeff, sigma1, sigma2);
                ep = generator.approximation(FunctionSpace::RealSpace, 1.0 / averagedRho, 0.0, dissolve.pairPotentialDelta(),
                                             dissolve.pairPotentialRange());
            }

            // Multiply by truncation function
            truncate(ep, rminpt, rmaxpt);

            // Set the additional potential in the main processing data
            dissolve.processingModuleData().realise<Data1D>(fmt::format("Potential_{}-{}_Additional", at1->name(), at2->name()),
                                                            "Dissolve", GenericItem::InRestartFileFlag) = ep;

            // Grab pointer to the relevant pair potential (if it exists)
            auto *pp = dissolve.pairPotential(at1, at2);
            if (pp)
                pp->setUAdditional(ep);

            return EarlyReturn<bool>::Continue;
        });

    return result.value_or(true);
}

// Generate and return single empirical potential function
Data1D EPSRModule::generateEmpiricalPotentialFunction(Dissolve &dissolve, int i, int j, int n)
{
    const auto nAtomTypes = dissolve.nAtomTypes();
    auto functionType = keywords_.enumeration<EPSRModule::ExpansionFunctionType>("ExpansionFunction");
    const auto gsigma1 = keywords_.asDouble("GSigma1");
    const auto gsigma2 = keywords_.asDouble("GSigma2");
    auto ncoeffp = keywords_.asInt("NCoeffP");
    const auto psigma1 = keywords_.asDouble("PSigma1");
    const auto psigma2 = keywords_.asDouble("PSigma2");
    auto rmaxpt = keywords_.asDouble("RMaxPT");
    auto rminpt = keywords_.asDouble("RMinPT");

    // EPSR constants
    const auto mcoeff = 200;

    // Calculate some values if they were not provided
    if (rmaxpt < 0.0)
        rmaxpt = dissolve.pairPotentialRange();
    if (rminpt < 0.0)
        rminpt = rmaxpt - 2.0;
    if (ncoeffp <= 0)
        ncoeffp = std::min(int(10.0 * rmaxpt + 0.0001), mcoeff);

    // Get coefficients array
    auto &coefficients = potentialCoefficients(dissolve, nAtomTypes);
    auto &potCoeff = coefficients[{i, j}];

    // Regenerate empirical potential from the stored coefficients
    Data1D result;
    if (functionType == EPSRModule::GaussianExpansionFunction)
    {
        // Construct our fitting object and generate the potential using it
        GaussFit generator(result);
        generator.set(rmaxpt, potCoeff, gsigma1);
        result = generator.singleFunction(n, FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(),
                                          dissolve.pairPotentialRange(), gsigma2 / gsigma1);
    }
    else if (functionType == EPSRModule::PoissonExpansionFunction)
    {
        // Construct our fitting object and generate the potential using it
        PoissonFit generator(result);
        generator.set(FunctionSpace::ReciprocalSpace, rmaxpt, potCoeff, psigma1, psigma2);
        result = generator.singleFunction(n, FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(),
                                          dissolve.pairPotentialRange());
    }

    // Multiply by truncation function
    truncate(result, rminpt, rmaxpt);

    return result;
}

// Calculate absolute energy of empirical potentials
double EPSRModule::absEnergyEP(Dissolve &dissolve)
{
    /*
     * Routine from EPSR25.
     * Loop over sets of empirical potential coefficients and find the maximal/minimal values.
     * Return the largest range we find.
     */

    // Get coefficients array
    auto &coefficients = potentialCoefficients(dissolve, dissolve.nAtomTypes());
    if (coefficients.empty())
        return 0.0;

    auto absEnergyEP = 0.0;

    for_each_pair(dissolve.atomTypes().begin(), dissolve.atomTypes().end(), [&](int i, auto at1, int j, auto at2) {
        auto &potCoeff = coefficients[{i, j}];

        auto cMin = potCoeff.empty() ? 0.0 : *std::min_element(potCoeff.begin(), potCoeff.end());
        auto cMax = potCoeff.empty() ? 0.0 : *std::max_element(potCoeff.begin(), potCoeff.end());

        auto range = cMax - cMin;
        if (range > absEnergyEP)
            absEnergyEP = range;

        // Output information
        Messenger::print("  abs_energy_ep>    {:4} {:4} {:12.6f}\n", at1->name(), at2->name(), range);
    });

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
            y[n] *= 0.5 * (1.0 + cos(((x - rMin) * PI) / decay));
    }
}
