/*
	*** Angle Definition
	*** src/classes/angle.cpp
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

#include "classes/angle.h"
#include "classes/atom.h"
#include "classes/species.h"
#include "base/processpool.h"

// Constructor
Angle::Angle() : DynamicArrayObject<Angle>()
{
	molecule_ = NULL;
	i_ = NULL;
	j_ = NULL;
	k_ = NULL;
	nAttached_[0] = 0;
	nAttached_[1] = 0;
	attached_[0] = NULL;
	attached_[1] = NULL;
}

// Destructor
Angle::~Angle()
{
}

/*
 * Basic Data
 */

// Set Molecule that this Angle exists in
void Angle::setMolecule(Molecule* parent)
{
	molecule_ = parent;
}

// Return Molecule that this Angle exists in
Molecule* Angle::molecule() const
{
	return molecule_;
}

/*
 * Atom Information
 */

// Set Atoms involved in Angle
void Angle::setAtoms(Atom* i, Atom* j, Atom* k)
{
	i_ = i;
	j_ = j;
	k_ = k;
#ifdef CHECKS
	if (i_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* i in Angle::set().\n");
	if (j_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* j in Angle::set().\n");
	if (k_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* k in Angle::set().\n");
#endif
}

// Return first Atom involved in Angle
Atom *Angle::i() const
{
	return i_;
}

// Return second (central) Atom involved in Angle
Atom *Angle::j() const
{
	return j_;
}

// Return third Atom involved in Angle
Atom *Angle::k() const
{
	return k_;
}

// Return whether Atoms in Angle match those specified
bool Angle::matches(Atom* i, Atom* j, Atom* k) const
{
	if (j_ != j) return false;
	if ((i_ == i) && (k_ == k)) return true;
	if ((i_ == k) && (k_ == i)) return true;
	return false;
}

/*
 * Connections
 */

// Create attached Atom array
void Angle::createAttachedAtomArray(int terminus, int size)
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
void Angle::setAttachedAtoms(int terminus, const RefList<Atom,int>& atoms)
{
	createAttachedAtomArray(terminus, atoms.nItems());
	int index = 0;
	for (RefListItem<Atom,int>* refAtom = atoms.first(); refAtom != NULL; refAtom = refAtom->next) attached_[terminus][index++] = refAtom->item;
}

// Return number of attached Atoms for terminus specified
int Angle::nAttached(int terminus) const
{
	return nAttached_[terminus];
}

// Return array of attached Atoms for terminus specified
Atom** Angle::attached(int terminus) const
{
	return attached_[terminus];
}

/*
 * Energy / Force
 */

// Set SpeciesAngle reference
void Angle::setSpeciesAngle(SpeciesAngle* angleRef)
{
	speciesAngle_ = angleRef;
}

// Return energy for specified angle
double Angle::energy(double angleInDegrees) const
{
	return speciesAngle_->energy(angleInDegrees);
}

// Return force multiplier for specified angle
double Angle::force(double angleInDegrees) const
{
	return speciesAngle_->force(angleInDegrees);
}
