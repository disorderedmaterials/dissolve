/*
	*** Isotopologue Set
	*** src/classes/isotopologueset.cpp
	Copyright T. Youngs 2012-2016

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/isotopologueset.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "base/processpool.h"

// Constructor
IsotopologueSet::IsotopologueSet()
{
}

// Copy Constructor
IsotopologueSet::IsotopologueSet(const IsotopologueSet& source)
{
	(*this) = source;
}

// Assignment Operator
void IsotopologueSet::operator=(const IsotopologueSet& source)
{
	// Isotopologue Mix
	isotopologueMixtures_ = source.isotopologueMixtures_;
	atomTypes_ = source.atomTypes_;
}

/*
 * Species/Isotopologue Definition
 */

// Return whether the IsotopologueSet contains a mixtures definition for the provided Species
IsotopologueMix* IsotopologueSet::hasSpeciesIsotopologueMixture(Species* sp) const
{
	/*
	 * This function reconstructs the current RefList of Species/Isotopologue pairs and ensures that
	 * it contains only valid Species and Isotopologue pointers.
	 */
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next) if (mix->species() == sp) return mix;
	return NULL;
}

// 			case (Keywords::IsotopologueModuleKeyword):
// 				// Locate Species first...
// 				sp = duq->findSpecies(parser.argc(1));
// 				if (sp == NULL)
// 				{
// 					Messenger::error("Module refers to Species '%s', but no such Species is defined.\n", parser.argc(1));
// 					error = true;
// 					break;
// 				}
// 				
// 				// Now locate Isotopologue
// 				iso = sp->findIsotopologue(parser.argc(2));
// 				if (iso == NULL)
// 				{
// 					Messenger::error("Module refers to Isotopologue '%s' in Species '%s', but no such Isotopologue is defined.\n", parser.argc(2), parser.argc(1));
// 					error = true;
// 					break;
// 				}
// 
// 				// OK to add 
// 				if (!sample->addIsotopologueToMixture(sp, iso, parser.argd(3))) error = true;
// 				else Messenger::print("--> Added Isotopologue '%s' (Species '%s') to Module '%s' (%f relative population).\n", iso->name(), sp->name(), sample->name(), parser.argd(3));
// 				break;
// 


// Update IsotopologueMix List
void IsotopologueSet::updateIsotopologueMixtures(const List<Species>& species)
{
	/*
	 * This function reconstructs the current List of IsotopologueMix items and ensures that
	 * it contains all valid component Species and Isotopologue pointers.
	 */

	// Construct a temporary RefList, and move all existing RefListItems to it
	List<IsotopologueMix> oldItems;
	IsotopologueMix* mix;
	while (isotopologueMixtures_.last() != NULL)
	{
		mix = isotopologueMixtures_.last();
		isotopologueMixtures_.cut(mix);
		oldItems.own(mix);
	}
	
	// Loop over Species in System, and search for an associated IsotopologueMix in the oldItems list
	for (Species* sp = species.first(); sp != NULL; sp = sp->next)
	{
		for (mix = oldItems.first(); mix != NULL; mix = mix->next) if (mix->species() == sp) break;

		// If we found the existing item, append it to the local list.
		// Otherwise, create a new one with the default (first) Isotopologue.
		if (mix != NULL)
		{
			// Update IsotopologueMix, and check that it still contains something...
			// If there are no isotopologues left in the mix, try to add one
			mix->update();
			if (mix->nIsotopologues() == 0) mix->addNextIsotopologue();
			
			// If we get here, its still valid, so store it
			oldItems.cut(mix);
			isotopologueMixtures_.own(mix);
		}
		else
		{
			mix = isotopologueMixtures_.add();
			mix->setSpecies(sp);
		}
	}
}

// Add Isotopologue for Species
bool IsotopologueSet::addIsotopologue(Species* sp, Isotopologue *iso, double relPop)
{
	// Check that the Species is in the list...
	IsotopologueMix* mix = hasSpeciesIsotopologueMixture(sp);
	if (mix == NULL)
	{
		Messenger::print("Warning: IsotopologueSet contains no IsotopologueMix definition for Species '%s'.\n", sp->name());
		return false;
	}

	// Check current number of Isotopologues defined against total available
	if (sp->nIsotopologues() == mix->nIsotopologues())
	{
		Messenger::print("Can't add a new Isotopologue definition for Species '%s' in IsotopologueSet since there are no unused Isotopologue definitions left.\n", sp->name());
		return false;
	}

	// Was a specific Isotopologue provided?
	if (iso == NULL)
	{
		// Add next available Isotopologue to mixture
		if (!mix->addNextIsotopologue()) return false;
	}
	else if (!mix->addIsotopologue(iso, relPop))
	{
		Messenger::error("Failed to add Isotopologue to IsotopologueSet.\n");
		return false;
	}
	
	return true;
}

// Return first IsotopologueMix
IsotopologueMix *IsotopologueSet::isotopologueMixtures() const
{
	return isotopologueMixtures_.first();
}

// Return nth IsotopologueMix
IsotopologueMix *IsotopologueSet::isotopologueMixture(int n)
{
	return isotopologueMixtures_[n];
}

// Assign default (first) Isotopologues for all Species
void IsotopologueSet::assignDefaultIsotopes()
{
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next)
	{
		if (mix->nIsotopologues() != 0) continue;
		if (mix->species()->nIsotopologues() == 0) continue;
		mix->addNextIsotopologue();
	}
}

// Create type list
bool IsotopologueSet::createTypeList(const List<Species>& allSpecies, const List<AtomType>& masterIndex)
{
	// Loop over IsotopologueSets and go through Isotopologues in each mixture
	atomTypes_.clear();
	Isotope* iso;

	Messenger::print("--> Generating AtomType/Isotope index...\n");
	// Simultaneous loop over defined Species and IsotopologueMixtures (which are directly related)
	Species* refSp = allSpecies.first();
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next, refSp = refSp->next)
	{
		// Sanity check
		if (mix->species() != refSp)
		{
			Messenger::error("Species referred to in mixture in IsotopologueSet '%s' does not correspond to that in the main Species list.\n", mix->species()->name());
			return false;
		}

		// We must now loop over the Isotopologues in the mixture
		for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
		{
			// Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the Isotopologue
			for (SpeciesAtom* i = refSp->atoms(); i != NULL; i = i->next)
			{
				iso = tope->item->atomTypeIsotope(i->atomType());
				atomTypes_.add(i->atomType(), iso);
			}
		}
	}

	// Calculate fractional populations
	atomTypes_.finalise();

	// Set master type indices
	for (AtomTypeData* atd = atomTypes_.first(); atd != NULL; atd = atd->next)
	{
		// Find entry in the master index which contains the AtomType of 'at1'
		int id = masterIndex.indexOf(atd->atomType());
		if (id < 0)
		{
			Messenger::print("INTERNAL_ERROR - Couldn't find entry for first AtomType '%s' in masterIndex.\n", atd->name());
			return false;
		}
		atd->setMasterIndex(id);
	}

	return true;
}

// Print full isotopologue information
void IsotopologueSet::print()
{
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next)
	{
		for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
		{
			if (tope == mix->isotopologues()) Messenger::print("       %-15s  %-15s\n", mix->species()->name(), tope->item->name());
			else Messenger::print("                        %-15s\n", tope->item->name());
		}
	}
}

// Return AtomTypeList
AtomTypeList& IsotopologueSet::atomTypes()
{
	return atomTypes_;
}

// Return number of used AtomTypes
int IsotopologueSet::nUsedTypes()
{
	return atomTypes_.nItems();
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool IsotopologueSet::broadcast(ProcessPool& procPool, const List<Species>& species)
{
#ifdef PARALLEL
	int index, topeCount;
	double relPop;
	IsotopologueMix* iso;

	// Mixture information
	// Component/Species RefList will have already been constructed in DUQ::addSample(), so just update constituent Isotopologue
	for (iso = isotopologueMixtures_.first(); iso != NULL; iso = iso->next)
	{
		// Master needs to determine Species index
		if (procPool.isMaster()) index = species.indexOf(iso->species());
		if (!procPool.broadcast(&index, 1)) return false;
		iso->setSpecies(species.item(index));

		// Now sent number of isotopes in mixture
		topeCount = iso->nIsotopologues();
		if (!procPool.broadcast(&topeCount, 1)) return false;
		
		if (procPool.isMaster()) for (RefListItem<Isotopologue,double>* ri = iso->isotopologues(); ri != NULL; ri = ri->next)
		{
			// Get Isotopologue index from Species
			index = iso->species()->indexOfIsotopologue(ri->item);
			if (!procPool.broadcast(&index, 1)) return false;
			// Send relative population
			relPop = ri->data;
			if (!procPool.broadcast(&relPop, 1)) return false;
		}
		else
		{
			for (int m = 0; m<topeCount; ++m)
			{
				// Receive Isotopologue index in associated Species
				if (!procPool.broadcast(&index, 1)) return false;
				// Receive relative population
				if (!procPool.broadcast(&relPop, 1)) return false;
				// Add new mixture component
				iso->addIsotopologue(iso->species()->isotopologue(index), relPop);
			}
		}
	}
#endif
	return true;
}
