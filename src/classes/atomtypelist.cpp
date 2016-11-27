/*
	*** AtomTypeList Definition
	*** src/classes/atomtypelist.cpp
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

#include "classes/atomtypelist.h"
#include "classes/atomtype.h"
#include "base/isotope.h"
#include "base/ptable.h"
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

// Add/increase this AtomType/Isotope pair
int AtomTypeList::add(AtomType* atomType, Isotope* tope, int popAdd)
{
	// Search the list for the AtomType provided.
	// If present, we will check the isotope already stored
	AtomTypeData* atd = NULL;
	int index = 0;
	for (atd = types_.first(); atd != NULL; atd = atd->next, ++index)
	{
		if (atd->atomType() != atomType) continue;
		if (atd->isotope() != tope) continue;
		break;
	}
	
	// Create new entry if one wasn't found
	if (atd == NULL)
	{
		atd = types_.add();
		atd->initialise(atomType, tope);
	}
	
	// Increase population
	atd->add(popAdd);

	return index;
}

// Zero populations of all types in the list
void AtomTypeList::zero()
{
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next)
	{
		atd->zero();
	}
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
void AtomTypeList::print()
{
	int count = 0;
	Dnchar indexData;
	Messenger::print("--> Populations : AtomType    El   Population  AtomFrac  Isotope  bc (fm)\n");
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next)
	{
		if (atd->masterIndex() == -1) indexData = "M";
		else indexData.sprintf("index=%i", atd->masterIndex());
		
		if (atd->isotope()) Messenger::print("              %2i  %-10s  %-3s  %-10i  %8.6f    %-3i   %8.3f  (%s)\n", count++, atd->name(), PeriodicTable::element(atd->atomType()->element()).symbol(), atd->population(), atd->fraction(), atd->isotope()->A(), atd->isotope()->boundCoherent(), indexData.get());
		else Messenger::print("              %2i  %-10s  %-3s  %-10i  %8.6f     --- N/A ---    (%s)\n", count++, atd->name(), PeriodicTable::element(atd->atomType()->element()).symbol(), atd->population(), atd->fraction(), indexData.get());
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
	int total = totalPopulation();
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) atd->finalise(total);
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
