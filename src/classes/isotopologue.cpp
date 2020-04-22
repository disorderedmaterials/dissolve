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

#include <algorithm>
#include "classes/isotopologue.h"
#include "base/processpool.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/isotopes.h"

// Constructor
Isotopologue::Isotopologue() : ListItem<Isotopologue>() { parent_ = NULL; }

// Destructor
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
	std::vector<std::tuple<AtomType*,std::shared_ptr<Isotope>>> oldItems;
	std::tuple<AtomType*, std::shared_ptr<Isotope>> rli;

	std::copy(isotopes_.rbegin(), isotopes_.rend(), oldItems.begin());
	
	// Loop over Atoms in species, get their assigned AtomTypes, and searching for them in the oldItems list
	for (SpeciesAtom *i = parent_->firstAtom(); i != NULL; i = i->next())
	{
		AtomType *at = i->atomType();
		if (at == NULL)
		{
			Messenger::error("NULL_POINTER - Found NULL AtomType pointer for Atom %i in Isotopologue::update().\n", i->userIndex());
			continue;
		}

		// If this AtomType is already in the list, we're done
		auto it = std::find_if(isotopes_.begin(), isotopes_.end(),
				       [&at](std::tuple<AtomType*, std::shared_ptr<Isotope>> item) {
					 return at == std::get<0>(item);
				       });
		if (it != isotopes_.end()) continue;
		
		// Otherwise, search for old item...
		it = std::find_if(oldItems.begin(), oldItems.end(),
				  [&at](std::tuple<AtomType*, std::shared_ptr<Isotope>> item) {
				    return at == std::get<0>(item);
				  });
		// If we found the existing item, append it to the local list. Otherwise, make a new entry
		if (it != isotopes_.end())
		{
			oldItems.erase(it);
			isotopes_.push_back(*it);
		}
		else isotopes_.emplace_back(at, Isotopes::naturalIsotope(at->element()));
	}
}

// Set Isotope associated to AtomType
bool Isotopologue::setAtomTypeIsotope(AtomType* at, std::shared_ptr<Isotope> isotope)
{
	// Check for NULL pointer
	if (at == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer passed to Isotopologue::setAtomTypeIsotope().\n");
		return false;
	}

	// Find the requested AtomType in the list
	// std::tuple<AtomType*,std::shared_ptr<Isotope>> rdi = isotopes_.contains(at);
	auto rdi = std::find_if(isotopes_.begin(),
				isotopes_.end(),
				[&at](std::tuple<AtomType*, std::shared_ptr<Isotope>> atom) {
				  return at == std::get<0>(atom);
				});
	if (rdi == isotopes_.end()) {
		Messenger::error("AtomType '%s' not found in Isotopologue '%s'.\n", at->name(), name_.get());
		return false;
	}
	
	std::get<1>(*rdi) = isotope;

	return true;
}

// Return Isotope for specified AtomType
std::shared_ptr<Isotope> Isotopologue::atomTypeIsotope(AtomType* at) const
{
	auto it = std::find_if(isotopes_.begin(), isotopes_.end(),
			       [&at](std::tuple<AtomType*, std::shared_ptr<Isotope>> item) {
				 return at == std::get<0>(item);
			       });
	if (it == isotopes_.end())
	{
		Messenger::error("Couldn't retrieve AtomType '%s' from Isotopologue '%s' as it doesn't exist.\n", at->name(), name_.get());
		return nullptr;
	}
	return std::get<1>(*it);
}

// Return AtomType/Isotope pairs list
const std::vector<std::tuple<AtomType*,std::shared_ptr<Isotope>>>& Isotopologue::isotopes() const
{
	return isotopes_;
}

// Return nth AtomType/Isotope pair
std::tuple<AtomType*,std::shared_ptr<Isotope>> Isotopologue::isotope(int n)
{
	return isotopes_[n];
}
