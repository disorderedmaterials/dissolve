// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/xRayWeights.h"
#include "classes/atomType.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <functional>
#include <numeric>

// Set-up from supplied species populations and form factors
bool XRayWeights::setUp(const std::vector<std::pair<const Species *, double>> &speciesPopulations,
                        XRayFormFactors::XRayFormFactorData formFactors)
{
    typeFractions_.clear();
    concentrations_.clear();
    concentrationProducts_.clear();
    preFactors_.clear();

    // Calculate fractional type weights
    typeFractions_.clear();
    auto sum = 0.0;
    for (auto &[species, speciesPopulation] : speciesPopulations)
        for (const auto &[atomType, atomTypePopulation] : species->atomTypePopulations())
        {
            typeFractions_[atomType] += speciesPopulation * atomTypePopulation;
            sum += speciesPopulation * atomTypePopulation;
        }

    // Normalise the atom type weights
    for (auto &value : std::views::values(typeFractions_))
        value /= sum;

    // Retrieve form factor data for the current atom types
    formFactors_ = formFactors;
    formFactorData_.clear();

    for (auto &[atomType, fraction] : typeFractions_)
    {
        // Try to retrieve form factor data for this atom type (element, formal charge [TODO])
        auto data = XRayFormFactors::formFactorData(formFactors_, atomType->Z());
        if (!data)
            return Messenger::error("No form factor data present for element {} (formal charge {}) in x-ray data set '{}'.\n",
                                    Elements::symbol(atomType->Z()), 0,
                                    XRayFormFactors::xRayFormFactorData().keyword(formFactors_));

        formFactorData_.push_back(*data);
    }

    // Set up weights matrices
    auto nTypes = typeFractions_.size();
    concentrations_.clear();
    concentrations_.resize(nTypes);
    concentrationProducts_.initialise(nTypes, nTypes, true);
    preFactors_.initialise(nTypes, nTypes, true);

    // Determine atomic concentration products and full pre-factor
    dissolve::for_each_pair(ParallelPolicies::seq, typeFractions_,
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                auto ci = popI.second;
                                auto cj = popJ.second;

                                concentrations_.at(indexI) = ci;
                                concentrationProducts_[{indexI, indexJ}] = ci * cj;
                                preFactors_[{indexI, indexJ}] = ci * cj * (indexI == indexJ ? 1 : 2);
                            });

    return true;
}

// Return X-Ray form factors being used
XRayFormFactors::XRayFormFactorData XRayWeights::formFactors() const { return formFactors_; }

// Return atom type fractions
const KeyedVector<const AtomType *, double> &XRayWeights::typeFractions() const { return typeFractions_; }

// Return pre-factor for types i and j
double XRayWeights::preFactor(int typeIndexI, int typeIndexJ) const { return preFactors_[{typeIndexI, typeIndexJ}]; }

// Return form factor for type i over supplied Q values
std::vector<double> XRayWeights::formFactor(int typeIndexI, const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> fiq(Q.size());

    auto &fi = formFactorData_[typeIndexI].get();

    for (auto n = 0; n < Q.size(); ++n)
        fiq[n] = fi.magnitude(Q[n]);

    return fiq;
}

// Return form factor product for types i and j at specified Q value
double XRayWeights::formFactorProduct(int typeIndexI, int typeIndexJ, double Q) const
{
    return formFactorData_[typeIndexI].get().magnitude(Q) * formFactorData_[typeIndexJ].get().magnitude(Q);
}

// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) at specified Q value
double XRayWeights::weight(int typeIndexI, int typeIndexJ, double Q) const
{
    return preFactors_[{typeIndexI, typeIndexJ}] * formFactorProduct(typeIndexI, typeIndexJ, Q);
}

// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) over supplied Q values
std::vector<double> XRayWeights::weight(int typeIndexI, int typeIndexJ, const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> fijq(Q.size());

    auto &fi = formFactorData_[typeIndexI].get();
    auto &fj = formFactorData_[typeIndexJ].get();
    auto preFactor = preFactors_[{typeIndexI, typeIndexJ}];

    std::transform(Q.begin(), Q.end(), fijq.begin(),
                   [preFactor, &fi, &fj](auto q) { return fi.magnitude(q) * fj.magnitude(q) * preFactor; });

    return fijq;
}

// Calculate and return Q-dependent average squared scattering (<b>**2) for supplied Q value
double XRayWeights::boundCoherentSquareOfAverage(double Q) const
{
    auto result = std::inner_product(concentrations_.begin(), concentrations_.end(), formFactorData_.begin(), 0.0,
                                     std::plus<>(), [Q](auto con, auto form) { return con * form.get().magnitude(Q); });
    return result * result;
}

// Calculate and return Q-dependent average squared scattering (<b>**2) for supplied Q values
std::vector<double> XRayWeights::boundCoherentSquareOfAverage(const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> bbar(Q.size(), 0.0);

    for (auto typeI = 0; typeI < typeFractions_.size(); ++typeI)
    {
        const double ci = concentrations_[typeI];
        auto &fi = formFactorData_[typeI].get();

        std::transform(Q.begin(), Q.end(), bbar.begin(), bbar.begin(),
                       [ci, &fi](auto q, auto b) { return b + ci * fi.magnitude(q); });
    }

    // Square the averages
    std::transform(bbar.begin(), bbar.end(), bbar.begin(), [](auto b) { return b * b; });

    return bbar;
}

// Calculate and return Q-dependent squared average scattering (<b**2>) for supplied Q value
double XRayWeights::boundCoherentAverageOfSquares(double Q) const
{
    return std::inner_product(concentrations_.begin(), concentrations_.end(), formFactorData_.begin(), 0.0, std::plus<>(),
                              [Q](auto con, auto form) { return con * form.get().magnitude(Q) * form.get().magnitude(Q); });
}

// Calculate and return Q-dependent squared average scattering (<b**2>) for supplied Q values
std::vector<double> XRayWeights::boundCoherentAverageOfSquares(const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> bbar(Q.size(), 0.0);

    for (auto typeI = 0; typeI < typeFractions_.size(); ++typeI)
    {
        const double ci = concentrations_[typeI];
        auto &fi = formFactorData_[typeI].get();

        std::transform(Q.begin(), Q.end(), bbar.begin(), bbar.begin(),
                       [&](auto q, auto b) { return b + ci * fi.magnitude(q) * fi.magnitude(q); });
    }

    return bbar;
}

// Print the atom fractions
void XRayWeights::print() const
{
    Messenger::print("  AtomType  El   Fraction\n");
    Messenger::print("  -----------------------\n");
    for (auto &[atomType, fraction] : typeFractions_)
    {
        Messenger::print("  {:<8}  {:<3}  {:8.6f}\n", atomType->name(), Elements::symbol(atomType->Z()), fraction);

        Messenger::print("  -----------------------------------------------------------------\n");
    }
}
