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
    auto nTypes = isotopeMix_.mix().size();
    concentrationProducts_.initialise(nTypes, nTypes, true);
    boundCoherentProducts_.initialise(nTypes, nTypes, true);
    weights_.initialise(nTypes, nTypes, true);
    intramolecularWeights_.initialise(nTypes, nTypes, true);
    boundCoherentAverageOfSquares_ = 0.0;
    boundCoherentSquareOfAverage_ = 0.0;
    double ci, cj, bi, bj;

    // Determine atomic concentration products, bound coherent products, and full scattering weights
    dissolve::for_each_pair(ParallelPolicies::seq, isotopeMix_.mix(),
                            [&](int indexI, const auto &typeMixI, int indexJ, const auto &typeMixJ)
                            {
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

                                concentrationProducts_[{indexI, indexJ}] = ci * cj;
                                boundCoherentProducts_[{indexI, indexJ}] = bi * bj;
                                weights_[{indexI, indexJ}] = ci * cj * bi * bj * (indexI == indexJ ? 1 : 2);
                            });

    // Finalise <b>**2
    boundCoherentSquareOfAverage_ *= boundCoherentSquareOfAverage_;

    // Determine bound (intramolecular) scattering weights
    // Loop over defined Isotopologues in our defining mixtures, summing terms from (intramolecular) pairs of Atoms
    intramolecularWeights_ = 0.0;
    Array2D<double> intraNorm(nTypes, nTypes, true);
    Array2D<char> globalFlag(nTypes, nTypes, true);
    intraNorm = 0.0;
    globalFlag = false;
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

                                        intramolecularWeights_[{typeI, typeJ}] += weight * bi * bj;
                                        intraNorm[{typeI, typeJ}] += weight;
                                        globalFlag[{typeI, typeJ}] = true;
                                    });
        }
    }

    // Normalise the boundWeights_ array, and multiply by atomic concentrations and Kronecker delta
    dissolve::for_each_pair(ParallelPolicies::seq, isotopeMix_.mix(),
                            [&](int indexI, const auto &typeMixI, int indexJ, const auto &typeMixJ)
                            {
                                // Skip this pair if there are no such intramolecular interactions
                                if (!globalFlag[{indexI, indexJ}])
                                    return;

                                ci = isotopeMix_.fraction(typeMixI.first);
                                cj = isotopeMix_.fraction(typeMixJ.first);

                                intramolecularWeights_[{indexI, indexJ}] /= intraNorm[{indexI, indexJ}];
                                intramolecularWeights_[{indexI, indexJ}] *= ci * cj * (indexI == indexJ ? 1 : 2);
                            });
}

// Create from species populations and isotopologues
void NeutronWeights::create(const KeyedVector<const Species *, double> &populations, const IsotopologueSet &isotopologues,
                            const std::vector<std::shared_ptr<AtomType>> &exchangeableTypes)
{
    clear();

    for (auto &[sp, pop] : populations)
    {
        // Find the defined Isotopologue for this Species - if it doesn't exist, use the Natural one
        auto isoRef = isotopologues.getIsotopologues(sp);
        if (isoRef)
        {
            const Isotopologues &topes = *isoRef;
            for (const auto &[iso, weight] : topes.mix())
                addIsotopologue(sp, pop, iso, weight);
        }
        else
            addIsotopologue(sp, pop, sp->naturalIsotopologue(), 1.0);
    }

    createFromIsotopologues(exchangeableTypes);
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

// Return number of used AtomTypes
int NeutronWeights::nUsedTypes() const { return isotopeMix_.mix().size(); }

// Return concentration product for types i and j
double NeutronWeights::concentrationProduct(int i, int j) const { return concentrationProducts_[{i, j}]; }

// Return bound coherent scattering product for types i
double NeutronWeights::boundCoherentProduct(int i, int j) const { return boundCoherentProducts_[{i, j}]; }

// Return full weighting for types i and j (ci * cj * bi * bj * [2-dij])
double NeutronWeights::weight(int i, int j) const { return weights_[{i, j}]; }

// Return full intramolecular weighting for types i and j
double NeutronWeights::intramolecularWeight(int i, int j) const { return intramolecularWeights_[{i, j}]; }

// Return full weights matrix
const Array2D<double> &NeutronWeights::weights() const { return weights_; }

// Return full intramolecular scattering weights matrix
const Array2D<double> &NeutronWeights::intramolecularWeights() const { return intramolecularWeights_; }

// Return bound coherent average squared scattering (<b>**2)
double NeutronWeights::boundCoherentSquareOfAverage() const { return boundCoherentSquareOfAverage_; }

// Return bound coherent squared average scattering (<b**2>)
double NeutronWeights::boundCoherentAverageOfSquares() const { return boundCoherentAverageOfSquares_; }

// Return whether the structure is valid (i.e. has been finalised)
bool NeutronWeights::isValid() const { return valid_; }
