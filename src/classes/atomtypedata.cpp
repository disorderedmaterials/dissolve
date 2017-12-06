/*
	*** AtomTypeData Definition
	*** src/classes/atomtypedata.cpp
	Copyright T. Youngs 2012-2017

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
#include "base/constants.h"
#include "base/isotope.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/ptable.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/broadcastlist.h"
#include <string.h>

// Constructor
AtomTypeData::AtomTypeData() : MPIListItem<AtomTypeData>()
{
	atomType_ = NULL;
	exchangeable_ = false;
	population_ = 0;
	fraction_ = 0.0;
	boundCoherent_ = 0.0;
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
	exchangeable_ = source.exchangeable_;
	isotopes_ = source.isotopes_;
	population_ = source.population_;
	fraction_ = source.fraction_;
	boundCoherent_ = source.boundCoherent_;
}

/*
 * Properties
 */

// Initialise
bool AtomTypeData::initialise(int listIndex, AtomType* atomType, int population)
{
	listIndex_ = listIndex;
	atomType_ = atomType;
	if (atomType == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer passed to AtomTypeData::initialise().\n");
		return false;
	}

	population_ = population;
	fraction_ = 0.0;
	boundCoherent_ = 0.0;

	// 	Messenger::print("--> Initialised AtomType index entry with AtomType '%s', Isotope %i (bc = %7.3f)\n", atomType->name(), tope->A(), tope->boundCoherent());
	return true;
}

// Add to population
void AtomTypeData::add(int nAdd)
{
	population_ += nAdd;
}

// Add to population of Isotope
void AtomTypeData::add(Isotope* tope, int nAdd)
{
	// Has this isotope already been added to the list?
	IsotopeData* topeData = NULL;
	for (topeData = isotopes_.first(); topeData != NULL; topeData = topeData->next) if (topeData->isotope() == tope) break;
	if (topeData == NULL)
	{
		topeData = isotopes_.add();
		topeData->initialise(tope);
	}

	// Increase Isotope population
	topeData->add(nAdd);

	// Increase total integer population
	population_ += nAdd;
}

// Zero populations
void AtomTypeData::zeroPopulations()
{
	// Zero individual isotope counts
	for (IsotopeData* topeData = isotopes_.first(); topeData != NULL; topeData = topeData->next) topeData->zeroPopulation();

	// Zero totals
	population_ = 0;
	fraction_ = 0.0;
}

// Return list index of AtomTypeData in AtomTypeList
int AtomTypeData::listIndex() const
{
	return listIndex_;
}

// Return reference AtomType
AtomType* AtomTypeData::atomType() const
{
	return atomType_;
}

// Set whether this atom is exchangeable
void AtomTypeData::setExchangeable(bool b)
{
	exchangeable_ = b;
}

// Return whether this atom is exchangeable
bool AtomTypeData::exchangeable() const
{
	return exchangeable_;
}

// Finalise, calculating fractional populations etc.
void AtomTypeData::finalise(int totalAtoms)
{
	// Calculate fractional world population
	fraction_ = double(population_) / totalAtoms;

	// Calculate isotope fractional populations (of AtomType)
	for (IsotopeData* topeData = isotopes_.first(); topeData != NULL; topeData = topeData->next) topeData->finalise(population_);

	// Determine bound coherent scattering for AtomType, based on Isotope populations
	boundCoherent_ = 0.0;
	for (IsotopeData* topeData = isotopes_.first(); topeData != NULL; topeData = topeData->next) boundCoherent_ += topeData->fraction()*topeData->isotope()->boundCoherent();
}

// Return if specified Isotope is already in the list
bool AtomTypeData::hasIsotope(Isotope* tope)
{
	for (IsotopeData* topeData = isotopes_.first(); topeData != NULL; topeData = topeData->next) if (topeData->isotope() == tope) return true;

	return false;
}

// Return Isotope
IsotopeData* AtomTypeData::isotopeData()
{
	return isotopes_.first();
}

// Return total population over all isotopes
int AtomTypeData::population() const
{
	return population_;
}

// Return total fractional population including all isotopes
double AtomTypeData::fraction() const
{
	return fraction_;
}

// Set calculated bond coherent scattering over all isotopes
void AtomTypeData::setBoundCoherent(double d)
{
	boundCoherent_ = d;
}

// Calculated bound coherent scattering over all Isotopes
double AtomTypeData::boundCoherent() const
{
	return boundCoherent_;
}

// Return referenced AtomType name
const char* AtomTypeData::atomTypeName() const
{
	return atomType_->name();
}

/*
 * I/O
 */

// Write data through specified LineParser
bool AtomTypeData::write(LineParser& parser)
{
	// Line Contains: AtomType name, exchangeable flag, population, fraction, boundCoherent, and nIsotopes
	if (!parser.writeLineF("%s %s %i %f %f %i\n", atomType_->name(), DUQSys::btoa(exchangeable_), population_, fraction_, boundCoherent_, isotopes_.nItems())) return false;
	ListIterator<IsotopeData> isotopeIterator(isotopes_);
	while (IsotopeData* topeData = isotopeIterator.iterate()) if (!topeData->write(parser)) return false;
	return true;
}

// Read data through specified LineParser
bool AtomTypeData::read(LineParser& parser)
{
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	for (atomType_ = List<AtomType>::masterInstance()->first(); atomType_ != NULL; atomType_ = atomType_->next) if (DUQSys::sameString(atomType_->name(), parser.argc(0))) break;
	if (!atomType_) return false;
	exchangeable_ = parser.argb(1);
	population_ = parser.argi(2);
	fraction_ = parser.argd(3);
	boundCoherent_ = parser.argd(4);
	isotopes_.clear();
	int nIso = parser.argi(5);
	for (int n = 0; n<nIso; ++n)
	{
		IsotopeData* tope = isotopes_.add();
		if (!tope->read(parser)) return false;
	}
	return true;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool AtomTypeData::broadcast(ProcessPool& procPool, int root)
{
#ifdef PARALLEL
	// Must have a master instance to proceed
	if (!List<AtomType>::hasMasterInstance())
	{
		Messenger::error("Master List<AtomType> instance has not been set, so AtomTypeData::broadcast() is not possible.\n");
		return false;
	}

	// For atomType_, use the master instance of List<AtomType> to find the index (*not* the local listIndex_) and broadcast it
	int typeIndex;
	if (procPool.poolRank() == root) typeIndex = List<AtomType>::masterInstance()->indexOf(atomType_);
	procPool.broadcast(typeIndex, root);
	atomType_ = List<AtomType>::masterInstance()->item(typeIndex);

	// Broadcast the IsotopeData list
	BroadcastList<IsotopeData> topeBroadcaster(procPool, root, isotopes_);
	if (topeBroadcaster.failed()) return false;

	procPool.broadcast(population_, root);
	procPool.broadcast(fraction_, root);
	procPool.broadcast(boundCoherent_, root);
#endif
	return true;
}
