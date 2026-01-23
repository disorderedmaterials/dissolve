// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/neutronWeights.h"
#include "base/lineParser.h"
#include "classes/atomType.h"
#include "classes/isotopologueSet.h"
#include "classes/species.h"
#include "data/isotopes.h"
#include "items/deserialisers.h"
#include "templates/algorithms.h"

NeutronWeights::NeutronWeights(const std::map<const Species *, double> &speciesPopulations,
                               const IsotopologueSet &isotopologues, const Exchangeables &exchangeables)
{
    boundCoherentSquareOfAverage_ = 0.0;
    boundCoherentAverageOfSquares_ = 0.0;

    // Create the isotope mix from defined isotopologues
    isotopeMix_.create(speciesPopulations, isotopologues, exchangeables);

    calculateWeightingMatrices(speciesPopulations, isotopologues, exchangeables);

    Messenger::print("  Species          nMols       Isotopologue     Weight\n");
    Messenger::print("  ------------------------------------------------------\n");
    for (auto &[species, speciesPopulation] : speciesPopulations)
    {
        auto first = true;
        for (auto &[iso, isoFraction] : isotopologues.normalisedIsotopologues(species))
        {
            if (first)
                Messenger::print("  {:<15}  {:<10g}  {:<15}  {}\n", species->name(), speciesPopulation, iso->name(),
                                 isoFraction);
            else
                Messenger::print("                            {:<15}  {}\n", iso->name(), isoFraction);

            first = false;
        }
    }

    // Print atomtypes table
    Messenger::print("\n");
    isotopeMix_.print(exchangeables);

    Messenger::print("\nCalculated average scattering lengths: <b>**2 = {:.5f}, <b**2> = {:.5f}\n",
                     boundCoherentSquareOfAverage_, boundCoherentAverageOfSquares_);
}

NeutronWeights::NeutronWeights(const NeutronWeights &source) { (*this) = source; }

void NeutronWeights::operator=(const NeutronWeights &source)
{
    isotopeMix_ = source.isotopeMix_;
    boundCoherentProducts_ = source.boundCoherentProducts_;
    concentrationProducts_ = source.concentrationProducts_;
    weights_ = source.weights_;
    intramolecularWeights_ = source.intramolecularWeights_;
    boundCoherentSquareOfAverage_ = source.boundCoherentSquareOfAverage_;
    boundCoherentAverageOfSquares_ = source.boundCoherentAverageOfSquares_;
}

/*
 * Data
 */

// Calculate weighting matrices based on current AtomType / Isotope information
void NeutronWeights::calculateWeightingMatrices(const std::map<const Species *, double> &speciesPopulations,
                                                const IsotopologueSet &isotopologues, const Exchangeables &exchangeables)
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
    for (auto &[species, speciesPopulation] : speciesPopulations)
    {
        for (auto &[iso, isoFraction] : isotopologues.normalisedIsotopologues(species))
        {
            // Sum the scattering lengths of each pair of AtomTypes, weighted by the species population and the
            // fractional Isotopologue weight in the mix.
            dissolve::for_each_pair(
                ParallelPolicies::seq, species->atomTypePopulations(),
                [&, iso, isoFraction, speciesPopulation](int indexI, const auto &atPop1, int indexJ, const auto &atPop2)
                {
                    DoubleKeyedMapKey key{atPop1.first->name(), atPop2.first->name()};

                    // If an AtomType is exchangeable we use its exchanged bound coherent scattering length
                    bi = exchangeables.contains(atPop1.first->name())
                             ? isotopeMix_.boundCoherent(atPop1.first)
                             : Sears91::boundCoherent(iso->atomTypeIsotope(atPop1.first));
                    bj = exchangeables.contains(atPop2.first->name())
                             ? isotopeMix_.boundCoherent(atPop2.first)
                             : Sears91::boundCoherent(iso->atomTypeIsotope(atPop2.first));

                    // Convert from fm to barns
                    bi *= 0.1;
                    bj *= 0.1;

                    intramolecularWeights_[key] += speciesPopulation * isoFraction * bi * bj;
                    intraNorm[key] += speciesPopulation * isoFraction;
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
