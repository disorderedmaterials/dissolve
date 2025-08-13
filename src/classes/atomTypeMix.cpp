// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/atomTypeMix.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/coreData.h"
#include "data/elements.h"
#include "data/isotopes.h"
#include <algorithm>
#include <utility>

void AtomTypeMix::operator=(const AtomTypeMix &source) { types_ = source.types_; }

AtomTypeData &AtomTypeMix::operator[](int n) { return types_[n]; }

const AtomTypeData &AtomTypeMix::operator[](int n) const { return types_[n]; }

/*
 * Type List
 */

// Clear all data
void AtomTypeMix::clear() { types_.clear(); }

// Add the specified AtomType to the list, returning data object and its index in the vector
std::pair<AtomTypeData &, int> AtomTypeMix::add(const AtomType *atomType, double population)
{
    // Search the list for the AtomType provided.
    auto atd =
        std::find_if(types_.begin(), types_.end(), [&atomType](const auto &data) { return data.atomType() == atomType; });

    // Return the entry if we found it
    if (atd != types_.end())
    {
        atd->add(population);
        return {*atd, atd - types_.begin()};
    }

    auto &newAtomTypeData = types_.emplace_back(atomType, population);
    return {newAtomTypeData, types_.size() - 1};
}

// Add/increase this AtomType/Isotope pair
void AtomTypeMix::addIsotope(const AtomType *atomType, Sears91::Isotope tope, double popAdd)
{
    auto &atd = std::get<0>(add(atomType));
    atd.add(tope, popAdd);
}

// Finalise list, calculating fractional populations etc.
void AtomTypeMix::finalise()
{
    auto total = totalPopulation();
    for (auto &atd : types_)
        atd.finalise(total);
}

// Finalise list, calculating fractional populations etc., and accounting for exchangeable sites in boundCoherent values
void AtomTypeMix::finalise(const std::vector<std::shared_ptr<AtomType>> &exchangeableTypes)
{
    // Perform basic tasks
    finalise();

    // Account for exchangeable atoms - form the average bound coherent scattering over all exchangeable atoms
    double totalFraction = 0.0, boundCoherent = 0.0;
    for (auto &atd : types_)
    {
        // If this type is not exchangeable, move on
        if (std::find_if(exchangeableTypes.begin(), exchangeableTypes.end(),
                         [&atd](const auto &exchType) { return atd.atomType() == exchType.get(); }) == exchangeableTypes.end())
            continue;

        // Sum total atomic fraction and weighted bound coherent scattering length
        totalFraction += atd.fraction();
        boundCoherent += atd.fraction() * atd.boundCoherent();
    }
    boundCoherent /= totalFraction;

    // Now go back through the list and set the new scattering length for exchangeable components
    for (auto &atd : types_)
    {
        // If this type is not exchangaeble, move on
        if (std::find_if(exchangeableTypes.begin(), exchangeableTypes.end(),
                         [&atd](const auto &exchType) { return atd.atomType() == exchType.get(); }) == exchangeableTypes.end())
            continue;

        // Set the bound coherent scattering length of this component to the average of all exchangable components
        atd.setBoundCoherent(boundCoherent);
        atd.setAsExchangeable();
    }
}

// Check for presence of AtomType
bool AtomTypeMix::contains(const AtomType *atomType) const
{
    return std::find_if(types_.begin(), types_.end(), [atomType](const auto &atd) { return atd.atomType() == atomType; }) !=
           types_.end();
}

// Return number of AtomType/Isotopes
int AtomTypeMix::nItems() const { return types_.size(); }

// Return size of the mix (equivalent to nItems(), added for standard container "compliance")
int AtomTypeMix::size() const { return types_.size(); }

// Return first item
const AtomTypeData &AtomTypeMix::first() const { return types_.front(); }

// Return starting iterator
std::vector<AtomTypeData>::const_iterator AtomTypeMix::begin() const { return types_.begin(); }

// Return ending iterator
std::vector<AtomTypeData>::const_iterator AtomTypeMix::end() const { return types_.end(); }

// Return index of AtomType
std::optional<int> AtomTypeMix::indexOf(const AtomType *atomType) const
{
    auto it = std::find_if(types_.begin(), types_.end(), [atomType](const auto &atd) { return atd.atomType() == atomType; });
    if (it == types_.end())
        return {};
    else
        return it - types_.begin();
}

// Return index of names AtomType
std::optional<int> AtomTypeMix::indexOf(std::string_view name) const
{
    auto it = std::find_if(types_.begin(), types_.end(),
                           [name](const auto &atd) { return DissolveSys::sameString(atd.atomType()->name(), name); });
    if (it == types_.end())
        return {};
    else
        return it - types_.begin();
}

// Return indices of AtomType pair
std::optional<std::pair<int, int>> AtomTypeMix::indexOf(const AtomType *at1, const AtomType *at2) const
{
    auto count = 0, index = -1;
    for (auto &atd : types_)
    {
        if (atd.atomType() == at1)
        {
            if (index == -1)
                index = count;
            else
                return {{count, index}};
        }
        if (atd.atomType() == at2)
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

// Return total population of all types
double AtomTypeMix::totalPopulation() const
{
    double total = 0;
    for (auto &atd : types_)
        total += atd.population();
    return total;
}

// Return nth referenced AtomType
const AtomType *AtomTypeMix::atomType(int n) const
{
    assert(n >= 0 && n < types_.size());

    return types_[n].atomType();
}

// Return AtomTypeData for specified AtomType
OptionalReferenceWrapper<const AtomTypeData> AtomTypeMix::atomTypeData(const AtomType *atomType) const
{
    auto it = std::find_if(types_.begin(), types_.end(), [&atomType](const auto &atd) { return atomType == atd.atomType(); });
    if (it == types_.end())
        return {};
    return *it;
}

// Print AtomType populations
void AtomTypeMix::print() const
{
    Messenger::print("  AtomType  El  Isotope  Population      Fraction           bc (fm)\n");
    Messenger::print("  -----------------------------------------------------------------\n");
    for (auto &atd : types_)
    {
        char exch = atd.exchangeable() ? 'E' : ' ';

        // If there are isotopes defined, print them
        if (!atd.isotopeData().empty())
        {
            Messenger::print("{} {:<8}  {:<3}    -     {:<10d}    {:10.6f} (of world) {:6.3f}\n", exch, atd.atomTypeName(),
                             Elements::symbol(atd.atomType()->Z()), atd.population(), atd.fraction(), atd.boundCoherent());

            for (auto &topeData : atd.isotopeData())
            {
                Messenger::print("                   {:<3d}   {:<10.6e}  {:10.6f} (of type)  {:6.3f}\n",
                                 Sears91::A(topeData.isotope()), topeData.population(), topeData.fraction(),
                                 Sears91::boundCoherent(topeData.isotope()));
            }
        }
        else
            Messenger::print("{} {:<8}  {:<3}          {:<10d}  {:8.6f}     --- N/A ---\n", exch, atd.atomTypeName(),
                             Elements::symbol(atd.atomType()->Z()), atd.population(), atd.fraction());

        Messenger::print("  -----------------------------------------------------------------\n");
    }
}
