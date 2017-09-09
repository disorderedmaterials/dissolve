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
Angle::Angle() : ListItem<Angle>()
{
	parent_ = NULL;
	molecule_ = NULL;
	i_ = NULL;
	j_ = NULL;
	k_ = NULL;
	nAttached_[0] = 0;
	nAttached_[1] = 0;
	attached_[0] = NULL;
	attached_[1] = NULL;
	interGrain_ = false;
	equilibrium_ = 109.5;
	forceConstant_ = 418.4;
}

// Destructor
Angle::~Angle()
{
}

/*
 * Basic Data
 */

// Set parent Species
void Angle::setParent(Species* parent)
{
	parent_ = parent;
}

// Return parent Species
Species* Angle::parent() const
{
	return parent_;
}

// Set parent Molecule
void Angle::setMolecule(Molecule* parent)
{
	molecule_ = parent;
}

// Return parent Molecule
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

// Return index (in parent Species) of first Atom
int Angle::indexI() const
{
#ifdef CHECKS
	if (i_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer 'i' found in Angle::indexI(). Returning 0...\n");
		return 0;
	}
#endif
	return i_->index();
}

// Return index (in parent Species) of second (central) Atom
int Angle::indexJ() const
{
#ifdef CHECKS
	if (j_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer 'j' found in Angle::indexJ(). Returning 0...\n");
		return 0;
	}
#endif
	return j_->index();
}

// Return index (in parent Species) of third Atom
int Angle::indexK() const
{
#ifdef CHECKS
	if (k_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer 'k' found in Angle::indexK(). Returning 0...\n");
		return 0;
	}
#endif
	return k_->index();
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
 * Interaction Parameters
 */

// Set equilibrium angle
void Angle::setEquilibrium(double rEq)
{
	equilibrium_ = rEq;
}

// Return equilibrium angle
double Angle::equilibrium() const
{
	return equilibrium_;
}

// Set force constant
void Angle::setForceConstant(double k)
{
	forceConstant_ = k;
}

// Return force constant
double Angle::forceConstant() const
{
	return forceConstant_;
}

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

	CharString s(-1, "--> For Angle between Atoms %i-%i-%i, terminus %i moves %i Atoms :", indexI()+1, indexJ()+1, indexK()+1, terminus+1, nAttached_[terminus]);
	for (int n=0; n<nAttached_[terminus]; ++n) s.strcatf(" %i", attached_[terminus][n]->userIndex());
	Messenger::printVerbose("%s\n", s.get());
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

// Set whether this Angle is interGrain
void Angle::setInterGrain(bool b)
{
	interGrain_ = b;
}

// Return whether this Angle is interGrain
bool Angle::interGrain() const
{
	return interGrain_;
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
