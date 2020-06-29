/*
    *** Isotopologue Definition
    *** src/classes/isotopologue.cpp
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

#include "classes/isotopologue.h"
#include "base/processpool.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/isotopes.h"

Isotopologue::Isotopologue() : ListItem<Isotopologue>() { parent_ = NULL; }

Isotopologue::~Isotopologue() {}

/*
 * Basic Information
 */

// Set parent Species
void Isotopologue::setParent(Species *parent) { parent_ = parent; }

// Return parent Species
Species *Isotopologue::parent() const { return parent_; }

// Set name of Isotopologue
void Isotopologue::setName(const char *name) { name_ = name; }

// Return name of Isotopologue
const char *Isotopologue::name() const { return name_.get(); }

/*
 * Isotope Definition
 */

// Update AtomType/Isotope RefList
void Isotopologue::update()
{
    /*
     * This function reconstructs the current RefList of AtomType/Isotope pairs and ensures that
     * it contains the same Atom ordering (and Atom contents) as the parent Species.  It can (and should) be
     * called after any change to Atom ordering or creation/deletion in the Species.
     */

    // Check for valid parent_
    if (parent_ == NULL)
    {
        Messenger::error("NULL_POINTER - Found NULL parent_ pointer in Isotopologue::update().\n");
        return;
    }

    // Construct a temporary RefList, and move all existing RefListItems to it
    std::vector<std::tuple<std::shared_ptr<AtomType>, Isotope *>> oldItems;
    std::tuple<std::shared_ptr<AtomType>, Isotope *> rli;
    while (std::get<std::shared_ptr<AtomType>>(isotopes_.back()) != nullptr)
    {
        rli = isotopes_.back();
        isotopes_.erase(std::remove(isotopes_.begin(), isotopes_.end(), rli));
        oldItems.push_back(rli);
    }

    // Loop over Atoms in species, get their assigned AtomTypes, and searching for them in the oldItems list
    for (auto *i = parent_->firstAtom(); i != NULL; i = i->next())
    {
        std::shared_ptr<AtomType> at = i->atomType();
        if (at == NULL)
        {
            Messenger::error("NULL_POINTER - Found NULL AtomType pointer for Atom %i in Isotopologue::update().\n",
                             i->userIndex());
            continue;
        }

        // If this AtomType is already in the list, we're done
        auto it = std::find_if(isotopes_.begin(), isotopes_.end(), [&at](auto value) { return std::get<0>(value) == at; });
        if (it != isotopes_.end())
            continue;

        // Otherwise, search for old item...
        it = std::find_if(oldItems.begin(), oldItems.end(), [&at](auto value) { return std::get<0>(value) == at; });
        // rli = oldItems.contains(at);
        // If we found the existing item, append it to the local list. Otherwise, make a new entry
        if (it != oldItems.end())
        {
            isotopes_.push_back(*it);
            oldItems.erase(it);
        }
        else
            isotopes_.emplace_back(at, Isotopes::naturalIsotope(at->element()));
    }
}

// Set Isotope associated to AtomType
bool Isotopologue::setAtomTypeIsotope(std::shared_ptr<AtomType> at, Isotope *isotope)
{
    // Check for NULL pointer
    if (at == NULL)
    {
        Messenger::error("NULL_POINTER - NULL AtomType pointer passed to Isotopologue::setAtomTypeIsotope().\n");
        return false;
    }

    // Find the requested AtomType in the list
    auto it = std::find_if(isotopes_.begin(), isotopes_.end(), [&at](auto value) { return std::get<0>(value) == at; });
    // RefDataItem<AtomType, Isotope *> *rdi = isotopes_.contains(at);
    if (it == isotopes_.end())
    {
        Messenger::error("AtomType '%s' not found in Isotopologue '%s'.\n", at->name(), name_.get());
        return false;
    }

    std::get<1>(*it) = isotope;

    return true;
}

// Return Isotope for specified AtomType
Isotope *Isotopologue::atomTypeIsotope(std::shared_ptr<AtomType> at) const
{
    auto it = std::find_if(isotopes_.begin(), isotopes_.end(), [&at](auto value) { return std::get<0>(value) == at; });
    // RefDataItem<AtomType, Isotope *> *rdi = isotopes_.contains(at);
    if (it == isotopes_.end())
    {
        Messenger::error("Couldn't retrieve AtomType '%s' from Isotopologue '%s' as it doesn't exist.\n", at->name(),
                         name_.get());
        return NULL;
    }
    return std::get<1>(*it);
}

// Return AtomType/Isotope pairs list
const std::vector<std::tuple<std::shared_ptr<AtomType>, Isotope *>> &Isotopologue::isotopes() const { return isotopes_; }

// Return nth AtomType/Isotope pair
std::tuple<std::shared_ptr<AtomType>, Isotope *> &Isotopologue::isotope(int n) { return isotopes_[n]; }
