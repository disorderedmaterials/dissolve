/*
	*** Bond Definition
	*** src/classes/bond.cpp
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

#include "classes/bond.h"
#include "classes/atom.h"
#include "classes/species.h"
#include "base/processpool.h"

// Constructor
Bond::Bond() : Intra(), DynamicArrayObject<Bond>()
{
	clear();
}

// Destructor
Bond::~Bond()
{
}

/*
 * DynamicArrayObject Virtuals
 */

// Clear object, ready for re-use
void Bond::clear()
{
	speciesBond_ = NULL;
	i_ = NULL;
	j_ = NULL;

	// Intra
	deleteAttachedAtomArrays();
	molecule_ = NULL;
}

/*
 * Atom Information
 */

// Set Atoms involved in Bond
void Bond::setAtoms(Atom* i, Atom* j)
{
	i_ = i;
	j_ = j;
#ifdef CHECKS
	if (i_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom i in Bond::set().\n");
	if (j_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom j in Bond::set().\n");
#endif
}

// Return first Atom involved in Bond
Atom* Bond::i() const
{
	return i_;
}

// Return second Atom involved in Bond
Atom* Bond::j() const
{
	return j_;
}

// Return the 'other' Atom in the Bond
Atom* Bond::partner(Atom* i) const
{
	return (i == i_ ? j_ : i_);
}

// Return whether Atoms in Bond match those specified
bool Bond::matches(Atom* i, Atom* j) const
{
	if ((i_ == i) && (j_ == j)) return true;
	if ((i_ == j) && (j_ == i)) return true;
	return false;
}

/*
 * Energy / Force
 */

// Set SpeciesBond reference
void Bond::setSpeciesBond(SpeciesBond* bondRef)
{
	speciesBond_ = bondRef;
}

// Return parent SpeciesBond
SpeciesBond* Bond::speciesBond() const
{
	return speciesBond_;
}

// Return energy for specified distance
double Bond::energy(double distance) const
{
#ifdef CHECKS
	if (speciesBond_ == NULL)
	{
		Messenger::warn("Bond:energy() - Bond has no SpeciesBond set - returning 0.0.\n");
		return 0.0;
	}
#endif
	return speciesBond_->energy(distance);
}

// Return force multiplier for specified distance
double Bond::force(double distance) const
{
#ifdef CHECKS
	if (speciesBond_ == NULL)
	{
		Messenger::warn("Bond:force() - Bond has no SpeciesBond set - returning 0.0.\n");
		return 0.0;
	}
#endif
	return speciesBond_->force(distance);
}
