// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/dissolve.h"
#include "math/gaussFit.h"
#include "math/poissonFit.h"
#include "module/context.h"
#include "modules/epsrManager/epsrManager.h"
#include "templates/algorithms.h"

// Generate empirical potentials from current coefficients
bool EPSRManagerModule::generateEmpiricalPotentials(GenericList &moduleData, Dissolve &dissolve, double averagedRho,
                                                    std::optional<int> ncoeffp, double rminpt, double rmaxpt, double sigma1,
                                                    double sigma2)
{
    // Loop over target data
    for (auto *module : target_)
    {
        // Get coefficients array
        const auto &coefficients = moduleData.valueIf<Array2D<std::vector<double>>>("PotentialCoefficients", module->name());

        if (!coefficients)
        {
            printf("That doesn't exist!");
            return false;
        }

        auto &object = coefficients->get();

        const auto &atomTypes = scatteringMatrix_.atomTypes();
        const auto nAtomTypes = atomTypes.size();

        auto result = for_each_pair_early(
            atomTypes.begin(), atomTypes.end(),
            [&](int i, auto at1, int j, auto at2) -> EarlyReturn<bool>
            {
                auto &potCoeff = object[{i, j}];

                // Regenerate empirical potential from the stored coefficients
                Data1D ep;
                if (expansionFunction_ == EPSRManagerModule::GaussianExpansionFunction)
                {
                    // Construct our fitting object and generate the potential using it
                    GaussFit generator(ep);
                    generator.set(rmaxpt, potCoeff, sigma1);
                    ep = generator.approximation(FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(),
                                                 dissolve.pairPotentialRange(), sigma2 / sigma1);
                }
                else if (expansionFunction_ == EPSRManagerModule::PoissonExpansionFunction)
                {
                    // Construct our fitting object and generate the potential using it
                    // We pass 1.0/rho as the factor to PossonFit::approximation() - this is the factor of rho not
                    // present in our denominator
                    PoissonFit generator(ep);
                    generator.set(FunctionSpace::ReciprocalSpace, rmaxpt, potCoeff, sigma1, sigma2);
                    ep = generator.approximation(FunctionSpace::RealSpace, 1.0 / averagedRho, 0.0,
                                                 dissolve.pairPotentialDelta(), dissolve.pairPotentialRange());
                }

                // Multiply by truncation function
                truncate(ep, rminpt, rmaxpt);

                // Set the additional potential in the main processing data
                dissolve.processingModuleData().realise<Data1D>(
                    fmt::format("Potential_{}-{}_Additional", at1->name(), at2->name()), "Dissolve",
                    GenericItem::InRestartFileFlag) = ep;

                // Grab pointer to the relevant pair potential (if it exists)
                auto *pp = dissolve.pairPotential(at1, at2);
                if (pp)
                    pp->setUAdditional(ep);

                return EarlyReturn<bool>::Continue;
            });
        return result.value_or(true);
    }
}

// Truncate the supplied data
void EPSRManagerModule::truncate(Data1D &data, double rMin, double rMax)
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