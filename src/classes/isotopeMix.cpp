// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/isotopeMix.h"
#include "classes/atomType.h"
#include "classes/isotopologueSet.h"
#include "classes/species.h"
#include "data/elements.h"
#include "data/isotopes.h"

/*
 * Private Functions
 */

// Calculate and return the isolated bound coherent scattering (unexchanged) for the specified atom type
double IsotopeMix::isolatedBoundCoherent(const AtomType *atomType) const
{
    const auto &isotopes = mix_.value(atomType);
    return std::accumulate(isotopes.begin(), isotopes.end(), 0.0, [](const auto acc, const auto &isotope)
                           { return acc + isotope.second * Sears91::boundCoherent(isotope.first); }) /
           std::accumulate(isotopes.begin(), isotopes.end(), 0.0,
                           [](const auto acc, const auto &isotope) { return acc + isotope.second; });
}

/*
 * Public Functions
 */

// Calculate and return full population of atom type in whole mix
double IsotopeMix::population(const AtomType *atomType) const { return populations_.at(atomType); }

// Calculate and return fractional population of atom type in whole mix
double IsotopeMix::fraction(const AtomType *atomType) const { return population(atomType) / totalPopulation_; }

// Create mix from Isotopologues
void IsotopeMix::create(const std::map<const Species *, double> &speciesPopulations, const IsotopologueSet &isotopologues,
                        const Exchangeables &exchangeables)
{
    mix_.clear();
    populations_.clear();
    boundCoherent_.clear();
    totalPopulation_ = 0.0;

    // Loop over species / populations to establish atom type populations
    for (auto &[species, speciesPopulation] : speciesPopulations)
    {
        // Get the normalised populations for this species
        auto topes = isotopologues.normalisedIsotopologues(species);

        // Loop over the Isotopologues in the mixture
        for (const auto &[iso, weight] : topes)
        {
            // Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the
            // Isotopologue
            for (const auto &[atomType, atomTypePopulation] : species->atomTypePopulations())
            {
                auto population = speciesPopulation * atomTypePopulation * weight;

                auto &isotopes = mix_[atomType];
                isotopes[iso->atomTypeIsotope(atomType)] += population;
                populations_[atomType] += population;
                totalPopulation_ += population;
            }
        }
    }

    // Calculate total exchangeable atom scattering length
    auto exchangeableBoundCoherent = 0.0;
    auto exchangeableFraction = 0.0;
    for (const auto &atomType : std::views::keys(mix_))
    {
        if (exchangeables.contains(atomType->name()))
        {
            auto frac = fraction(atomType);
            exchangeableFraction += frac;
            exchangeableBoundCoherent += frac * isolatedBoundCoherent(atomType);
        }
    }
    exchangeableBoundCoherent /= exchangeableFraction;

    // Calculate bound coherent scattering lengths per atom type
    for (const auto &atomType : std::views::keys(mix_))
        boundCoherent_[atomType] =
            exchangeables.contains(atomType->name()) ? exchangeableBoundCoherent : isolatedBoundCoherent(atomType);
}

// Calculate and return bound coherent scattering, accounting for isotope mix and exchangeability
double IsotopeMix::boundCoherent(const AtomType *atomType) const { return boundCoherent_.at(atomType); }

// Return types/topes map
const KeyedVector<const AtomType *, std::map<Sears91::Isotope, double>> &IsotopeMix::mix() const { return mix_; }

// Return indices of AtomType pair
std::optional<std::pair<int, int>> IsotopeMix::indexOf(const AtomType *at1, const AtomType *at2) const
{
    auto count = 0, index = -1;
    for (auto &atomType : std::views::keys(mix_))
    {
        if (atomType == at1)
        {
            if (index == -1)
                index = count;
            else
                return {{count, index}};
        }
        if (atomType == at2)
        {
            if (index == -1)
                index = count;
            else
                return {{index, count}};
        }
        ++count;
    }

    return {};
}

// Print AtomType populations
void IsotopeMix::print(const Exchangeables &exchangeables) const
{
    Messenger::print("  AtomType  El  Isotope  Population      Fraction           bc (fm)\n");
    Messenger::print("  -----------------------------------------------------------------\n");
    for (auto &[atomType, isotopeMix] : mix_)
    {
        char exch = exchangeables.contains(atomType->name()) ? 'E' : ' ';

        // If there are isotopes defined, print them
        if (!isotopeMix.empty())
        {
            Messenger::print("{} {:<8}  {:<3}    -     {:<10.1f}    {:10.6f} (of world) {:6.3f}\n", exch, atomType->name(),
                             Elements::symbol(atomType->Z()), population(atomType), fraction(atomType),
                             boundCoherent(atomType));

            for (auto &[isotope, isotopePopulation] : isotopeMix)
            {
                Messenger::print("                   {:<3d}   {:<10.6e}  {:10.6f} (of type)  {:6.3f}\n", Sears91::A(isotope),
                                 isotopePopulation, isotopePopulation / population(atomType), Sears91::boundCoherent(isotope));
            }
        }
        else
            Messenger::print("{} {:<8}  {:<3}          {:<10.1f}  {:8.6f}     --- N/A ---\n", exch, atomType->name(),
                             Elements::symbol(atomType->Z()), population(atomType), fraction(atomType));

        Messenger::print("  -----------------------------------------------------------------\n");
    }
}
