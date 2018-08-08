/*
	*** AtomTypeList Definition
	*** src/classes/atomtypelist.cpp
	Copyright T. Youngs 2012-2018

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
#include "classes/atomtype.h"
#include "classes/isotopedata.h"
#include "data/elements.h"
#include "data/isotopes.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/broadcastlist.h"
#include "templates/listio.h"
#include <string.h>

// Constructor
AtomTypeList::AtomTypeList()
{
}

// Destructor
AtomTypeList::~AtomTypeList()
{
}

// Copy Constructor
AtomTypeList::AtomTypeList(const AtomTypeList& source)
{
	(*this) = source;
}

// Assignment Operator
void AtomTypeList::operator=(const AtomTypeList& source)
{
	types_ = source.types_;
}

// Array access operator
AtomTypeData* AtomTypeList::operator[](int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= types_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Specified index %i out of range in AtomTypeList::operator[].\n", n);
		return NULL;
	}
#endif
	return types_[n];
}

/*
 * Type List
 */

// Clear all data
void AtomTypeList::clear()
{
	types_.clear();
}

// Zero populations of all types in the list
void AtomTypeList::zero()
{
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) atd->zeroPopulations();
}

// Add the specified AtomType to the list, returning the index of the AtomType in the list
AtomTypeData* AtomTypeList::add(AtomType* atomType, int population)
{
	// Search the list for the AtomType provided.
	AtomTypeData* atd = NULL;
	for (atd = types_.first(); atd != NULL; atd = atd->next) if (atd->atomType() == atomType) break;
	
	// Create new entry if one wasn't found
	if (atd == NULL)
	{
		atd = types_.add();
		atd->initialise(types_.nItems()-1, atomType, 0);
	}

	// Increase general (non-isotopic) population
	atd->add(population);

	return atd;
}

// Add the AtomTypes in the supplied list into this one, increasing populations etc.
void AtomTypeList::add(const AtomTypeList& source)
{
	// Loop over AtomTypes in the source list
	for (AtomTypeData* newType = source.first(); newType != NULL; newType = newType->next)
	{
		AtomTypeData* atd = add(newType->atomType());

		// Now add Isotope data
		for (IsotopeData* topeData = newType->isotopeData(); topeData != NULL; topeData = topeData->next) atd->add(topeData->isotope(), topeData->population());
	}
}

// Add/increase this AtomType/Isotope pair
void AtomTypeList::addIsotope(AtomType* atomType, Isotope* tope, int popAdd)
{
	AtomTypeData* atd = add(atomType, 0);
	
	// Add / increase isotope population
	if (tope != NULL) atd->add(tope, popAdd);
}

// Finalise list, calculating fractional populations etc.
void AtomTypeList::finalise()
{
	// Finalise AtomTypeData
	int total = totalPopulation();
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) atd->finalise(total);
}

// Finalise list, calculating fractional populations etc., and accounting for exchangeable sites in boundCoherent values
void AtomTypeList::finalise(const AtomTypeList& exchangeable)
{
	// Perform basic tasks
	finalise();

	// Account for exchangeable atoms - form the average bound coherent scattering over all exchangeable atoms
	double totalFraction = 0.0, boundCoherent = 0.0;
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next)
	{
		// If this type is not exchangable, move on
		if (!exchangeable.contains(atd->atomType())) continue;

		// Sum total atomic fraction and weighted bound coherent scattering length
		totalFraction += atd->fraction();
		boundCoherent += atd->fraction() * atd->boundCoherent();
	}
	boundCoherent /= totalFraction;

	// Now go back through the list and set the new scattering length for exchangeable components
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next)
	{
		// If this type is not exchangable, move on
		if (!exchangeable.contains(atd->atomType())) continue;

		// Set the bound coherent scattering length of this component to the average of all exchangable components
		atd->setBoundCoherent(boundCoherent);
		atd->setAsExchangeable();
	}
}

// Make all AtomTypeData in the list reference only their natural isotope
void AtomTypeList::naturalise()
{
	// Loop over AtomTypes in the source list
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) atd->naturalise();
}

// Check for presence of AtomType in list
bool AtomTypeList::contains(AtomType* atomType) const
{
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) if (atd->atomType() == atomType) return true;

	return false;
}

// Check for presence of AtomType/Isotope pair in list
bool AtomTypeList::contains(AtomType* atomType, Isotope* tope)
{
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next)
	{
		if (atd->atomType() != atomType) continue;
		if (!atd->hasIsotope(tope)) continue;
		return true;
	}

	return false;
}

// Return number of AtomType/Isotopes in list
int AtomTypeList::nItems() const
{
	return types_.nItems();
}

// Return first item in list
AtomTypeData* AtomTypeList::first() const
{
	return types_.first();
}

// Return types list
const List<AtomTypeData>& AtomTypeList::types() const
{
	return types_;
}

// Return index of AtomType in list
int AtomTypeList::indexOf(AtomType* atomtype) const
{
	int count = 0;
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next)
	{
		if (atd->atomType() == atomtype) return count;
		++count;
	}

	return -1;
}

// Return index of names AtomType in list
int AtomTypeList::indexOf(const char* name) const
{
	int count = 0;
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next)
	{
		if (DissolveSys::sameString(atd->atomType()->name(), name)) return count;
		++count;
	}

	return -1;
}

// Return total population of all types in list
int AtomTypeList::totalPopulation() const
{
	int total = 0;
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) total += atd->population();
	return total;
}

// Return nth referenced AtomType
AtomType* AtomTypeList::atomType(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= types_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Specified index %i out of range in AtomTypeList::atomType().\n");
		return NULL;
	}
#endif
	return types_[n]->atomType();
}

// Return AtomTypeData for specified AtomType
AtomTypeData* AtomTypeList::atomTypeData(AtomType* atomType)
{
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) if (atomType == atd->atomType()) return atd;

	return NULL;
}

// Print AtomType populations
void AtomTypeList::print() const
{
	Messenger::print("  AtomType  El  Isotope  Population  Fraction           bc (fm)\n");
	Messenger::print("  -------------------------------------------------------------\n");
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next)
	{
		char exch = atd->exchangeable() ? 'E' : ' ';

		// If there are isotopes defined, print them
		if (atd->isotopeData())
		{
			Messenger::print("%c %-8s  %-3s    -     %-10i  %8.6f (of world) %6.3f\n", exch, atd->atomTypeName(), atd->atomType()->element()->symbol(), atd->population(), atd->fraction(), atd->boundCoherent());

			for (IsotopeData* topeData = atd->isotopeData(); topeData != NULL; topeData = topeData->next)
			{
				Messenger::print("                   %-3i   %-10i  %8.6f (of type)  %6.3f\n", topeData->isotope()->A(), topeData->population(), topeData->fraction(), topeData->isotope()->boundCoherent());
			}

		}
		else Messenger::print("%c %-8s  %-3s          %-10i  %8.6f     --- N/A ---\n", exch, atd->atomTypeName(), atd->atomType()->element()->symbol(), atd->population(), atd->fraction());

		Messenger::print("  -------------------------------------------------------------\n");	
	}
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* AtomTypeList::itemClassName()
{
	return "AtomTypeList";
}

// Write data through specified LineParser
bool AtomTypeList::write(LineParser& parser)
{
	return ListIO<AtomTypeData>::write(types_, parser);
}

// Read data through specified LineParser
bool AtomTypeList::read(LineParser& parser)
{
	return ListIO<AtomTypeData>::read(types_, parser);
}

/*
 * Parallel Comms
 */

// Broadcast item contents
bool AtomTypeList::broadcast(ProcessPool& procPool, int root)
{
#ifdef PARALLEL
	// Broadcast AtomTypeData list
	BroadcastList<AtomTypeData> atdBroadcaster(procPool, root, types_);
	if (atdBroadcaster.failed()) return false;
#endif
	return true;
}

// Check item equality
bool AtomTypeList::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	// Check number of types in list first
	if (!procPool.equality(types_.nItems())) return Messenger::error("AtomTypeList size is not equivalent (process %i has %i).\n", procPool.poolRank(), types_.nItems());
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) if (!atd->equality(procPool)) return Messenger::error("AtomTypeList entry for type '%s' is not equivalent.\n", atd->atomTypeName());
#endif
	return true;
}
