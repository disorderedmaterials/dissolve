/*
	*** SpeciesSite Definition
	*** src/classes/speciessite.cpp
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

#include "classes/speciessite.h"
#include "classes/species.h"
#include "base/processpool.h"

// Constructor
SpeciesSite::SpeciesSite() : ListItem<SpeciesSite>()
{
	parent_ = NULL;
}

// Destructor
SpeciesSite::~SpeciesSite()
{
}

// Set name of site
void SpeciesSite::setName(const char* newName)
{
	name_ = newName;
}

// Return anme of site
const char* SpeciesSite::name() const
{
	return name_.get();
}

// Set Species parent
void SpeciesSite::setParent(Species* sp)
{
	parent_ = sp;
}

// Return species parent
Species* SpeciesSite::parent()
{
	return parent_;
}

// Add origin atom
bool SpeciesSite::addOriginAtom(SpeciesAtom* originAtom)
{
	if (!originAtom) return Messenger::error("NULL SpeciesAtom passed to SpeciesSite::addOriginAtom().\n");

	// If the SpeciesAtom already exists in the list, complain
	if (originAtoms_.contains(originAtom)) return Messenger::error("Origin atom index %i specified twice for site '%s'.\n", originAtom->index(), name_.get());

	originAtoms_.add(originAtom);

	return true;
}

// Add origin atom from index
bool SpeciesSite::addOriginAtom(int atomIndex)
{
#ifdef CHECKS
	if (!parent_) return Messenger::error("Tried to add an origin atom by index to a SpeciesSite, but no parent Species is set.\n");
#endif
	return addOriginAtom(parent_->atom(atomIndex));
}

// Return list of origin atoms
const RefList<SpeciesAtom,int>& SpeciesSite::originAtoms()
{
	return originAtoms_;
}

// Add x-axis atom
bool SpeciesSite::addXAxisAtom(SpeciesAtom* xAxisAtom)
{
	if (!xAxisAtom) return Messenger::error("NULL SpeciesAtom passed to SpeciesSite::addXAxisAtom().\n");

	// If the SpeciesAtom already exists in the list, complain
	if (xAxisAtoms_.contains(xAxisAtom)) return Messenger::error("X-axis atom index %i specified twice for site '%s'.\n", xAxisAtom->index(), name_.get());

	xAxisAtoms_.add(xAxisAtom);

	return true;
}

// Add x-axis atom from index
bool SpeciesSite::addXAxisAtom(int atomIndex)
{
#ifdef CHECKS
	if (!parent_) return Messenger::error("Tried to add an x-axis atom by index to a SpeciesSite, but no parent Species is set.\n");
#endif
	return addXAxisAtom(parent_->atom(atomIndex));
}

// Return list of x-axis atoms
const RefList<SpeciesAtom,int>& SpeciesSite::xAxisAtoms()
{
	return xAxisAtoms_;
}

// Add y-axis atom
bool SpeciesSite::addYAxisAtom(SpeciesAtom* yAxisAtom)
{
	if (!yAxisAtom) return Messenger::error("NULL SpeciesAtom passed to SpeciesSite::addYAxisAtom().\n");

	// If the SpeciesAtom already exists in the list, complain
	if (yAxisAtoms_.contains(yAxisAtom)) return Messenger::error("Y-axis atom index %i specified twice for site '%s'.\n", yAxisAtom->index(), name_.get());

	yAxisAtoms_.add(yAxisAtom);

	return true;
}

// Add y-axis atom from index
bool SpeciesSite::addYAxisAtom(int atomIndex)
{
#ifdef CHECKS
	if (!parent_) return Messenger::error("Tried to add a y-axis atom by index to a SpeciesSite, but no parent Species is set.\n");
#endif
	return addYAxisAtom(parent_->atom(atomIndex));
}

// Return list of y-axis atoms
const RefList<SpeciesAtom,int>& SpeciesSite::yAxisAtoms()
{
	return yAxisAtoms_;
}
