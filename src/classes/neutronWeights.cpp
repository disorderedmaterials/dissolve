// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/neutronWeights.h"
#include "base/lineParser.h"
#include "classes/atomType.h"
#include "classes/isotopologueSet.h"
#include "classes/species.h"
#include "data/isotopes.h"
#include "items/deserialisers.h"
#include "items/serialisers.h"
#include "templates/algorithms.h"

NeutronWeights::NeutronWeights()
{
    boundCoherentSquareOfAverage_ = 0.0;
    boundCoherentAverageOfSquares_ = 0.0;
    valid_ = false;
}

NeutronWeights::NeutronWeights(const NeutronWeights &source) { (*this) = source; }

void NeutronWeights::operator=(const NeutronWeights &source)
{
    isotopologueMixtures_ = source.isotopologueMixtures_;
    isotopeMix_ = source.isotopeMix_;
    boundCoherentProducts_ = source.boundCoherentProducts_;
    concentrationProducts_ = source.concentrationProducts_;
    weights_ = source.weights_;
    intramolecularWeights_ = source.intramolecularWeights_;
    boundCoherentSquareOfAverage_ = source.boundCoherentSquareOfAverage_;
    boundCoherentAverageOfSquares_ = source.boundCoherentAverageOfSquares_;
    valid_ = source.valid_;
}

/*
 * Construction
 */

// Clear contents
void NeutronWeights::clear()
{
    isotopologueMixtures_.clear();
    concentrationProducts_.clear();
    boundCoherentProducts_.clear();
    weights_.clear();
    intramolecularWeights_.clear();
    boundCoherentSquareOfAverage_ = 0.0;
    boundCoherentAverageOfSquares_ = 0.0;
    valid_ = false;
}

// Add Isotopologue for Species
void NeutronWeights::addIsotopologue(const Species *sp, double speciesPopulation, const Isotopologue *iso,
                                     double isotopologueRelativePopulation)
{
    // Does an Isotopologues definition already exist for the supplied Species?
    auto it = std::find_if(isotopologueMixtures_.begin(), isotopologueMixtures_.end(),
                           [sp](auto &data) { return data.species() == sp; });

    if (it == isotopologueMixtures_.end())
    {
        isotopologueMixtures_.emplace_back(sp, speciesPopulation);
        isotopologueMixtures_.back().mix().add(iso, isotopologueRelativePopulation);
    }
    else
        it->mix().add(iso, isotopologueRelativePopulation);
}

// Return whether an Isotopologues definition exists for the provided Species
bool NeutronWeights::containsIsotopologues(const Species *sp) const
{
    return std::any_of(isotopologueMixtures_.cbegin(), isotopologueMixtures_.cend(),
                       [sp](const Isotopologues &mix) { return mix.species() == sp; });
}

// Print atomtype / weights information
void NeutronWeights::print() const
{
    Messenger::print("  Species          Isotopologue     nTotMols    Fraction\n");
    Messenger::print("  ------------------------------------------------------\n");
    for (auto &topes : isotopologueMixtures_)
    {
        for (auto it = topes.mix().begin(); it != topes.mix().end(); ++it)
        {
            if (it == topes.mix().begin())
                Messenger::print("  {:<15}  {:<15}  {:<10g}  {}\n", topes.species()->name(), it->first->name(),
                                 topes.speciesPopulation(), it->second);
            else
                Messenger::print("                   {:<15}              {}\n", it->first->name(), it->second);
        }
    }

    // Print atomtypes table
    Messenger::print("\n");
    isotopeMix_.print();

    Messenger::print("\nCalculated average scattering lengths: <b>**2 = {:.5f}, <b**2> = {:.5f}\n",
                     boundCoherentSquareOfAverage_, boundCoherentAverageOfSquares_);
}

/*
 * Data
 */

// Calculate weighting matrices based on current AtomType / Isotope information
void NeutronWeights::calculateWeightingMatrices()
{
    // Create weights matrices and calculate average scattering lengths
    // Note: Multiplier of 0.1 on b terms converts from units of fm (1e-11 m) to barn (1e-12 m)
    concentrationProducts_.clear(true);
    boundCoherentProducts_.clear(true);
    weights_.clear(true);
    intramolecularWeights_.clear(true);
    boundCoherentAverageOfSquares_ = 0.0;
    boundCoherentSquareOfAverage_ = 0.0;
    double ci, cj, bi, bj;

    // Determine atomic concentration products, bound coherent products, and full scattering weights
    dissolve::for_each_pair(ParallelPolicies::seq, isotopeMix_.mix(),
                            [&](int indexI, const auto &typeMixI, int indexJ, const auto &typeMixJ)
                            {
                                DoubleKeyedMapKey key{typeMixI.first->name(), typeMixJ.first->name()};

                                ci = isotopeMix_.fraction(typeMixI.first);
                                bi = isotopeMix_.boundCoherent(typeMixI.first) * 0.1;

                                // Update average scattering values
                                if (indexI == indexJ)
                                {
                                    boundCoherentSquareOfAverage_ += ci * bi;
                                    boundCoherentAverageOfSquares_ += ci * bi * bi;
                                }

                                cj = isotopeMix_.fraction(typeMixJ.first);
                                bj = isotopeMix_.boundCoherent(typeMixJ.first) * 0.1;

                                concentrationProducts_.set(key, ci * cj);
                                boundCoherentProducts_.set(key, bi * bj);
                                weights_.set(key, ci * cj * bi * bj * (indexI == indexJ ? 1 : 2));
                            });

    // Finalise <b>**2
    boundCoherentSquareOfAverage_ *= boundCoherentSquareOfAverage_;

    // Determine bound (intramolecular) scattering weights
    // Loop over defined Isotopologues in our defining mixtures, summing terms from (intramolecular) pairs of Atoms
    DoubleKeyedMap<double> intraNorm(true);
    DoubleKeyedMap<bool> globalFlag(true);
    for (auto &topes : isotopologueMixtures_)
    {
        // Get weighting for associated Species population
        auto speciesWeight = double(topes.speciesPopulation());

        // Using the underlying Species, construct a flag matrix which states the AtomType interactions we have present
        const auto *sp = topes.species();

        // Loop over Isotopologues defined for this mixture
        for (auto &[iso, weight] : topes.mix())
        {
            // Sum the scattering lengths of each pair of AtomTypes, weighted by the speciesWeight and the
            // fractional Isotopologue weight in the mix.
            dissolve::for_each_pair(ParallelPolicies::seq, sp->atomTypePopulations(),
                                    [&, iso, weight](int indexI, const auto &atPop1, int indexJ, const auto &atPop2)
                                    {
                                        DoubleKeyedMapKey key{atPop1.first->name(), atPop2.first->name()};

                                        // Find the atom types in our local mix
                                        auto optPairIndex = isotopeMix_.indexOf(atPop1.first, atPop2.first);
                                        if (!optPairIndex)
                                            return;
                                        auto &[typeI, typeJ] = *optPairIndex;

                                        // If an AtomType is exchangeable we use its exchanged bound coherent scattering length
                                        bi = isotopeMix_.isExchangeable(atPop1.first)
                                                 ? isotopeMix_.boundCoherent(atPop1.first)
                                                 : Sears91::boundCoherent(iso->atomTypeIsotope(atPop1.first));
                                        bj = isotopeMix_.isExchangeable(atPop2.first)
                                                 ? isotopeMix_.boundCoherent(atPop2.first)
                                                 : Sears91::boundCoherent(iso->atomTypeIsotope(atPop2.first));

                                        // Convert from fm to barns
                                        bi *= 0.1;
                                        bj *= 0.1;

                                        intramolecularWeights_[key] += weight * bi * bj;
                                        intraNorm[key] += weight;
                                        globalFlag[key] = true;
                                    });
        }
    }

    // Normalise the intramolecular weights, and multiply by atomic concentrations and Kronecker delta
    dissolve::for_each_pair(ParallelPolicies::seq, isotopeMix_.mix(),
                            [&](int indexI, const auto &typeMixI, int indexJ, const auto &typeMixJ)
                            {
                                DoubleKeyedMapKey key{typeMixI.first->name(), typeMixJ.first->name()};

                                // Zero this term if there are no intramolecular interactions
                                if (!globalFlag.contains(key))
                                {
                                    intramolecularWeights_[key] = 0.0;
                                    return;
                                }

                                ci = isotopeMix_.fraction(typeMixI.first);
                                cj = isotopeMix_.fraction(typeMixJ.first);

                                intramolecularWeights_[key] /= intraNorm[key];
                                intramolecularWeights_[key] *= ci * cj * (indexI == indexJ ? 1 : 2);
                            });
}

// Create AtomType list and matrices based on stored Isotopologues information
void NeutronWeights::createFromIsotopologues(const std::vector<std::shared_ptr<AtomType>> &exchangeableTypes)
{
    // Create the isotope mix from defined isotopologues
    isotopeMix_.create(isotopologueMixtures_, exchangeableTypes);

    calculateWeightingMatrices();

    valid_ = true;
}

// Return isotope mix
const IsotopeMix &NeutronWeights::isotopeMix() const { return isotopeMix_; }

// Return full scattering weights
const DoubleKeyedMap<double> &NeutronWeights::weights() const { return weights_; }

// Return concentration products
const DoubleKeyedMap<double> &NeutronWeights::concentrationProducts() const { return concentrationProducts_; }

// Return bound coherent scattering products
const DoubleKeyedMap<double> &NeutronWeights::boundCoherentProducts() const { return boundCoherentProducts_; }

// Return full intramolecular scattering weights
const DoubleKeyedMap<double> &NeutronWeights::intramolecularWeights() const { return intramolecularWeights_; }

// Return bound coherent average squared scattering (<b>**2)
double NeutronWeights::boundCoherentSquareOfAverage() const { return boundCoherentSquareOfAverage_; }

// Return bound coherent squared average scattering (<b**2>)
double NeutronWeights::boundCoherentAverageOfSquares() const { return boundCoherentAverageOfSquares_; }

// Return whether the structure is valid (i.e. has been finalised)
bool NeutronWeights::isValid() const { return valid_; }
