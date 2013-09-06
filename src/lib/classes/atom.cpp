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
Atom::Atom()
{
	element_ = 0;
	charge_ = 0.0;
	atomTypeIndex_ = -1;
	index_ = Atom::UnusedAtom;
	molecule_ = NULL;
	moleculeAtomIndex_ = 0;
	r_.zero();
	grain_ = NULL;
	cell_ = NULL;
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
 * \brief Set charge of atom
 * \details Store a charge associated to this atom. Such charges can then be used in the generation of suitable pair potentials.
 */
void Atom::setCharge(double charge)
{
	charge_ = charge;
}

/*!
 * \brief Return charge of atom
 */
double Atom::charge() const
{
	return charge_;
}

/*!
 * \brief Set AtomType index for atom
 */
void Atom::setAtomTypeIndex(int id)
{
	atomTypeIndex_ = id;
}

/*!
 * \brief Return AtomType index for atom
 */
int Atom::atomTypeIndex() const
{
	return atomTypeIndex_;
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

/*!
 * \brief Set molecule and local atom index (0->[N-1])
 */
void Atom::setMolecule(Molecule* mol, int atomIndex)
{
	molecule_ = mol;
	moleculeAtomIndex_ = atomIndex;
}

/*!
 * \brief Return associated molecule (0->[N-1])
 */
Molecule* Atom::molecule() const
{
	return molecule_;
}

/*!
 * \brief Return local atom index in molecule (0->[N-1])
 */
int Atom::moleculeAtomIndex() const
{
	return moleculeAtomIndex_;
}

/*!
 * \brief Set cell in which the atom exists
 */
void Atom::setCell(Cell* cell)
{
	cell_ = cell;
}

/*!
 * \brief Return cell in which the atom exists
 */
Cell* Atom::cell()
{
	return cell_;
}

// Copy properties from supplied Atom
void Atom::copyProperties(const Atom* source)
{
	r_ = source->r_;
	element_ = source->element_;
	atomTypeIndex_ = source->atomTypeIndex_;
	charge_ = source->charge_;
	index_ = source->index_;
	molecule_ = source->molecule_;
	moleculeAtomIndex_ = source->moleculeAtomIndex_;
	grain_ = source->grain_;
	cell_ = source->cell_;
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
		msg.print("BAD_USAGE - Tried to set atom %i's grain for a second time.\n", index_);
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
		msg.error("NULL_POINTER - NULL grain pointer found in Atom::setCoordinates() (atom id = %i).\n", index_);
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
bool Atom::broadcast()
{
#ifdef PARALLEL
	int index;

	if (!Comm.broadcast(&element_, 1)) return false;
	if (!Comm.broadcast(r_)) return false;
	if (!Comm.broadcast(&charge_, 1)) return false;
	if (!Comm.broadcast(&atomTypeIndex_, 1)) return false;
#endif
	return true;
}
