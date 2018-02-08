/*
	*** Atom Definition
	*** src/classes/atom.cpp
	Copyright T. Youngs 2012-2018

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
#include "classes/bond.h"
#include "classes/angle.h"
#include "classes/torsion.h"
#include "base/processpool.h"

// Constructor
Atom::Atom()
{
	// Properties
	element_ = 0;
	charge_ = 0.0;
	localTypeIndex_ = -1;
	masterTypeIndex_ = -1;
	r_.zero();

	// Location
	molecule_ = NULL;
	grain_ = NULL;
	cell_ = NULL;
}

// Destructor
Atom::~Atom()
{
}

/*
 * Properties
 */

// Set basic Atom properties
void Atom::set(int element, double rx, double ry, double rz)
{
	element_ = element;
	r_.set(rx, ry, rz);
}

// Set basic Atom properties
void Atom::set(int element, const Vec3<double> r)
{
	element_ = element;
	r_ = r;
}

// Set atomic element
void Atom::setElement(int el)
{
	element_ = el;
}

// Return atomic element
int Atom::element() const
{
	return element_;
}

// Return coordinates
const Vec3<double>& Atom::r() const
{
	return r_;
}

// Set charge of atom
void Atom::setCharge(double charge)
{
	charge_ = charge;
}

// Return charge of atom
double Atom::charge() const
{
	return charge_;
}

// Set local AtomType index
void Atom::setLocalTypeIndex(int id)
{
#ifdef CHECKS
	if (localTypeIndex_ != -1) Messenger::warn("Overwriting local AtomType index for Atom '%p'.\n", this);
#endif
	localTypeIndex_ = id;
}

// Return local AtomType index
int Atom::localTypeIndex() const
{
#ifdef CHECKS
	if (localTypeIndex_ == -1) Messenger::warn("Local AtomType index has not yet been set for Atom '%p'.\n", this);
#endif
	return localTypeIndex_;
}

// Set master AtomType index 
void Atom::setMasterTypeIndex(int id)
{
	if (masterTypeIndex_ != -1) Messenger::warn("Warning: Overwriting master AtomType index for Atom '%p'.\n", this);
	masterTypeIndex_ = id;
}

// Return master AtomType index 
int Atom::masterTypeIndex() const
{
#ifdef CHECKS
	if (masterTypeIndex_ == -1) Messenger::warn("Global AtomType index has not yet been set for Atom '%p'.\n", this);
#endif
	return masterTypeIndex_;
}

// Copy properties from supplied Atom
void Atom::copyProperties(const Atom* source)
{
	r_ = source->r_;
	element_ = source->element_;
	localTypeIndex_ = source->localTypeIndex_;
	masterTypeIndex_ = source->masterTypeIndex_;
	charge_ = source->charge_;
	molecule_ = source->molecule_;
	grain_ = source->grain_;
	cell_ = source->cell_;
}

/*
 * Location
 */


// Set parent Molecule
void Atom::setMolecule(Molecule* mol)
{
	molecule_ = mol;
}

// Return associated Molecule
Molecule* Atom::molecule() const
{
	return molecule_;
}

// Set associated Grain
void Atom::setGrain(Grain* grain)
{
	// Check for double-set of Grain
	if (grain_ != NULL)
	{
		Messenger::print("BAD_USAGE - Tried to set Atom %p's grain for a second time.\n", this);
		return;
	}
	grain_ = grain;
}

// Return associated Grain
Grain* Atom::grain() const
{
	return grain_;
}

// Set cell in which the atom exists
void Atom::setCell(Cell* cell)
{
	cell_ = cell;
}

// Return cell in which the atom exists
Cell* Atom::cell() const
{
	return cell_;
}

/*
 * Connectivity
 */

// Add specified Bond to Atom
void Atom::addBond(Bond* bond)
{
	bonds_.append(bond);

	// Insert the pointer to the 'other' Atom into the exclusions_ list
	exclusions_.add(bond->partner(this));
}

// Return array of Bonds in which the Atom is involved
const PointerArray<Bond>& Atom::bonds() const
{
	return bonds_;
}

// Return Bond (if it exists) between this Atom and the Atom specified
Bond* Atom::findBond(Atom* j)
{
	for (int n=0; n<bonds_.nItems(); ++n) if (bonds_[n]->partner(this) == j) return bonds_[n];
	return NULL;
}

// Add specified Angle to Atom
void Atom::addAngle(Angle* angle)
{
	angles_.append(angle);

	// Insert the pointers to the other Atoms into the exclusions_ list
	if (angle->i() != this) exclusions_.add(angle->i());
	if (angle->j() != this) exclusions_.add(angle->j());
	if (angle->k() != this) exclusions_.add(angle->k());
}

// Return array of Angles in which the Atom is involved
const PointerArray<Angle>& Atom::angles() const
{
	return angles_;
}

// Add specified Torsion to Atom
void Atom::addTorsion(Torsion* torsion, double scaling14)
{
	torsions_.append(torsion);

	// Insert the pointers to the other Atoms into the exclusions_ list
	if (torsion->i() == this)
	{
		exclusions_.add(torsion->j());
		exclusions_.add(torsion->k());
		exclusions_.add(torsion->l(), scaling14);
	}
	else if (torsion->l() == this)
	{
		exclusions_.add(torsion->i(), scaling14);
		exclusions_.add(torsion->j());
		exclusions_.add(torsion->k());
	}
	else
	{
		exclusions_.add(torsion->i());
		exclusions_.add(torsion->l());
		if (torsion->j() != this) exclusions_.add(torsion->j());
		if (torsion->k() != this) exclusions_.add(torsion->k());
	}
}

// Return array of Torsions in which the Atom is involved
const PointerArray<Torsion>& Atom::torsions() const
{
	return torsions_;
}

// Return scaling factor to employ with specified Atom
double Atom::scaling(Atom* j) const
{
	// Look through our ordered list of excluded Atom interactions
	for (int n=0; n<exclusions_.nItems(); ++n)
	{
		// If the current item matches our Atom 'j', we have found a match
		if (exclusions_.pointer(n) == j) return exclusions_.data(n);

		// If the pointer of the item is greater than our test Atom 'j', we can exit the loop now since it is not in the list
		if (exclusions_.pointer(n) > j) return 1.0;
	}

	return 1.0;
}

/*
 * Coordinate Manipulation
 */

// Set coordinates
void Atom::setCoordinates(const Vec3<double>& newr)
{
	r_ = newr;
}

// Set coordinates
void Atom::setCoordinates(double dx, double dy, double dz)
{
	setCoordinates(Vec3<double>(dx,dy,dz));
}

// Translate coordinates
void Atom::translateCoordinates(const Vec3<double>& delta)
{
	setCoordinates(r_+delta);
}

// Translate coordinates
void Atom::translateCoordinates(double dx, double dy, double dz)
{
	setCoordinates(r_+Vec3<double>(dx,dy,dz));
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Atom::broadcast(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!procPool.broadcast(&element_, 1)) return false;
	if (!procPool.broadcast(r_)) return false;
	if (!procPool.broadcast(&charge_, 1)) return false;
	if (!procPool.broadcast(&localTypeIndex_, 1)) return false;
	if (!procPool.broadcast(&masterTypeIndex_, 1)) return false;
#endif
	return true;
}
