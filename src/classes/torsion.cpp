/*
	*** Torsion Definition
	*** src/classes/torsion.cpp
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

#include "classes/torsion.h"
#include "classes/speciestorsion.h"
#include "classes/atom.h"
#include "base/processpool.h"

// Constructor
Torsion::Torsion() : DynamicArrayObject<Torsion>()
{
	speciesTorsion_ = NULL;
	molecule_ = NULL;
	i_ = NULL;
	j_ = NULL;
	k_ = NULL;
	l_ = NULL;
	nAttached_[0] = 0;
	nAttached_[1] = 0;
	attached_[0] = NULL;
	attached_[1] = NULL;
}

// Destructor
Torsion::~Torsion()
{
}

/*
 * Basic Data
 */

// Set Molecule in which this Torsion exists
void Torsion::setMolecule(Molecule* parent)
{
	molecule_ = parent;
}

// Return Molecule in which this Torsion exists
Molecule* Torsion::molecule() const
{
	return molecule_;
}

/*
 * Atom Information
 */

// Set Atoms involved in Torsion
void Torsion::setAtoms(Atom* i, Atom* j, Atom* k, Atom* l)
{
	i_ = i;
	j_ = j;
	k_ = k;
	l_ = l;
#ifdef CHECKS
	if (i_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* i in Torsion::set().\n");
	if (j_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* j in Torsion::set().\n");
	if (k_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* k in Torsion::set().\n");
	if (l_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* l in Torsion::set().\n");
#endif
}

// Return first Atom involved in Torsion
Atom* Torsion::i() const
{
	return i_;
}

// Return second (central) Atom involved in Torsion
Atom* Torsion::j() const
{
	return j_;
}

// Return third Atom involved in Torsion
Atom* Torsion::k() const
{
	return k_;
}

// Return fourth Atom involved in Torsion
Atom* Torsion::l() const
{
	return l_;
}

// Return whether Atoms in Torsion match those specified
bool Torsion::matches(Atom* i, Atom* j, Atom* k, Atom* l) const
{
	if (i_ == i)
	{
		if (j_ != j) return false;
		if (k_ != k) return false;
		if (l_ == l) return true;
	}
	else if (i_ == l)
	{
		if (j_ != k) return false;
		if (k_ != j) return false;
		if (l_ == i) return true;
	}

	return false;
}

/*
 * Connections
 */

// Create attached Atom array
void Torsion::createAttachedAtomArray(int terminus, int size)
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
void Torsion::setAttachedAtoms(int terminus, const RefList<Atom,int>& atoms)
{
	createAttachedAtomArray(terminus, atoms.nItems());
	int index = 0;
	for (RefListItem<Atom,int>* refAtom = atoms.first(); refAtom != NULL; refAtom = refAtom->next) attached_[terminus][index++] = refAtom->item;
}

// Return number of attached Atoms for terminus specified
int Torsion::nAttached(int terminus) const
{
	return nAttached_[terminus];
}

// Return array of attached Atoms for terminus specified
Atom** Torsion::attached(int terminus) const
{
	return attached_[terminus];
}

/*
 * Energy / Force
 */

// Set SpeciesTorsion reference
void Torsion::setSpeciesTorsion(SpeciesTorsion* torsionRef)
{
	speciesTorsion_ = torsionRef;
}

// Return SpeciesTorsion reference
SpeciesTorsion* Torsion::speciesTorsion()
{
	return speciesTorsion_;
}

// Return energy for specified torsion
double Torsion::energy(double torsionInDegrees) const
{
	return speciesTorsion_->energy(torsionInDegrees);
}

// Return force multiplier for specified torsion
double Torsion::force(double torsionInDegrees) const
{
	return speciesTorsion_->force(torsionInDegrees);
}
