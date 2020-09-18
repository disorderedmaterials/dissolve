/*
    *** AtomTypeList Definition
    *** src/classes/atomtypelist.cpp
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

#include "classes/atomtypelist.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/isotopedata.h"
#include "data/elements.h"
#include "data/isotopes.h"
#include "templates/broadcastlist.h"
#include "templates/broadcastvector.h"
#include <algorithm>
#include <string.h>

AtomTypeList::AtomTypeList() {}

AtomTypeList::~AtomTypeList() {}

AtomTypeList::AtomTypeList(const AtomTypeList &source) { (*this) = source; }

void AtomTypeList::operator=(const AtomTypeList &source) { types_ = source.types_; }

// Array access operator
AtomTypeData &AtomTypeList::operator[](unsigned int n)
{
#ifdef CHECKS
    if ((n < 0) || (n >= types_.size()))
    {
        Messenger::print("OUT_OF_RANGE - Specified index {} out of range in AtomTypeList::operator[].\n", n);
    }
#endif
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
    for (auto &newType : source)
    {
        AtomTypeData &atd = add(newType.atomType());

        // Now add Isotope data
        for (auto *topeData = newType.isotopeData(); topeData != nullptr; topeData = topeData->next())
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
    auto &atd = add(atomType, 0);

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
std::shared_ptr<AtomType> AtomTypeList::atomType(int n)
{
#ifdef CHECKS
    if ((n < 0) || (n >= types_.size()))
    {
        Messenger::print("OUT_OF_RANGE - Specified index {} out of range in AtomTypeList::atomType().\n");
    }
#endif
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
                             atd.atomType()->element()->symbol(), atd.population(), atd.fraction(), atd.boundCoherent());

            for (const auto *topeData = atd.isotopeData(); topeData != nullptr; topeData = topeData->next())
            {
                Messenger::print("                   {:<3d}   {:<10.6e}  {:10.6f} (of type)  {:6.3f}\n",
                                 topeData->isotope()->A(), topeData->population(), topeData->fraction(),
                                 topeData->isotope()->boundCoherent());
            }
        }
        else
            Messenger::print("{} {:<8}  {:<3}          {:<10d}  {:8.6f}     --- N/A ---\n", exch, atd.atomTypeName(),
                             atd.atomType()->element()->symbol(), atd.population(), atd.fraction());

        Messenger::print("  -----------------------------------------------------------------\n");
    }
}

/*
 * GenericItemBase Implementations
 */

// Return class name
std::string_view AtomTypeList::itemClassName() { return "AtomTypeList"; }

// Read data through specified LineParser
bool AtomTypeList::read(LineParser &parser, CoreData &coreData)
{
    types_.clear();

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nItems = parser.argi(0);
    for (int n = 0; n < nItems; ++n)
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

        // types_.emplace_back(types_.size(), atomType, population);
        types_.emplace_back(atomType, population, fraction, boundCoherent);
        auto &atd = types_.back();
        for (int i = 0; i < nIsotopes; ++i)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            auto isotope = Isotopes::isotope(parser.argi(0), parser.argi(1));
            atd.add(isotope, parser.argd(2));
        }
    }

    return true;
}

// Write data through specified LineParser
bool AtomTypeList::write(LineParser &parser)
{
    if (!parser.writeLineF("{}  # nItems\n", types_.size()))
        return false;
    for (auto &atd : types_)
        if (!atd.write(parser))
            return false;

    return true;
}

/*
 * Parallel Comms
 */

// Broadcast item contents
bool AtomTypeList::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    int count;
    if (procPool.isMaster())
    {
        // Broadcast number of items in list, then list items...
        count = types_.size();
        if (!procPool.broadcast(count, root))
            return false;
        for (auto &type : types_)
        {
            std::string name{type.atomType()->name()};
            if (!procPool.broadcast(name, root))
                return false;
            if (!type.broadcast(procPool, root, coreData))
                return false;
        }
    }
    else
    {
        // Get number of list items to expect
        if (!procPool.broadcast(count, root))
            return false;

        // Clear list and reconstruct
        types_.clear();
        for (int n = 0; n < count; ++n)
        {
            // Slaves must create a suitable structure first, and then join the broadcast
            std::string typeName;
            if (!procPool.broadcast(typeName), root)
                return false;
            auto atomType = coreData.findAtomType(typeName);
            types_.emplace_back(atomType);
            auto &item = types_.back();
            if (!item.broadcast(procPool, root, coreData))
                return false;
        }
    }

    // All OK - success!
#endif
    return true;
}

// Check item equality
bool AtomTypeList::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    // Check number of types in list first
    if (!procPool.equality((long int)types_.size()))
        return Messenger::error("AtomTypeList size is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                types_.size());
    for (auto &atd : types_)
        if (!atd.equality(procPool))
            return Messenger::error("AtomTypeList entry for type '{}' is not equivalent.\n", atd.atomTypeName());
#endif
    return true;
}
