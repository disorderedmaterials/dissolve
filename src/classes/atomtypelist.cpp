// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtypelist.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/isotopedata.h"
#include "data/elements.h"
#include "data/isotopes.h"
#include <algorithm>

AtomTypeList::AtomTypeList() {}

AtomTypeList::~AtomTypeList() {}

AtomTypeList::AtomTypeList(const AtomTypeList &source) { (*this) = source; }

void AtomTypeList::operator=(const AtomTypeList &source) { types_ = source.types_; }

// Array access operator
AtomTypeData &AtomTypeList::operator[](int n)
{
    assert(n >= 0 && n < types_.size());

    return types_[n];
}

/*
 * Type List
 */

// Clear all data
void AtomTypeList::clear() { types_.clear(); }

// Zero populations of all types in the list
void AtomTypeList::zero()
{
    for (auto &atd : types_)
        atd.zeroPopulations();
}

// Add the specified AtomType to the list, returning the index of the AtomType in the list
AtomTypeData &AtomTypeList::add(std::shared_ptr<AtomType> atomType, double population)
{
    // Search the list for the AtomType provided.
    auto atd =
        std::find_if(types_.begin(), types_.end(), [&atomType](const auto &data) { return data.atomType() == atomType; });

    // Return the entry if we found it
    if (atd != types_.end())
    {
        atd->add(population);
        return *atd;
    }

    types_.emplace_back(types_.size(), atomType, 0);
    types_.back().add(population);
    return types_.back();
}

// Add the AtomTypes in the supplied list into this one, increasing populations etc.
void AtomTypeList::add(const AtomTypeList &source)
{
    // Loop over AtomTypes in the source list
    for (auto &otherType : source)
    {
        AtomTypeData &atd = add(otherType.atomType());

        // If no Isotope data are present, add the population now. Otherwise, add it via the isotopes...
        if (otherType.nIsotopes() == 0)
            atd.add(otherType.population());
        else
            for (auto *topeData = otherType.isotopeData(); topeData != nullptr; topeData = topeData->next())
                atd.add(topeData->isotope(), topeData->population());
    }
}

// Remove specified AtomType from the list
void AtomTypeList::remove(std::shared_ptr<AtomType> atomType)
{
    types_.erase(
        std::remove_if(types_.begin(), types_.end(), [&atomType](const auto &atd) { return atd.atomType() == atomType; }));
}

// Add/increase this AtomType/Isotope pair
void AtomTypeList::addIsotope(std::shared_ptr<AtomType> atomType, Isotope *tope, double popAdd)
{
    auto &atd = add(atomType);

    // Add / increase isotope population
    if (tope != nullptr)
        atd.add(tope, popAdd);
}

// Finalise list, calculating fractional populations etc.
void AtomTypeList::finalise()
{
    // Finalise AtomTypeData
    double total = totalPopulation();
    for (auto &atd : types_)
        atd.finalise(total);
}

// Finalise list, calculating fractional populations etc., and accounting for exchangeable sites in boundCoherent values
void AtomTypeList::finalise(const AtomTypeList &exchangeable)
{
    // Perform basic tasks
    finalise();

    // Account for exchangeable atoms - form the average bound coherent scattering over all exchangeable atoms
    double totalFraction = 0.0, boundCoherent = 0.0;
    for (auto &atd : types_)
    {
        // If this type is not exchangable, move on
        if (!exchangeable.contains(atd.atomType()))
            continue;

        // Sum total atomic fraction and weighted bound coherent scattering length
        totalFraction += atd.fraction();
        boundCoherent += atd.fraction() * atd.boundCoherent();
    }
    boundCoherent /= totalFraction;

    // Now go back through the list and set the new scattering length for exchangeable components
    for (auto &atd : types_)
    {
        // If this type is not exchangable, move on
        if (!exchangeable.contains(atd.atomType()))
            continue;

        // Set the bound coherent scattering length of this component to the average of all exchangable components
        atd.setBoundCoherent(boundCoherent);
        atd.setAsExchangeable();
    }
}

// Make all AtomTypeData in the list reference only their natural isotope
void AtomTypeList::naturalise()
{
    // Loop over AtomTypes in the source list
    for (auto &atd : types_)
        atd.naturalise();
}

// Check for presence of AtomType in list
bool AtomTypeList::contains(std::shared_ptr<AtomType> atomType) const
{
    for (auto &atd : types_)
        if (atd.atomType() == atomType)
            return true;

    return false;
}

// Check for presence of AtomType/Isotope pair in list
bool AtomTypeList::contains(std::shared_ptr<AtomType> atomType, Isotope *tope)
{
    for (auto &atd : types_)
    {
        if (atd.atomType() != atomType)
            continue;
        if (!atd.hasIsotope(tope))
            continue;
        return true;
    }

    return false;
}

// Return number of AtomType/Isotopes in list
int AtomTypeList::nItems() const { return types_.size(); }

// Return first item in list
const AtomTypeData &AtomTypeList::first() const { return types_.front(); }

// Return starting iterator
std::vector<AtomTypeData>::const_iterator AtomTypeList::begin() const { return types_.begin(); }

// Return ending iterator
std::vector<AtomTypeData>::const_iterator AtomTypeList::end() const { return types_.end(); }

// Return index of AtomType in list
int AtomTypeList::indexOf(std::shared_ptr<AtomType> atomtype) const
{
    auto count = 0;
    for (auto &atd : types_)
    {
        if (atd.atomType() == atomtype)
            return count;
        ++count;
    }

    return -1;
}

// Return index of names AtomType in list
int AtomTypeList::indexOf(std::string_view name) const
{
    auto count = 0;
    for (auto &atd : types_)
    {
        if (DissolveSys::sameString(atd.atomType()->name(), name))
            return count;
        ++count;
    }

    return -1;
}

// Return total population of all types in list
double AtomTypeList::totalPopulation() const
{
    double total = 0;
    for (auto &atd : types_)
        total += atd.population();
    return total;
}

// Return nth referenced AtomType
const std::shared_ptr<AtomType> AtomTypeList::atomType(int n) const
{
    assert(n >= 0 && n < types_.size());

    return types_[n].atomType();
}

// Return AtomTypeData for specified AtomType
OptionalReferenceWrapper<const AtomTypeData> AtomTypeList::atomTypeData(std::shared_ptr<AtomType> atomType)
{
    auto it = std::find_if(types_.begin(), types_.end(), [&atomType](const auto &atd) { return atomType == atd.atomType(); });
    if (it == types_.end())
        return {};
    return *it;
}

// Print AtomType populations
void AtomTypeList::print() const
{
    Messenger::print("  AtomType  El  Isotope  Population      Fraction           bc (fm)\n");
    Messenger::print("  -----------------------------------------------------------------\n");
    for (auto &atd : types_)
    {
        char exch = atd.exchangeable() ? 'E' : ' ';

        // If there are isotopes defined, print them
        if (atd.isotopeData())
        {
            Messenger::print("{} {:<8}  {:<3}    -     {:<10d}    {:10.6f} (of world) {:6.3f}\n", exch, atd.atomTypeName(),
                             Elements::symbol(atd.atomType()->Z()), atd.population(), atd.fraction(), atd.boundCoherent());

            for (const auto *topeData = atd.isotopeData(); topeData != nullptr; topeData = topeData->next())
            {
                Messenger::print("                   {:<3d}   {:<10.6e}  {:10.6f} (of type)  {:6.3f}\n",
                                 topeData->isotope()->A(), topeData->population(), topeData->fraction(),
                                 topeData->isotope()->boundCoherent());
            }
        }
        else
            Messenger::print("{} {:<8}  {:<3}          {:<10d}  {:8.6f}     --- N/A ---\n", exch, atd.atomTypeName(),
                             Elements::symbol(atd.atomType()->Z()), atd.population(), atd.fraction());

        Messenger::print("  -----------------------------------------------------------------\n");
    }
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool AtomTypeList::deserialise(LineParser &parser, const CoreData &coreData)
{
    types_.clear();

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nItems = parser.argi(0);
    for (auto n = 0; n < nItems; ++n)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        auto atomType = coreData.findAtomType(parser.argsv(0));
        if (!atomType)
            return Messenger::error("Could not find atom type {}.", parser.argsv(0));
        auto population = parser.argd(1);
        auto fraction = parser.argd(2);
        auto boundCoherent = parser.argd(3);
        auto nIsotopes = parser.argi(4);

        types_.emplace_back(atomType, population, fraction, boundCoherent);
        auto &atd = types_.back();
        for (auto i = 0; i < nIsotopes; ++i)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            auto Z = Elements::element(parser.argi(0));
            auto isotope = Isotopes::isotope(Z, parser.argi(1));
            atd.add(isotope, parser.argd(2));
        }
    }

    return true;
}

// Write data through specified LineParser
bool AtomTypeList::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("{}  # nItems\n", types_.size()))
        return false;
    for (auto &atd : types_)
        if (!atd.serialise(parser))
            return false;

    return true;
}
