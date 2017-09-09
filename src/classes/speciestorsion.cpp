/*
	*** SpeciesTorsion Definition
	*** src/classes/speciestorsion.cpp
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

#include "classes/speciestorsion.h"
#include "classes/speciesatom.h"
#include "base/processpool.h"

// Constructor
SpeciesTorsion::SpeciesTorsion() : ListItem<SpeciesTorsion>()
{
	parent_ = NULL;
	i_ = NULL;
	j_ = NULL;
	k_ = NULL;
	l_ = NULL;
	nAttached_[0] = 0;
	nAttached_[1] = 0;
	attachedAtoms_[0] = NULL;
	attachedAtoms_[1] = NULL;
	attachedAtomIndices_[0] = NULL;
	attachedAtomIndices_[1] = NULL;
	equilibrium_ = 109.5;
	forceConstant_ = 418.4;
}

// Destructor
SpeciesTorsion::~SpeciesTorsion()
{
	for (int n=0; n<2; ++n)
	{
		if (attachedAtoms_[n] != NULL) delete[] attachedAtoms_[n];
		attachedAtoms_[n] = NULL;
		if (attachedAtomIndices_[n] != NULL) delete[] attachedAtomIndices_[n];
		attachedAtomIndices_[n] = NULL;
		nAttached_[n] = 0;
	}
}

/*
 * Basic Data
 */

// Set parent Species
void SpeciesTorsion::setParent(Species* parent)
{
	parent_ = parent;
}

// Return parent Species
Species* SpeciesTorsion::parent() const
{
	return parent_;
}

/*
 * Atom Information
 */

// Set Atoms involved in Torsion
void SpeciesTorsion::setAtoms(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l)
{
	i_ = i;
	j_ = j;
	k_ = k;
#ifdef CHECKS
	if (i_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* i in SpeciesTorsion::set().\n");
	if (j_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* j in SpeciesTorsion::set().\n");
	if (k_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* k in SpeciesTorsion::set().\n");
#endif
}

// Return first SpeciesAtom involved in Torsion
SpeciesAtom *SpeciesTorsion::i() const
{
	return i_;
}

// Return second (central) SpeciesAtom involved in Torsion
SpeciesAtom *SpeciesTorsion::j() const
{
	return j_;
}

// Return third SpeciesAtom involved in Torsion
SpeciesAtom *SpeciesTorsion::k() const
{
	return k_;
}

// Return index (in parent Species) of first SpeciesAtom
int SpeciesTorsion::indexI() const
{
#ifdef CHECKS
	if (i_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'i' found in SpeciesTorsion::indexI(). Returning 0...\n");
		return 0;
	}
#endif
	return i_->index();
}

// Return index (in parent Species) of second (central) SpeciesAtom
int SpeciesTorsion::indexJ() const
{
#ifdef CHECKS
	if (j_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'j' found in SpeciesTorsion::indexJ(). Returning 0...\n");
		return 0;
	}
#endif
	return j_->index();
}

// Return index (in parent Species) of third SpeciesAtom
int SpeciesTorsion::indexK() const
{
#ifdef CHECKS
	if (k_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'k' found in SpeciesTorsion::indexK(). Returning 0...\n");
		return 0;
	}
#endif
	return k_->index();
}

// Return whether Atoms in Torsion match those specified
bool SpeciesTorsion::matches(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l) const
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
 * Interaction Parameters
 */

// Set equilibrium angle
void SpeciesTorsion::setEquilibrium(double rEq)
{
	equilibrium_ = rEq;
}

// Return equilibrium angle
double SpeciesTorsion::equilibrium() const
{
	return equilibrium_;
}

// Set force constant
void SpeciesTorsion::setForceConstant(double k)
{
	forceConstant_ = k;
}

// Return force constant
double SpeciesTorsion::forceConstant() const
{
	return forceConstant_;
}

// Create attached Atom array
void SpeciesTorsion::createAttachedAtomArrays(int terminus, int size)
{
	if (attachedAtoms_[terminus] != NULL) delete[] attachedAtoms_[terminus];
	attachedAtoms_[terminus] = NULL;
	if (attachedAtomIndices_[terminus] != NULL) delete[] attachedAtomIndices_[terminus];
	attachedAtomIndices_[terminus] = NULL;
	nAttached_[terminus] = size;

	if (nAttached_[terminus] != 0)
	{
		attachedAtoms_[terminus] = new SpeciesAtom*[nAttached_[terminus]];
		attachedAtomIndices_[terminus] = new int[nAttached_[terminus]];
		for (int n=0; n<nAttached_[terminus]; ++n)
		{
			attachedAtoms_[terminus][n] = NULL;
			attachedAtomIndices_[terminus][n] = -1;
		}
	}
}

// Set attached Atoms for terminus specified
void SpeciesTorsion::setAttachedAtoms(int terminus, const RefList<SpeciesAtom,int>& atoms)
{
	createAttachedAtomArrays(terminus, atoms.nItems());
	int index = 0;
	for (RefListItem<SpeciesAtom,int>* refAtom = atoms.first(); refAtom != NULL; refAtom = refAtom->next)
	{
		attachedAtoms_[terminus][index] = refAtom->item;
		attachedAtomIndices_[terminus][index] = refAtom->item->index();
		++index;
	}

	CharString s("--> For angle between atoms %i-%i-%i, terminus %i moves %i other atoms :", indexI()+1, indexJ()+1, indexK()+1, terminus+1, nAttached_[terminus]);
	for (int n=0; n<nAttached_[terminus]; ++n) s.strcatf(" %i", attachedAtoms_[terminus][n]->userIndex());
	Messenger::print("%s\n", s.get());
}

// Return number of attached Atoms for terminus specified
int SpeciesTorsion::nAttached(int terminus) const
{
	return nAttached_[terminus];
}

// Return array of attached Atoms for terminus specified
SpeciesAtom** SpeciesTorsion::attachedAtoms(int terminus) const
{
	return attachedAtoms_[terminus];
}

// Return array of attached indices for terminus specified
int* SpeciesTorsion::attachedIndices(int terminus) const
{
	return attachedAtomIndices_[terminus];
}

// Set whether this Torsion is interGrain
void SpeciesTorsion::setInterGrain(bool b)
{
	interGrain_ = b;
}

// Return whether this Torsion is interGrain
bool SpeciesTorsion::interGrain() const
{
	return interGrain_;
}

// Return energy for specified angle
double SpeciesTorsion::energy(double angleInDegrees) const
{
	double delta = (angleInDegrees - equilibrium_)/DEGRAD;
	return 0.5*forceConstant_*delta*delta;
}

// Return force multiplier for specified angle
double SpeciesTorsion::force(double angleInDegrees) const
{
	// Set initial derivative of angle w.r.t. cos(angle)
	double dU_dtheta = -1.0 / sin(angleInDegrees/DEGRAD);

	// Chain rule - multiply by derivative of energy w.r.t. angle (harmonic form)
	dU_dtheta *= -forceConstant_*((angleInDegrees-equilibrium_)/DEGRAD);

	return dU_dtheta;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool SpeciesTorsion::broadcast(ProcessPool& procPool, const List<SpeciesAtom>& atoms)
{
#ifdef PARALLEL
	int buffer[3];

	// Put atom indices into buffer and send
	if (procPool.isMaster())
	{
		buffer[0] = indexI();
		buffer[1] = indexJ();
		buffer[2] = indexK();
	}
	if (!procPool.broadcast(buffer, 3)) return false;
	
	// Slaves now take Atom pointers from supplied List
	if (procPool.isSlave())
	{
		i_ = atoms.item(buffer[0]);
		j_ = atoms.item(buffer[1]);
		k_ = atoms.item(buffer[2]);
	}
	
	// Send parameter info
	if (!procPool.broadcast(&equilibrium_, 1)) return false;
	if (!procPool.broadcast(&forceConstant_, 1)) return false;
#endif
	return true;
}
