// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/xRayWeights.h"
#include "classes/atomType.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <functional>
#include <numeric>

// Set-up from supplied species populations and form factors
bool XRayWeights::setUp(const std::map<const Species *, double> &speciesPopulations,
                        XRayFormFactors::XRayFormFactorData formFactors)
{
    typeFractions_.clear();
    concentrationProducts_.clear();
    preFactors_.clear();

    // Calculate fractional type weights
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
    formFactorData_.clear();

    for (auto &[atomType, fraction] : typeFractions_)
    {
        // Try to retrieve form factor data for this atom type (element, formal charge [TODO])
        auto data = XRayFormFactors::formFactorData(formFactors, atomType->Z());
        if (!data)
            return Messenger::error("No form factor data present for element {} (formal charge {}) in x-ray data set '{}'.\n",
                                    Elements::symbol(atomType->Z()), 0,
                                    XRayFormFactors::xRayFormFactorData().keyword(formFactors));

        formFactorData_.emplace(atomType, *data);
    }

    // Set up weights matrices
    auto nTypes = typeFractions_.size();
    concentrationProducts_.clear();
    preFactors_.clear();

    // Determine atomic concentration products and full pre-factor
    dissolve::for_each_pair(ParallelPolicies::seq, typeFractions_,
                            [&](int indexI, const auto &popI, int indexJ, const auto &popJ)
                            {
                                DoubleKeyedMapKey key{popI.first->name(), popJ.first->name()};
                                auto ci = popI.second;
                                auto cj = popJ.second;

                                concentrationProducts_.set(key, ci * cj);
                                preFactors_.set(key, ci * cj * (indexI == indexJ ? 1 : 2));
                            });

    return true;
}

// Return atom type fractions
const KeyedVector<const AtomType *, double> &XRayWeights::typeFractions() const { return typeFractions_; }

// Return pre-factor for types i and j
const DoubleKeyedMap<double> &XRayWeights::preFactors() const { return preFactors_; }

// Return form factor for type i over supplied Q values
std::vector<double> XRayWeights::formFactor(const AtomType *i, const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> fiq(Q.size());

    auto &fi = formFactorData_.at(i).get();

    for (auto n = 0; n < Q.size(); ++n)
        fiq[n] = fi.magnitude(Q[n]);

    return fiq;
}

// Return form factor product for types i and j at specified Q value
double XRayWeights::formFactorProduct(const AtomType *i, const AtomType *j, double Q) const
{
    return formFactorData_.at(i).get().magnitude(Q) * formFactorData_.at(j).get().magnitude(Q);
}

// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) at specified Q value
double XRayWeights::weight(const AtomType *i, const AtomType *j, double Q) const
{
    return preFactors_[{i->name(), j->name()}] * formFactorProduct(i, j, Q);
}

// Return full weighting for types i and j (ci * cj * f(i,Q) * F(j,Q) * [2-dij]) over supplied Q values
std::vector<double> XRayWeights::weight(const AtomType *i, const AtomType *j, const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> fijq(Q.size());

    auto &fi = formFactorData_.at(i).get();
    auto &fj = formFactorData_.at(j).get();
    auto preFactor = preFactors_.get(i->name(), j->name());

    std::transform(Q.begin(), Q.end(), fijq.begin(),
                   [preFactor, &fi, &fj](auto q) { return fi.magnitude(q) * fj.magnitude(q) * preFactor; });

    return fijq;
}

// Calculate and return Q-dependent average squared scattering (<b>**2) for supplied Q value
double XRayWeights::boundCoherentSquareOfAverage(double Q) const
{
    auto result = std::accumulate(typeFractions_.begin(), typeFractions_.end(), 0.0, [&, Q](auto acc, const auto &typePop)
                                  { return acc + typePop.second * formFactorData_.at(typePop.first).get().magnitude(Q); });
    return result * result;
}

// Calculate and return Q-dependent average squared scattering (<b>**2) for supplied Q values
std::vector<double> XRayWeights::boundCoherentSquareOfAverage(const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> bbar(Q.size(), 0.0);

    for (auto &[atomType, fraction] : typeFractions_)
    {
        auto &fi = formFactorData_.at(atomType).get();

        std::transform(Q.begin(), Q.end(), bbar.begin(), bbar.begin(),
                       [fraction, &fi](auto q, auto b) { return b + fraction * fi.magnitude(q); });
    }

    // Square the averages
    std::transform(bbar.begin(), bbar.end(), bbar.begin(), [](auto b) { return b * b; });

    return bbar;
}

// Calculate and return Q-dependent squared average scattering (<b**2>) for supplied Q value
double XRayWeights::boundCoherentAverageOfSquares(double Q) const
{
    return std::accumulate(typeFractions_.begin(), typeFractions_.end(), 0.0,
                           [&, Q](auto acc, const auto &typePop)
                           {
                               auto mag = formFactorData_.at(typePop.first).get().magnitude(Q);
                               return acc + typePop.second * pow(mag, 2);
                           });
}

// Calculate and return Q-dependent squared average scattering (<b**2>) for supplied Q values
std::vector<double> XRayWeights::boundCoherentAverageOfSquares(const std::vector<double> &Q) const
{
    // Initialise results array
    std::vector<double> bbar(Q.size(), 0.0);

    for (auto &[atomType, fraction] : typeFractions_)
    {
        auto &fi = formFactorData_.at(atomType).get();

        std::transform(Q.begin(), Q.end(), bbar.begin(), bbar.begin(),
                       [&](auto q, auto b) { return b + fraction * fi.magnitude(q) * fi.magnitude(q); });
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
