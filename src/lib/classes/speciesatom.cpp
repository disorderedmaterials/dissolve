/*
	*** SpeciesAtom Definition
	*** src/lib/classes/speciesatom.cpp
	Copyright T. Youngs 2012-2013

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

#include "classes/speciesatom.h"
#include "classes/atomtype.h"
#include "classes/speciesbond.h"
#include "base/comms.h"

/*!
 * \brief Constructor
 * \details Constructor for SpeciesAtom. 
 */
SpeciesAtom::SpeciesAtom()
{
	element_ = 0;
	charge_ = 0.0;
	atomType_ = NULL;
	grain_ = NULL;
	r_.zero();
	index_= -1;
}

/*!
 * \brief Destructor
 * \details Constructor for SpeciesAtom. 
 */
SpeciesAtom::~SpeciesAtom()
{
}

/*!
 * \brief Set Species parent
 */
void SpeciesAtom::setParent(Species* sp)
{
	parent_ = sp;
}

/*!
 * \brief Return species parent
 */
Species* SpeciesAtom::parent()
{
	return parent_;
}

/*!
 * \brief Set basic SpeciesAtom properties
 * \details Set the element and Cartesian coordinates of the SpeciesAtom
 */
void SpeciesAtom::set(int element, double rx, double ry, double rz)
{
	element_ = element;
	r_.set(rx, ry, rz);
}

/*!
 * \brief Set basic SpeciesAtom properties
 * \details Set the element and Cartesian coordinates of the atom. This is an overloaded function, provided for convenience.
 */
void SpeciesAtom::set(int element, const Vec3<double> r)
{
	element_ = element;
	r_ = r;
}

/*!
 * \brief Set atomic element
 */
void SpeciesAtom::setElement(int el)
{
	element_ = el;
}

/*!
 * \brief Return atomic element
 */
int SpeciesAtom::element() const
{
	return element_;
}

/*!
 * \brief Return coordinates
 * \details Return the current coordinates of the SpeciesAtom, be they absolute or Grain-local
 */
const Vec3<double> &SpeciesAtom::r() const
{
	return r_;
}

/*!
 * \brief Set charge of SpeciesAtom
 * \details Store a charge associated to this SpeciesAtom. Such charges can then be used in the generation of suitable pair potentials.
 */
void SpeciesAtom::setCharge(double charge)
{
	charge_ = charge;
}

/*!
 * \brief Return charge of SpeciesAtom
 */
double SpeciesAtom::charge() const
{
	return charge_;
}

/*!
 * \brief Set AtomType of SpeciesAtom
 */
void SpeciesAtom::setAtomType(AtomType* at)
{
	// Check elements
	if (at && (at->element() != element_))
	{
		msg.print("Warning: Refused to assign AtomType '%s' to SpeciesAtom, since their elements differ.\n", at->name());
	}
	else atomType_ = at;
}

/*!
 * \brief Return SpeciesAtomType of SpeciesAtom
 */
AtomType* SpeciesAtom::atomType() const
{
	return atomType_;
}

/*!
 * \brief Set List index (0->[N-1])
 */
void SpeciesAtom::setIndex(int id)
{
	index_ = id;
}

/*!
 * \brief Return List index (0->[N-1])
 */
int SpeciesAtom::index() const
{
	return index_;
}

/*!
 * \brief Return 'user' index (1->N)
 */
int SpeciesAtom::userIndex() const
{
	return index_+1;
}

/*
 * Containing Grain
 */

/*!
 * \brief Set grain to which this atom belongs
 */
void SpeciesAtom::setGrain(SpeciesGrain* grain)
{
	grain_ = grain;
}

/*!
 * \brief Return grain to which this atom belongs
 */
SpeciesGrain* SpeciesAtom::grain()
{
	return grain_;
}

/*
// Bond Information
*/

/*!
 * \brief Add Bond reference
 */
void SpeciesAtom::addBond(SpeciesBond* b)
{
	bonds_.addUnique(b);
}

/*!
 * \brief Remove Bond reference
 */
void SpeciesAtom::removeBond(SpeciesBond* b)
{
	bonds_.remove(b);
}

/*!
 * \brief Clear all Bond references
 */
void SpeciesAtom::clearBonds()
{
	bonds_.clear();
}

/*!
 * \brief Return number of Bond references
 */
int SpeciesAtom::nBonds() const
{
	return bonds_.nItems();
}

/*!
 * \brief Return first Bond reference
 */
RefListItem<SpeciesBond,int>* SpeciesAtom::bonds()
{
	return bonds_.first();
}

/*!
 * \brief Return whether Bond to specified Atom exists
 */
SpeciesBond* SpeciesAtom::hasBond(SpeciesAtom* j)
{
	for (RefListItem<SpeciesBond,int>* ri = bonds_.first(); ri != NULL; ri = ri->next) if (ri->item->partner(this) == j) return ri->item;
	return NULL;
}

/*
 * Coordinate Manipulation
 */

/*!
 * \brief Set coordinates of atom
 * \details Set the coordinates of the atom
 */
void SpeciesAtom::setCoordinates(const Vec3<double>& newr)
{
	r_ = newr;
}

/*!
 * \brief Translate coordinates of atom
 * \details Set the coordinates of the atom
 */
void SpeciesAtom::translateCoordinates(const Vec3<double>& delta)
{
	r_ += delta;
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
bool SpeciesAtom::broadcast(const List<AtomType>& atomTypes)
{
#ifdef PARALLEL
	int index;

	if (!Comm.broadcast(&element_, 1)) return false;
	if (!Comm.broadcast(r_)) return false;
	if (!Comm.broadcast(&charge_, 1)) return false;
	
	// Must get index of AtomType...
	if (Comm.master()) index = atomTypes.indexOf(atomType_);
	if (!Comm.broadcast(&index, 1)) return false;
	atomType_ = atomTypes.item(index);
	
	// Bond information - added in Species::broadcast().
#endif
	return true;
}
