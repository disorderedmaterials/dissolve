/*
	*** Bond Definition
	*** src/lib/classes/bond.cpp
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

#include "classes/bond.h"
#include "classes/atom.h"
#include "classes/species.h"
#include "base/comms.h"

/*!
 * \brief Constructor
 * \details Constructor for Bond. 
 */
Bond::Bond() : ListItem<Bond>()
{
	parent_ = NULL;
	molecule_ = NULL;
	i_ = NULL;
	j_ = NULL;
	nAttached_[0] = 0;
	nAttached_[1] = 0;
	attached_[0] = NULL;
	attached_[1] = NULL;
	interGrain_ = FALSE;
	equilibrium_ = 1.0;
	forceConstant_ = 4184.0;
}

/*!
 * \brief Destructor
 * \details Constructor for Bond. 
 */
Bond::~Bond()
{
	clear();
}

/*!
 * \brief Clear all data
 */
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
// Basic Data
*/

/*!
 * \brief Set parent Species
 */
void Bond::setParent(Species* parent)
{
	parent_ = parent;
}

/*!
 * \brief Return parent Species
 */
Species *Bond::parent() const
{
	return parent_;
}

/*!
 * \brief Set parent Molecule
 */
void Bond::setMolecule(Molecule* parent)
{
	molecule_ = parent;
}

/*!
 * \brief Return parent Molecule
 */
Molecule* Bond::molecule() const
{
	return molecule_;
}

/*
// Atom Information
*/

/*!
 * \brief Set Atoms involved in Bond
 */
void Bond::setAtoms(Atom* i, Atom* j)
{
	i_ = i;
	j_ = j;
#ifdef CHECKS
	if (i_ == NULL) msg.error("NULL_POINTER - NULL pointer passed for Atom i in Bond::set().\n");
	if (j_ == NULL) msg.error("NULL_POINTER - NULL pointer passed for Atom j in Bond::set().\n");
#endif
}

/*!
 * \brief Return first Atom involved in Bond
 */
Atom *Bond::i() const
{
	return i_;
}

/*!
 * \brief Return second Atom involved in Bond
 */
Atom *Bond::j() const
{
	return j_;
}

/*!
 * \brief Return the 'other' Atom in the Bond
 */
Atom *Bond::partner(Atom* i) const
{
	return (i == i_ ? j_ : i_);
}

/*!
 * \brief Return index (in parent Species) of first Atom
 */
int Bond::indexI() const
{
#ifdef CHECKS
	if (i_ == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer 'i' found in Bond::indexI(). Returning 0...\n");
		return 0;
	}
#endif
	return i_->index();
}

/*!
 * \brief Return index (in parent Species) of second Atom
 */
int Bond::indexJ() const
{
#ifdef CHECKS
	if (j_ == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer 'j' found in Bond::indexJ(). Returning 0...\n");
		return 0;
	}
#endif
	return j_->index();
}

/*!
 * \brief Return whether Atoms in Angle match those specified
 */
bool Bond::matches(Atom* i, Atom* j) const
{
	if ((i_ == i) && (j_ == j)) return TRUE;
	if ((i_ == j) && (j_ == i)) return TRUE;
	return FALSE;
}

/*
// Interaction Parameters
*/

/*!
 * \brief Set nominal equilibrium Bond length
 */
void Bond::setEquilibrium(double rEq)
{
	equilibrium_ = rEq;
}

/*!
 * \brief Return nominal equilibrium Bond length
 */
double Bond::equilibrium() const
{
	return equilibrium_;
}

/*!
 * \brief Set force constant
 */
void Bond::setForceConstant(double k)
{
	forceConstant_ = k;
}

/*!
 * \brief Return force constant
 */
double Bond::forceConstant() const
{
	return forceConstant_;
}

/*!
 * \brief Create attached Atom array
 * \details Creates and NULLifies the attached atom array for the terminus specified.
 */
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

/*!
 * \brief Set attached Atoms for terminus specified
 */
void Bond::setAttachedAtoms(int terminus, const RefList<Atom,int>& atoms)
{
	createAttachedAtomArray(terminus, atoms.nItems());
	int index = 0;
	for (RefListItem<Atom,int>* refAtom = atoms.first(); refAtom != NULL; refAtom = refAtom->next) attached_[terminus][index++] = refAtom->item;

	Dnchar s(-1, "--> For Bond between Atoms %i-%i, terminus %i moves %i Atoms :", indexI()+1, indexJ()+1, terminus+1, nAttached_[terminus]);
	for (int n=0; n<nAttached_[terminus]; ++n) s.strcatf(" %i", attached_[terminus][n]->userIndex());
	msg.print("%s\n", s.get());
}

/*!
 * \brief Return number of attached Atoms for terminus specified
 */
int Bond::nAttached(int terminus) const
{
	return nAttached_[terminus];
}

/*!
 * \brief Return array of attached Atoms for terminus specified
 */
Atom** Bond::attached(int terminus) const
{
	return attached_[terminus];
}

/*!
 * \brief Set whether this Bond is interGrain
 */
void Bond::setInterGrain(bool b)
{
	interGrain_ = b;
}

/*!
 * \brief Return whether this Bond is interGrain
 */
bool Bond::interGrain() const
{
	return interGrain_;
}

/*!
 * \brief Return energy for specified distance
 */
double Bond::energy(double distance) const
{
	double delta = distance - equilibrium_;
	return 0.5*forceConstant_*delta*delta;
}

/*!
 * \brief Return force multiplier for specified distance
 */
double Bond::force(double distance) const
{
	return -forceConstant_*(distance-equilibrium_);
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
bool Bond::broadcast(const List<Atom>& atoms)
{
#ifdef PARALLEL
	int buffer[2];

	// Put atom indices into buffer and send
	if (Comm.master())
	{
		buffer[0] = indexI();
		buffer[1] = indexJ();
	}
	if (!Comm.broadcast(buffer, 2)) return FALSE;
	
	// Slaves now take Atom pointers from supplied List
	if (Comm.slave())
	{
		setAtoms(atoms.item(buffer[0]), atoms.item(buffer[1]));
		if (i_ != NULL) i_->addBond(this);
		if (j_ != NULL) j_->addBond(this);
	}
	
	// Send parameter info
	if (!Comm.broadcast(&equilibrium_, 1)) return FALSE;
	if (!Comm.broadcast(&forceConstant_, 1)) return FALSE;
#endif
	return TRUE;
}
