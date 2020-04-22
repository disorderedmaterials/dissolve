/*
	*** AtomTypeData Definition
	*** src/classes/atomtypedata.cpp
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

#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/atomtypelist.h"
#include "classes/coredata.h"
#include "classes/isotopedata.h"
#include "data/isotopes.h"
#include "templates/broadcastlist.h"
#include <string.h>

// Constructor
AtomTypeData::AtomTypeData() : ListItem<AtomTypeData>()
{
	atomType_ = NULL;
	exchangeable_ = false;
	population_ = 0;
	fraction_ = 0.0;
	boundCoherent_ = 0.0;
}

// Copy Constructor
AtomTypeData::AtomTypeData(const AtomTypeData &source) { (*this) = source; }

// Assignment Operator
void AtomTypeData::operator=(const AtomTypeData &source)
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
bool AtomTypeData::initialise(int listIndex, AtomType *atomType, double population)
{
	listIndex_ = listIndex;
	atomType_ = atomType;
	if (atomType == NULL)
		return Messenger::error("NULL_POINTER - NULL AtomType pointer passed to AtomTypeData::initialise().\n");

	population_ = population;
	fraction_ = 0.0;
	boundCoherent_ = 0.0;

	// 	Messenger::print("Initialised AtomType index entry with AtomType '%s', Isotope %i (bc = %7.3f)\n", atomType->name(), tope->A(), tope->boundCoherent());
	return true;
}

// Add to population
void AtomTypeData::add(double nAdd) { population_ += nAdd; }

// Add to population of Isotope
void AtomTypeData::add(std::shared_ptr<Isotope> tope, double nAdd)
{
	// Has this isotope already been added to the list?
	IsotopeData *topeData = NULL;
	for (topeData = isotopes_.first(); topeData != NULL; topeData = topeData->next())
		if (topeData->isotope() == tope)
			break;
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
	for (IsotopeData *topeData = isotopes_.first(); topeData != NULL; topeData = topeData->next())
		topeData->zeroPopulation();

	// Zero totals
	population_ = 0.0;
	fraction_ = 0.0;
}

// Return list index of AtomTypeData in AtomTypeList
int AtomTypeData::listIndex() const { return listIndex_; }

// Return reference AtomType
AtomType *AtomTypeData::atomType() const { return atomType_; }

// Set exchangeable flag
void AtomTypeData::setAsExchangeable() { exchangeable_ = true; }

// Return whether the associated AtomType is exchangeable
bool AtomTypeData::exchangeable() const { return exchangeable_; }

// Finalise, calculating fractional populations etc.
void AtomTypeData::finalise(double totalAtoms)
{
	// Calculate fractional world population
	fraction_ = population_ / totalAtoms;

	// Calculate isotope fractional populations (of AtomType)
	for (IsotopeData *topeData = isotopes_.first(); topeData != NULL; topeData = topeData->next())
		topeData->finalise(population_);

	// Determine bound coherent scattering for AtomType, based on Isotope populations
	boundCoherent_ = 0.0;
	for (IsotopeData *topeData = isotopes_.first(); topeData != NULL; topeData = topeData->next())
		boundCoherent_ += topeData->fraction() * topeData->isotope()->boundCoherent();
}

// Remove any existing isotopes, and add only the natural isotope
void AtomTypeData::naturalise()
{
	// Clear the isotopes list and add on the natural isotope, keeping the current population
	isotopes_.clear();
	IsotopeData *topeData = isotopes_.add();
	topeData->initialise(Isotopes::naturalIsotope(atomType_->element()));
	topeData->add(population_);
	topeData->finalise(population_);
	boundCoherent_ = topeData->isotope()->boundCoherent();
}

// Return if specified Isotope is already in the list
bool AtomTypeData::hasIsotope(std::shared_ptr<Isotope> tope)
{
	for (IsotopeData *topeData = isotopes_.first(); topeData != NULL; topeData = topeData->next())
		if (topeData->isotope() == tope)
			return true;

	return false;
}

// Set this AtomType to have only the single Isotope provided
void AtomTypeData::setSingleIsotope(std::shared_ptr<Isotope> tope)
{
	isotopes_.clear();
	IsotopeData *topeData = isotopes_.add();
	topeData->initialise(tope);
	topeData->add(population_);
	topeData->finalise(population_);
	boundCoherent_ = topeData->isotope()->boundCoherent();
}

// Return Isotope
IsotopeData *AtomTypeData::isotopeData() { return isotopes_.first(); }

// Return total population over all isotopes
int AtomTypeData::population() const { return population_; }

// Return total fractional population including all isotopes
double AtomTypeData::fraction() const { return fraction_; }

// Set calculated bond coherent scattering over all isotopes
void AtomTypeData::setBoundCoherent(double d) { boundCoherent_ = d; }

// Calculated bound coherent scattering over all Isotopes
double AtomTypeData::boundCoherent() const { return boundCoherent_; }

// Return referenced AtomType name
const char *AtomTypeData::atomTypeName() const { return atomType_->name(); }

/*
 * I/O
 */

// Read data through specified LineParser
bool AtomTypeData::read(LineParser &parser, const CoreData &coreData)
{
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
		return false;
	atomType_ = coreData.findAtomType(parser.argc(0));
	if (!atomType_)
		return false;
	population_ = parser.argd(1);
	fraction_ = parser.argd(2);
	boundCoherent_ = parser.argd(3);
	isotopes_.clear();
	int nIso = parser.argi(4);
	for (int n = 0; n < nIso; ++n)
	{
		IsotopeData *tope = isotopes_.add();
		if (!tope->read(parser, coreData))
			return false;
	}
	return true;
}

// Write data through specified LineParser
bool AtomTypeData::write(LineParser &parser)
{
	// Line Contains: AtomType name, exchangeable flag, population, fraction, boundCoherent, and nIsotopes
	if (!parser.writeLineF("%s %f %f %f %i\n", atomType_->name(), population_, fraction_, boundCoherent_, isotopes_.nItems()))
		return false;
	ListIterator<IsotopeData> isotopeIterator(isotopes_);
	while (IsotopeData *topeData = isotopeIterator.iterate())
		if (!topeData->write(parser))
			return false;
	return true;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool AtomTypeData::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
	// For the atomType_, use the fact that the AtomType names are unique...
	CharString typeName;
	if (procPool.poolRank() == root)
		typeName = atomType_->name();
	procPool.broadcast(typeName, root);
	atomType_ = coreData.findAtomType(typeName);

	// Broadcast the IsotopeData list
	BroadcastList<IsotopeData> topeBroadcaster(procPool, root, isotopes_, coreData);
	if (topeBroadcaster.failed())
		return false;

	procPool.broadcast(population_, root);
	procPool.broadcast(fraction_, root);
	procPool.broadcast(boundCoherent_, root);
#endif
	return true;
}

// Check item equality
bool AtomTypeData::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
	if (!procPool.equality(atomTypeName()))
		return Messenger::error("AtomTypeData atom type name is not equivalent (process %i has '%s').\n", procPool.poolRank(), atomTypeName());
	if (!procPool.equality(population_))
		return Messenger::error("AtomTypeData population is not equivalent (process %i has %i).\n", procPool.poolRank(), population_);
	if (!procPool.equality(fraction_))
		return Messenger::error("AtomTypeData fraction is not equivalent (process %i has %e).\n", procPool.poolRank(), fraction_);
	if (!procPool.equality(boundCoherent_))
		return Messenger::error("AtomTypeData bound coherent is not equivalent (process %i has %e).\n", procPool.poolRank(), boundCoherent_);

	// Number of isotopes
	if (!procPool.equality(isotopes_.nItems()))
		return Messenger::error("AtomTypeData number of isotopes is not equivalent (process %i has %i).\n", procPool.poolRank(), isotopes_.nItems());
	ListIterator<IsotopeData> isotopeIterator(isotopes_);
	int count = 0;
	while (IsotopeData *topeData = isotopeIterator.iterate())
	{
		if (!topeData->equality(procPool))
			return Messenger::error("AtomTypeData entry for isotope data %i is not equivalent.\n", count);
		++count;
	}
#endif
	return true;
}
