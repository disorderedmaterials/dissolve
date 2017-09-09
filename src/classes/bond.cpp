/*
	*** Bond Definition
	*** src/classes/bond.cpp
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

#include "classes/bond.h"
#include "classes/atom.h"
#include "classes/species.h"
#include "base/processpool.h"

// Constructor
Bond::Bond() : DynamicArrayObject<Bond>()
{
	speciesBond_ = NULL;
	molecule_ = NULL;
	i_ = NULL;
	j_ = NULL;
	nAttached_[0] = 0;
	nAttached_[1] = 0;
	attached_[0] = NULL;
	attached_[1] = NULL;
	interGrain_ = false;
}

// Destructor
Bond::~Bond()
{
	clear();
}

// Clear all data
void Bond::clear()
{
	for (int n=0; n<2; ++n)
	{
		if (attached_[n] != NULL) delete[] attached_[n];
		attached_[n] = NULL;
		nAttached_[n] = 0;
	}
}

/*
 * Basic Data
 */

// Set Molecule in which this Bond exists
void Bond::setMolecule(Molecule* parent)
{
	molecule_ = parent;
}

// Return Molecule in which this Bond exists
Molecule* Bond::molecule() const
{
	return molecule_;
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

// Return whether Atoms in Angle match those specified
bool Bond::matches(Atom* i, Atom* j) const
{
	if ((i_ == i) && (j_ == j)) return true;
	if ((i_ == j) && (j_ == i)) return true;
	return false;
}

/*
 * Connections
 */

// Create attached Atom array
void Bond::createAttachedAtomArray(int terminus, int size)
{
	if (attached_[terminus] != NULL) delete[] attached_[terminus];
	attached_[terminus] = NULL;
	nAttached_[terminus] = size;

	if (nAttached_[terminus] != 0)
	{
		attached_[terminus] = new Atom*[nAttached_[terminus]];
		for (int n=0; n<nAttached_[terminus]; ++n) attached_[terminus][n] = NULL;
	}
}

// Set attached Atoms for terminus specified
void Bond::setAttachedAtoms(int terminus, const RefList<Atom,int>& atoms)
{
	createAttachedAtomArray(terminus, atoms.nItems());
	int index = 0;
	for (RefListItem<Atom,int>* refAtom = atoms.first(); refAtom != NULL; refAtom = refAtom->next) attached_[terminus][index++] = refAtom->item;
}

// Return number of attached Atoms for terminus specified
int Bond::nAttached(int terminus) const
{
	return nAttached_[terminus];
}

// Return array of attached Atoms for terminus specified
Atom** Bond::attached(int terminus) const
{
	return attached_[terminus];
}

// Set whether this Bond is interGrain
void Bond::setInterGrain(bool b)
{
	interGrain_ = b;
}

// Return whether this Bond is interGrain
bool Bond::interGrain() const
{
	return interGrain_;
}

/*
 * Energy / Force
 */

// Set SpeciesBond reference
void Bond::setSpeciesBond(SpeciesBond* bondRef)
{
	speciesBond_ = bondRef;
}

// Return parent Species
SpeciesBond* Bond::speciesBond() const
{
	return speciesBond_;
}

// Return energy for specified distance
double Bond::energy(double distance) const
{
	return speciesBond_->energy(distance);
}

// Return force multiplier for specified distance
double Bond::force(double distance) const
{
	return speciesBond_->force(distance);
}
