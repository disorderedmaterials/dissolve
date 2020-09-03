/*
    *** EPSR Module - Functions
    *** src/modules/epsr/functions.cpp
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

#include "classes/atomtype.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include "math/gaussfit.h"
#include "math/poissonfit.h"
#include "modules/epsr/epsr.h"
#include "templates/algorithms.h"

// Return list of target Modules / data for refeinement
const RefDataList<Module, ModuleGroup *> &EPSRModule::allTargets() const { return groupedTargets_.modules(); }

// Return grouped target Modules
const ModuleGroups &EPSRModule::groupedTargets() const { return groupedTargets_; }

// Add target Modules
void EPSRModule::addTargets(RefList<Module> targets, std::string_view groupName)
{
    for (Module *module : targets)
        groupedTargets_.addModule(module, groupName);

    // Must flag that the associated keyword has been set by other means
    if (targets.nItems() > 0)
        keywords_.hasBeenSet("Target");
}

// Create / retrieve arrays for storage of empirical potential coefficients
Array2D<Array<double>> &EPSRModule::potentialCoefficients(Dissolve &dissolve, const int nAtomTypes, const int ncoeffp)
{
    auto &coefficients = GenericListHelper<Array2D<Array<double>>>::realise(
        dissolve.processingModuleData(), "PotentialCoefficients", uniqueName_, GenericItem::InRestartFileFlag);
    auto arrayNCoeffP = (coefficients.nRows() && coefficients.nColumns() ? coefficients.at(0, 0).nItems() : 0);
    if ((coefficients.nRows() != nAtomTypes) || (coefficients.nColumns() != nAtomTypes) ||
        ((ncoeffp != -1) && (ncoeffp != arrayNCoeffP)))
    {
        coefficients.initialise(nAtomTypes, nAtomTypes, true);
        for (int n = 0; n < coefficients.linearArraySize(); ++n)
        {
            coefficients.linearArray()[n].initialise(ncoeffp);
            coefficients.linearArray()[n] = 0.0;
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
    int i, j;

    // Get coefficients array
    Array2D<Array<double>> &coefficients = potentialCoefficients(dissolve, nAtomTypes, ncoeffp);

    i = 0;
    auto result = for_each_pair_early(
        dissolve.atomTypes().begin(), dissolve.atomTypes().end(), [&](int i, auto at1, int j, auto at2) -> std::optional<bool> {
            Array<double> &potCoeff = coefficients.at(i, j);

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

            // Grab pointer to the relevant pair potential
            PairPotential *pp = dissolve.pairPotential(at1, at2);
            if (!pp)
            {
                Messenger::error("Failed to find PairPotential for AtomTypes '{}' and '{}'.\n", at1->name(), at2->name());
                return false;
            }

            pp->setUAdditional(ep);
            return std::nullopt;
        });

    return result.value_or(true).value_or(true);
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
    const auto qMax = keywords_.asDouble("QMax");
    const auto qMin = keywords_.asDouble("QMin");
    double rmaxpt = keywords_.asDouble("RMaxPT");
    double rminpt = keywords_.asDouble("RMinPT");

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
    Array2D<Array<double>> &coefficients = potentialCoefficients(dissolve, nAtomTypes);
    Array<double> &potCoeff = coefficients.at(i, j);

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
    Array2D<Array<double>> &coefficients = potentialCoefficients(dissolve, dissolve.nAtomTypes());

    double absEnergyEP = 0.0;

    for_each_pair(dissolve.atomTypes().begin(), dissolve.atomTypes().end(), [&](int i, auto at1, int j, auto at2) {
        Array<double> &potCoeff = coefficients.at(i, j);

        double cMin = potCoeff.nItems() == 0 ? 0.0 : potCoeff.constAt(0);
        double cMax = cMin;
        for (int n = 1; n < potCoeff.nItems(); ++n)
        {
            if (potCoeff.constAt(n) < cMin)
                cMin = potCoeff.constAt(n);
            if (potCoeff.constAt(n) > cMax)
                cMax = potCoeff.constAt(n);
        }

        double range = cMax - cMin;
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
    Array<double> &y = data.values();
    const auto decay = rMax - rMin;
    for (int n = 0; n < data.nValues(); ++n)
    {
        x = data.xAxis(n);

        if (x >= rMax)
            y[n] = 0.0;
        // 		else if (x <= rMin) y[n] = y[n];
        else if (x > rMin)
            y[n] *= 0.5 * (1.0 + cos(((x - rMin) * PI) / decay));
    }
}
