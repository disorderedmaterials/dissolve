/*
	*** AtomTypeList Definition
	*** src/classes/atomtypelist.cpp
	Copyright T. Youngs 2012-2018

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

#include "classes/atomtypelist.h"
#include "classes/atomtype.h"
#include "classes/isotopedata.h"
#include "base/lineparser.h"
#include "base/ptable.h"
#include "base/processpool.h"
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

/*
 * List
 */

// Clear all data
void AtomTypeList::clear()
{
	types_.clear();
}

// Add the specified AtomType to the list, returning the index of the AtomType in the list
AtomTypeData* AtomTypeList::add(AtomType* atomType, int population, bool exchangeable)
{
	// Search the list for the AtomType provided.
	AtomTypeData* atd = NULL;
	for (atd = types_.first(); atd != NULL; atd = atd->next) if (atd->atomType() == atomType) break;
	
	// Create new entry if one wasn't found
	if (atd == NULL)
	{
		atd = types_.add();
		atd->initialise(types_.nItems()-1, atomType, 0);
		atd->setExchangeable(exchangeable);
	}

	// Increase general (non-isotopic) population
	atd->add(population);

	return atd;
}

// Add/increase this AtomType/Isotope pair
void AtomTypeList::addIsotope(AtomType* atomType, Isotope* tope, int popAdd, bool exchangeable)
{
	AtomTypeData* atd = add(atomType, 0, exchangeable);
	
	// Add / increase isotope population
	if (tope != NULL) atd->add(tope, popAdd);
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

// Check for presence of AtomType in list
bool AtomTypeList::contains(AtomType* atomType)
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

// Zero populations of all types in the list
void AtomTypeList::zero()
{
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) atd->zeroPopulations();
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
			Messenger::print("%c %-8s  %-3s    -     %-10i  %8.6f (of world) %6.3f\n", exch, atd->atomTypeName(), PeriodicTable::element(atd->atomType()->element()).symbol(), atd->population(), atd->fraction(), atd->boundCoherent());

			for (IsotopeData* topeData = atd->isotopeData(); topeData != NULL; topeData = topeData->next)
			{
				Messenger::print("                   %-3i   %-10i  %8.6f (of type)  %6.3f\n", topeData->isotope()->A(), topeData->population(), topeData->fraction(), topeData->isotope()->boundCoherent());
			}

		}
		else Messenger::print("%c %-8s  %-3s          %-10i  %8.6f     --- N/A ---\n", exch, atd->atomTypeName(), PeriodicTable::element(atd->atomType()->element()).symbol(), atd->population(), atd->fraction());

		Messenger::print("  -------------------------------------------------------------\n");	
	}
}

/*
 * Access
 */

// Return index of AtomType/Isotope in list
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

// Return total population of all types in list
int AtomTypeList::totalPopulation() const
{
	int total = 0;
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) total += atd->population();
	return total;
}

// Finalise list, calculating fractional populations etc.
void AtomTypeList::finalise()
{
	// Finalise AtomTypeData
	int total = totalPopulation();
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) atd->finalise(total);

	// Account for exchangeable atoms - form the average bound coherent scattering over all exchangeable atoms
	double totalFraction = 0.0, boundCoherent = 0.0;
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next)
	{
		if (!atd->exchangeable()) continue;
		totalFraction += atd->fraction();
		boundCoherent += atd->fraction() * atd->boundCoherent();
	}
	boundCoherent /= totalFraction;

	// Now go back through the list and set the new scattering length for exchangeable components
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) if (atd->exchangeable()) atd->setBoundCoherent(boundCoherent);
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

// Array access operator
AtomTypeData* AtomTypeList::operator[](int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= types_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Specified index %i out of range in AtomTypeList::atomType().\n");
		return NULL;
	}
#endif
	return types_[n];
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
