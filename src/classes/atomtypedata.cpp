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

// Constructor
AtomTypeData::AtomTypeData() : ListItem<AtomTypeData>()
{
	atomType_ = NULL;
	masterIndex_ = -1;
}

// Copy Constructor
AtomTypeData::AtomTypeData(const AtomTypeData& source)
{
	(*this) = source;
}

// Assignment Operator
void AtomTypeData::operator=(const AtomTypeData& source)
{
	atomType_ = source.atomType_;
	isotope_ = source.isotope_;
	population_ = source.population_;
	fraction_ = source.fraction_;
	masterIndex_ = source.masterIndex_;
}

/*
 * Properties
 */

// Initialise
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

// Add to population of Isotope
void AtomTypeData::add(int nAdd)
{
	population_ += nAdd;
}

// Return reference AtomType
AtomType* AtomTypeData::atomType() const
{
	return atomType_;
}

/*
 * \brief // Finalise, calculating fractional populations etc.
 */
void AtomTypeData::finalise(int totalAtoms)
{
	fraction_ = double(population_) / totalAtoms;
}

// Return Isotope
Isotope* AtomTypeData::isotope() const
{
	return isotope_;
}

// Return population
int AtomTypeData::population() const
{
	return population_;
}

// Return fractional population
double AtomTypeData::fraction() const
{
	return fraction_;
}

// Return referenced AtomType name
const char* AtomTypeData::name()
{
	return atomType_->name();
}

// Set index of non-isotopic master type
void AtomTypeData::setMasterIndex(int id)
{
	masterIndex_ = id;
}

// Return index of non-isotopic master type
int AtomTypeData::masterIndex()
{
	return masterIndex_;
}
