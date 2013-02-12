/*
	*** Atom Definition
	*** src/lib/classes/atom.cpp
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

#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/grain.h"
#include "base/comms.h"

/*!
 * \brief Constructor
 * \details Constructor for Atom. 
 */
Atom::Atom() : ListItem<Atom>()
{
	element_ = 0;
	charge_ = 0.0;
	atomType_ = NULL;
	index_ = -100;
	r_.zero();
	grain_ = NULL;
}

/*!
 * \brief Destructor
 * \details Constructor for Atom. 
 */
Atom::~Atom()
{
}

/*
// Properties
*/

/*!
 * \brief Set basic Atom properties
 * \details Set the element and Cartesian coordinates of the Atom
 */
void Atom::set(int element, double rx, double ry, double rz)
{
	element_ = element;
	r_.set(rx, ry, rz);
}

/*!
 * \brief Set basic Atom properties
 * \details Set the element and Cartesian coordinates of the atom. This is an overloaded function, provided for convenience.
 */
void Atom::set(int element, const Vec3<double> r)
{
	element_ = element;
	r_ = r;
}

/*!
 * \brief Set atomic element
 */
void Atom::setElement(int el)
{
	element_ = el;
}

/*!
 * \brief Return atomic element
 */
int Atom::element() const
{
	return element_;
}

/*!
 * \brief Return coordinates
 * \details Return the current coordinates of the Atom, be they absolute or Grain-local
 */
const Vec3<double> &Atom::r() const
{
	return r_;
}

/*!
 * \brief Set charge of Atom
 * \details Store a charge associated to this Atom. Such charges can then be used in the generation of suitable pair potentials.
 */
void Atom::setCharge(double charge)
{
	charge_ = charge;
}

/*!
 * \brief Return charge of Atom
 */
double Atom::charge() const
{
	return charge_;
}

/*!
 * \brief Set AtomType of Atom
 */
void Atom::setAtomType(AtomType* at)
{
	// Check elements
	if (at && (at->element() != element_))
	{
		msg.print("Warning: Refused to assign AtomType '%s' to Atom, since their elements differ.\n", at->name());
	}
	atomType_ = at;
}

/*!
 * \brief Return AtomType of Atom
 */
AtomType* Atom::atomType() const
{
	return atomType_;
}

/*!
 * \brief Set List index (0->[N-1])
 */
void Atom::setIndex(int id)
{
	index_ = id;
}

/*!
 * \brief Return List index (0->[N-1])
 */
int Atom::index() const
{
	return index_;
}

/*!
 * \brief Return 'user' index (1->N)
 */
int Atom::userIndex() const
{
	return index_+1;
}

// Copy properties from supplied Atom
void Atom::copyProperties(const Atom* source)
{
	r_ = source->r_;
	element_ = source->element_;
	atomType_ = source->atomType_;
	charge_ = source->charge_;
	index_ = source->index_;
}

/*
// Bond Information
*/

/*!
 * \brief Add Bond reference
 */
void Atom::addBond(Bond* b)
{
	bonds_.addUnique(b);
}

/*!
 * \brief Remove Bond reference
 */
void Atom::removeBond(Bond* b)
{
	bonds_.remove(b);
}

/*!
 * \brief Clear all Bond references
 */
void Atom::clearBonds()
{
	bonds_.clear();
}

/*!
 * \brief Return number of Bond references
 */
int Atom::nBonds() const
{
	return bonds_.nItems();
}

/*!
 * \brief Return first Bond reference
 */
RefListItem<Bond,int>* Atom::bonds()
{
	return bonds_.first();
}

/*!
 * \brief Return whether Bond to specified Atom exists
 */
Bond* Atom::hasBond(Atom* j)
{
	for (RefListItem<Bond,int>* ri = bonds_.first(); ri != NULL; ri = ri->next) if (ri->item->partner(this) == j) return ri->item;
	return NULL;
}

/*
// Coordinate Manipulation
*/

/*!
 * \brief Set associated Grain
 */
void Atom::setGrain(Grain* grain)
{
	// Check for double-set of Grain
	if (grain_ != NULL)
	{
		msg.print("BAD_USAGE - Tried to set Atom's Grain for a second time.\n");
		return;
	}
	grain_ = grain;
}

/*!
 * \brief Return associated Grain
 */
Grain* Atom::grain() const
{
	return grain_;
}

/*!
 * \brief Set coordinates
 * \details Set the coordinates of the Atom to those specified, automatically updating the centre-of-geometry
 * of the Atom's Grain at the same time.
 */
void Atom::setCoordinates(const Vec3<double>& newr)
{
#ifdef CHECKS
	if (grain_ == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer found in Atom::setCoordinates().\n");
		return;
	}
#endif
	// Update Grain centre before we store the new position
// 	if (index_ == 3955) printf("Atom %i moving - old = %f %f %f, new = %f %f %f\n", index_, r_.x, r_.y, r_.z, newr.x, newr.y, newr.z);
	grain_->updateCentre(newr-r_);
	r_ = newr;
}

/*!
 * \brief Set coordinates
 * \details Set the coordinates of the Atom to those specified, automatically updating the centre-of-geometry
 * of the Atom's Grain at the same time. This is an overloaded version of the Vec3<double> function.
 */
void Atom::setCoordinates(double dx, double dy, double dz)
{
	setCoordinates(Vec3<double>(dx,dy,dz));
}

/*!
 * \brief Translate coordinates
 * \details Translate the coordinates of the Atom by the delta provided, automatically updating the centre-of-geometry
 * of the Atom's Grain at the same time.
 */
void Atom::translateCoordinates(const Vec3<double>& delta)
{
	setCoordinates(r_+delta);
}

/*!
 * \brief Translate coordinates
 * \details Translate the coordinates of the Atom by the delta provided, automatically updating the centre-of-geometry
 * of the Atom's Grain at the same time. This is an overloaded version of the Vec3<double> function.
 */
void Atom::translateCoordinates(double dx, double dy, double dz)
{
	setCoordinates(r_+Vec3<double>(dx,dy,dz));
}

/*!
 * \brief Set coordinates with no Grain update
 * \details Set the coordinates of the Atom, but do nothing else (i.e. do not update the associated Grain centre).
 */
void Atom::setCoordinatesNasty(const Vec3<double>& newr)
{
	r_ = newr;
}

/*!
 * \brief Translate coordinates with no Grain update
 * \details Set the coordinates of the Atom, but do nothing else (i.e. do not update the associated Grain centre).
 */
void Atom::translateCoordinatesNasty(const Vec3<double>& delta)
{
	r_ += delta;
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
bool Atom::broadcast(const List<AtomType>& atomTypes)
{
#ifdef PARALLEL
	int index;

	if (!Comm.broadcast(&element_, 1)) return FALSE;
	if (!Comm.broadcast(r_)) return FALSE;
	if (!Comm.broadcast(&charge_, 1)) return FALSE;
	
	// Must get index of AtomType...
	if (Comm.master()) index = atomTypes.indexOf(atomType_);
	if (!Comm.broadcast(&index, 1)) return FALSE;
	atomType_ = atomTypes.item(index);
	
	// Bond information - added in Species::broadcast().
#endif
	return TRUE;
}
