/*
	*** SpeciesBond Definition
	*** src/classes/speciesbond.cpp
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

#include "classes/speciesbond.h"
#include "classes/species.h"
#include "base/comms.h"

// Constructor
SpeciesBond::SpeciesBond() : ListItem<SpeciesBond>()
{
	parent_ = NULL;
	i_ = NULL;
	j_ = NULL;
	nAttached_[0] = 0;
	nAttached_[1] = 0;
	attachedAtoms_[0] = NULL;
	attachedAtoms_[1] = NULL;
	attachedAtomIndices_[0] = NULL;
	attachedAtomIndices_[1] = NULL;
	equilibrium_ = 1.0;
	forceConstant_ = 4184.0;
}

// Destructor
SpeciesBond::~SpeciesBond()
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
void SpeciesBond::setParent(Species* parent)
{
	parent_ = parent;
}

// Return parent Species
Species *SpeciesBond::parent() const
{
	return parent_;
}

/*
 * SpeciesAtom Information
 */

// Set SpeciesAtoms involved in SpeciesBond
void SpeciesBond::setAtoms(SpeciesAtom* i, SpeciesAtom* j)
{
	i_ = i;
	j_ = j;
#ifdef CHECKS
	if (i_ == NULL) msg.error("NULL_POINTER - NULL pointer passed for SpeciesAtom i in SpeciesBond::set().\n");
	if (j_ == NULL) msg.error("NULL_POINTER - NULL pointer passed for SpeciesAtom j in SpeciesBond::set().\n");
#endif
}

// Return first SpeciesAtom involved in SpeciesBond
SpeciesAtom *SpeciesBond::i() const
{
	return i_;
}

// Return second SpeciesAtom involved in SpeciesBond
SpeciesAtom *SpeciesBond::j() const
{
	return j_;
}

/*
 * \brief Return the 'other' SpeciesAtom in the SpeciesBond
 */
SpeciesAtom *SpeciesBond::partner(SpeciesAtom* i) const
{
	return (i == i_ ? j_ : i_);
}

// Return index (in parent Species) of first SpeciesAtom
int SpeciesBond::indexI() const
{
#ifdef CHECKS
	if (i_ == NULL)
	{
		msg.error("NULL_POINTER - NULL SpeciesAtom pointer 'i' found in SpeciesBond::indexI(). Returning 0...\n");
		return 0;
	}
#endif
	return i_->index();
}

// Return index (in parent Species) of second SpeciesAtom
int SpeciesBond::indexJ() const
{
#ifdef CHECKS
	if (j_ == NULL)
	{
		msg.error("NULL_POINTER - NULL SpeciesAtom pointer 'j' found in SpeciesBond::indexJ(). Returning 0...\n");
		return 0;
	}
#endif
	return j_->index();
}

// Return whether SpeciesAtoms in Angle match those specified
bool SpeciesBond::matches(SpeciesAtom* i, SpeciesAtom* j) const
{
	if ((i_ == i) && (j_ == j)) return true;
	if ((i_ == j) && (j_ == i)) return true;
	return false;
}

/*
 * Interaction Parameters
 */

// Set nominal equilibrium SpeciesBond length
void SpeciesBond::setEquilibrium(double rEq)
{
	equilibrium_ = rEq;
}

// Return nominal equilibrium SpeciesBond length
double SpeciesBond::equilibrium() const
{
	return equilibrium_;
}

// Set force constant
void SpeciesBond::setForceConstant(double k)
{
	forceConstant_ = k;
}

// Return force constant
double SpeciesBond::forceConstant() const
{
	return forceConstant_;
}

// Create attached SpeciesAtom array
void SpeciesBond::createAttachedAtomArrays(int terminus, int size)
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

// Set attached SpeciesAtoms for terminus specified
void SpeciesBond::setAttachedAtoms(int terminus, const RefList<SpeciesAtom,int>& atoms)
{
	createAttachedAtomArrays(terminus, atoms.nItems());
	int index = 0;
	for (RefListItem<SpeciesAtom,int>* refSpeciesAtom = atoms.first(); refSpeciesAtom != NULL; refSpeciesAtom = refSpeciesAtom->next)
	{
		attachedAtoms_[terminus][index] = refSpeciesAtom->item;
		attachedAtomIndices_[terminus][index] = refSpeciesAtom->item->index();
		++index;
	}

	Dnchar s(-1, "--> For bond between atoms %i-%i, terminus %i moves %i other atoms:", indexI()+1, indexJ()+1, terminus+1, nAttached_[terminus]);
	for (int n=0; n<nAttached_[terminus]; ++n) s.strcatf(" %i", attachedAtoms_[terminus][n]->userIndex());
	msg.print("%s\n", s.get());
}

// Return number of attached SpeciesAtoms for terminus specified
int SpeciesBond::nAttached(int terminus) const
{
	return nAttached_[terminus];
}

// Return array of attached SpeciesAtoms for terminus specified
SpeciesAtom** SpeciesBond::attachedAtoms(int terminus) const
{
	return attachedAtoms_[terminus];
}

// Return array of attached indices for terminus specified
int* SpeciesBond::attachedIndices(int terminus) const
{
	return attachedAtomIndices_[terminus];
}

// Set whether this Bond is interGrain
void SpeciesBond::setInterGrain(bool b)
{
	interGrain_ = b;
}

// Return whether this Bond is interGrain
bool SpeciesBond::interGrain() const
{
	return interGrain_;
}

// Return energy for specified distance
double SpeciesBond::energy(double distance) const
{
	double delta = distance - equilibrium_;
	return 0.5*forceConstant_*delta*delta;
}

// Return force multiplier for specified distance
double SpeciesBond::force(double distance) const
{
	return -forceConstant_*(distance-equilibrium_);
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool SpeciesBond::broadcast(const List<SpeciesAtom>& atoms)
{
#ifdef PARALLEL
	int buffer[2];

	// Put atom indices into buffer and send
	if (Comm.master())
	{
		buffer[0] = indexI();
		buffer[1] = indexJ();
	}
	if (!Comm.broadcast(buffer, 2)) return false;
	
	// Slaves now take SpeciesAtom pointers from supplied List
	if (Comm.slave())
	{
		setAtoms(atoms.item(buffer[0]), atoms.item(buffer[1]));
		if (i_ != NULL) i_->addBond(this);
		if (j_ != NULL) j_->addBond(this);
	}
	
	// Send parameter info
	if (!Comm.broadcast(&equilibrium_, 1)) return false;
	if (!Comm.broadcast(&forceConstant_, 1)) return false;
#endif
	return true;
}
