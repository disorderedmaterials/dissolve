/*
	*** AtomTypeIndexIndex Definition
	*** src/classes/atomtypeindex.cpp
	Copyright T. Youngs 2012-2014

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

#include "classes/atomtypeindex.h"
#include "classes/atomtype.h"
#include "base/constants.h"
#include "base/isotope.h"
#include "base/messenger.h"
#include <base/ptable.h>
#include <string.h>

/*
// AtomTypeData
*/

/*!
 * \brief Constructor
 * \details Constructor for AtomTypeData. 
 */
AtomTypeData::AtomTypeData() : ListItem<AtomTypeData>()
{
	atomType_ = NULL;
	masterIndex_ = -1;
}

/*
// Properties
*/

/*!
 * \brief Initialise
 * \details Set the AtomType pointer and first Isotope. The population of the Isotope is set to zero.
 */
bool AtomTypeData::initialise(AtomType* atomType, Isotope* tope)
{
	atomType_ = atomType;
	isotope_ = tope;
	if (atomType == NULL)
	{
		msg.error("NULL_POINTER - NULL AtomType pointer passed to AtomTypeData::initialise().\n");
		return false;
	}

	population_ = 0;
	fraction_ = 0.0;
// 	msg.print("--> Initialised AtomType index entry with AtomType '%s', Isotope %i (bc = %7.3f)\n", atomType->name(), tope->A(), tope->boundCoherent());
	return true;
}

/*!
 * \brief Add to population of Isotope
 */
void AtomTypeData::add(int nAdd)
{
	population_ += nAdd;
}

/*!
 * \brief Return reference AtomType
 */
AtomType* AtomTypeData::atomType() const
{
	return atomType_;
}

/*!
 * \brief // Finalise, calculating fractional populations etc.
 */
void AtomTypeData::finalise(int totalAtoms)
{
	fraction_ = double(population_) / totalAtoms;
}

/*!
 * \brief Return Isotope
 */
Isotope* AtomTypeData::isotope() const
{
	return isotope_;
}

/*!
 * \brief Return population
 */
int AtomTypeData::population() const
{
	return population_;
}

/*!
 * \brief Return fractional population
 */
double AtomTypeData::fraction() const
{
	return fraction_;
}

/*!
 * \brief Return referenced AtomType name
 */
const char* AtomTypeData::name()
{
	return atomType_->name();
}

/*!
 * \brief Set index of non-isotopic master type
 */
void AtomTypeData::setMasterIndex(int id)
{
	masterIndex_ = id;
}

/*!
 * \brief Return index of non-isotopic master type
 */
int AtomTypeData::masterIndex()
{
	return masterIndex_;
}

/*
// AtomTypeIndex
*/

/*!
 * \brief Constructor
 * \details Constructor for AtomTypeIndex. 
 */
AtomTypeIndex::AtomTypeIndex()
{
}

/*!
 * \brief Destructor
 * \details Constructor for AtomTypeIndex. 
 */
AtomTypeIndex::~AtomTypeIndex()
{
}

/*
// List
*/

/*!
 * \brief Clear all data
 */
void AtomTypeIndex::clear()
{
	types_.clear();
}

/*!
 * \brief Add/increase this AtomType/Isotope pair
 */
int AtomTypeIndex::add(AtomType* atomType, Isotope* tope, int popAdd)
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

/*!
 * \brief Return number of AtomType/Isotopes in list
 */
int AtomTypeIndex::nItems() const
{
	return types_.nItems();
}

/*!
 * \brief Return first item in list
 */
AtomTypeData* AtomTypeIndex::first() const
{
	return types_.first();
}

/*!
 * \brief Print AtomType populations
 */
void AtomTypeIndex::print()
{
	int count = 0;
	msg.print("--> Populations : AtomType    El   Population  AtomFrac  Isotope  bc (fm)\n");
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next)
	{
		if (atd->masterIndex() == -1) msg.print("              %2i  %-10s  %-3s  %-10i  %8.6f    %-3i   %8.3f  (M)\n", count++, atd->name(), PeriodicTable::element(atd->atomType()->element()).symbol(), atd->population(), atd->fraction(), atd->isotope()->A(), atd->isotope()->boundCoherent());
		else msg.print("              %2i  %-10s  %-3s  %-10i  %8.6f    %-3i   %8.3f  (index=%i)\n", count++, atd->name(), PeriodicTable::element(atd->atomType()->element()).symbol(), atd->population(), atd->fraction(), atd->isotope()->A(), atd->isotope()->boundCoherent(), atd->masterIndex());
	}
}

/*
// Access
*/

/*!
 * \brief Return index of AtomType/Isotope in list
 */
int AtomTypeIndex::indexOf(AtomType* atomtype) const
{
	int count = 0;
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next)
	{
		if (atd->atomType() == atomtype) return count;
		++count;
	}
	return -1;
}

/*!
 * \brief Return total population of all types in list
 */
int AtomTypeIndex::totalPopulation() const
{
	int total = 0;
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) total += atd->population();
	return total;
}

/*!
 * \brief Finalise list, calculating fractional populations etc.
 */
void AtomTypeIndex::finalise()
{
	int total = totalPopulation();
	for (AtomTypeData* atd = types_.first(); atd != NULL; atd = atd->next) atd->finalise(total);
}

/*!
 * \brief Return nth referenced AtomType
 */
AtomType* AtomTypeIndex::atomType(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= types_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Specified index %i out of range in AtomTypeIndex::atomType().\n");
		return NULL;
	}
#endif
	return types_[n]->atomType();
}

/*!
 * \brief Array access operator
 */
AtomTypeData* AtomTypeIndex::operator[](int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= types_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Specified index %i out of range in AtomTypeIndex::atomType().\n");
		return NULL;
	}
#endif
	return types_[n];
}
