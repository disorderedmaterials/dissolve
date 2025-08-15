// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/atomTypeMix.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/coreData.h"
#include "data/elements.h"
#include "data/isotopes.h"
#include <utility>

// Finalise list, calculating fractional populations etc., and accounting for exchangeable sites in boundCoherent values
void AtomTypeMix::finalise(const std::vector<std::shared_ptr<AtomType>> &exchangeableTypes)
{
    // Set exchangeable flags
    for (auto &at : exchangeableTypes)
        exchangeables_.insert(at.get());

    for (auto &isotopeMix : std::views::values(mix_))
        isotopeMix.finalise();

    // Account for exchangeable atoms - form the average bound coherent scattering over all exchangeable atoms
    auto totalFraction = 0.0, boundCoherent = 0.0;
    for (auto &[atomType, isotopeMix] : mix_)
    {
        // If this type is not exchangeable, move on
        if (!exchangeables_.contains(atomType))
            continue;

        // Sum total atomic fraction and weighted bound coherent scattering length
        auto frac = fraction(atomType);
        totalFraction += frac;
        boundCoherent += frac * isotopeMix.boundCoherent();
    }
    boundCoherent /= totalFraction;

    // Now go back through the list and set the new scattering length for exchangeable components
    for (auto &[atomType, isotopeMix] : mix_)
    {
        // If this type is not exchangaeble, move on
        if (!exchangeables_.contains(atomType))
            continue;

        // Set the bound coherent scattering length of this component to the average of all exchangeable components
        isotopeMix.setBoundCoherent(boundCoherent);
    }
}

// Calculate and return full population of atom type in whole mix
double AtomTypeMix::population(const AtomType *atomType) const
{
    if (mix_.contains(atomType))
    {
        auto &topes = mix_.value(atomType).isotopes();
        return std::accumulate(topes.begin(), topes.end(), 0.0,
                               [](auto acc, const auto &isotope) { return acc + isotope.second; });
    }

    return 0.0;
}

// Calculate and return fractional population of atomtype in whole mix
double AtomTypeMix::fraction(const AtomType *atomType) const { return population(atomType) / totalPopulation_; }

// Create mix from Isotopologues
void AtomTypeMix::create(const std::vector<Isotopologues> &isotopologues,
                         const std::vector<std::shared_ptr<AtomType>> &exchangeableTypes)
{
    mix_.clear();
    totalPopulation_ = 0.0;
    exchangeables_.clear();

    // Loop over Isotopologues and add to the mix
    for (auto &topes : isotopologues)
    {
        // Get normalised Isotopologue populations and species atom type populations
        auto normalised = topes.normalised();

        // Loop over the Isotopologues in the mixture
        for (const auto &[iso, weight] : normalised)
        {
            // Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the
            // Isotopologue
            for (const auto &[atomType, atomTypePopulation] : topes.species()->atomTypePopulations())
            {
                mix_[atomType].add(iso->atomTypeIsotope(atomType), atomTypePopulation * weight * topes.speciesPopulation());
                totalPopulation_ += atomTypePopulation * weight * topes.speciesPopulation();
            }
        }
    }

    finalise(exchangeableTypes);
}

// Return types/topes map
const KeyedVector<const AtomType *, AtomTypeData> &AtomTypeMix::mix() const { return mix_; }

// Return whether specified atom type is exchangeable
bool AtomTypeMix::isExchangeable(const AtomType *atomType) const { return exchangeables_.contains(atomType); }

// Return indices of AtomType pair
std::optional<std::pair<int, int>> AtomTypeMix::indexOf(const AtomType *at1, const AtomType *at2) const
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
void AtomTypeMix::print() const
{
    Messenger::print("  AtomType  El  Isotope  Population      Fraction           bc (fm)\n");
    Messenger::print("  -----------------------------------------------------------------\n");
    for (auto &[atomType, isotopeMix] : mix_)
    {
        char exch = exchangeables_.contains(atomType) ? 'E' : ' ';

        // If there are isotopes defined, print them
        if (!isotopeMix.isotopes().empty())
        {
            Messenger::print("{} {:<8}  {:<3}    -     {:<10.1f}    {:10.6f} (of world) {:6.3f}\n", exch, atomType->name(),
                             Elements::symbol(atomType->Z()), population(atomType), fraction(atomType),
                             isotopeMix.boundCoherent());

            for (auto &[isotope, isotopePopulation] : isotopeMix.isotopes())
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
