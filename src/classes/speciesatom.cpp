/*
	*** SpeciesAtom Definition
	*** src/classes/speciesatom.cpp
	Copyright T. Youngs 2012-2020

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include "classes/speciesatom.h"
#include "classes/atomtype.h"
#include "classes/speciesbond.h"
#include "classes/species.h"
#include "data/elements.h"
#include "base/processpool.h"

// Constructor
SpeciesAtom::SpeciesAtom() : ListItem<SpeciesAtom>()
{
	element_ = NULL;
	charge_ = 0.0;
	atomType_ = NULL;
	r_.zero();
	index_= -1;
	selected_ = false;
}

// Destructor
SpeciesAtom::~SpeciesAtom()
{
}

// Set Species parent
void SpeciesAtom::setSpecies(Species* sp)
{
	parent_ = sp;
}

// Return Species parent
const Species* SpeciesAtom::species() const
{
	return parent_;
}

// Set basic SpeciesAtom properties
void SpeciesAtom::set(Element* element, double rx, double ry, double rz, double q)
{
	element_ = element;
	r_.set(rx, ry, rz);
	charge_ = q;
}

// Set basic SpeciesAtom properties
void SpeciesAtom::set(Element* element, const Vec3<double> r, double q)
{
	element_ = element;
	r_ = r;
	charge_ = q;
}

// Set atomic element
void SpeciesAtom::setElement(Element* el)
{
	element_ = el;
}

// Return atomic element
Element* SpeciesAtom::element() const
{
	return element_;
}

// Return coordinates
const Vec3<double>& SpeciesAtom::r() const
{
	return r_;
}

// Set charge of SpeciesAtom
void SpeciesAtom::setCharge(double charge)
{
	charge_ = charge;
}

// Return charge of SpeciesAtom
double SpeciesAtom::charge() const
{
	return charge_;
}

// Set AtomType of SpeciesAtom
void SpeciesAtom::setAtomType(AtomType* at)
{
	// Check elements
	if (at && (at->element() != element_))
	{
		Messenger::warn("Refused to assign AtomType '%s' to an atom of element %s, since the element of the AtomType is %s.\n", at->name(), element_->symbol(), at->element()->symbol());
		return;
	}

	atomType_ = at;
	if (parent_) parent_->bumpAtomTypesVersion();
}

// Return SpeciesAtomType of SpeciesAtom
AtomType* SpeciesAtom::atomType() const
{
	return atomType_;
}

// Set List index (0->[N-1])
void SpeciesAtom::setIndex(int id)
{
	index_ = id;
}

// Return List index (0->[N-1])
int SpeciesAtom::index() const
{
	return index_;
}

// Return 'user' index (1->N)
int SpeciesAtom::userIndex() const
{
	return index_+1;
}

// Set whether the atom is currently selected
void SpeciesAtom::setSelected(bool selected)
{
	selected_ = selected;
}

// Return whether the atom is currently selected
bool SpeciesAtom::isSelected() const
{
	return selected_;
}

/*
 * Bond Information
 */

// Add Bond reference
void SpeciesAtom::addBond(SpeciesBond *bond) {
  if (find(bonds_.begin(), bonds_.end(), bond) == bonds_.end()) {
    bonds_.push_back(bond);
  }
}

// Remove Bond reference
void SpeciesAtom::removeBond(SpeciesBond* b)
{
	bonds_.erase(find(bonds_.begin(), bonds_.end(), b));
}

// Clear all Bond references
void SpeciesAtom::clearBonds()
{
	bonds_.clear();
}

// Return number of Bond references
int SpeciesAtom::nBonds() const
{
	return bonds_.size();
}

// Return specified bond
SpeciesBond* SpeciesAtom::bond(int index)
{
	return bonds_.at(index);
}

// Return bonds list
const std::vector<SpeciesBond*>& SpeciesAtom::bonds() const
{
	return bonds_;
}

// Return whether Bond to specified Atom exists
SpeciesBond* SpeciesAtom::hasBond(SpeciesAtom* partner)
{
	auto result = find_if(bonds_.begin(), bonds_.end(),
			       [&](const SpeciesBond* bond){return bond->partner(this) == partner;});
	return result == bonds_.end() ? nullptr : *result;
}


// Add specified SpeciesAngle to Atom
void SpeciesAtom::addAngle(SpeciesAngle* angle)
{
	angles_.push_back(angle);

	// Insert the pointers to the other Atoms into the exclusions_ list
	if (angle->i() != this) exclusions_.add(angle->i());
	if (angle->j() != this) exclusions_.add(angle->j());
	if (angle->k() != this) exclusions_.add(angle->k());
}


// Remove angle reference
void SpeciesAtom::removeAngle(SpeciesAngle* angle)
{
	angles_.erase(find(angles_.begin(), angles_.end(), angle));
}

// Return the number of Angles in which the Atom is involved
int SpeciesAtom::nAngles() const
{
	return angles_.size();
}

// Return specified angle
SpeciesAngle* SpeciesAtom::angle(int index)
{
	return angles_.at(index);
}

// Return array of Angles in which the Atom is involved
const std::vector<SpeciesAngle*>& SpeciesAtom::angles() const
{
	return angles_;
}

// Add specified SpeciesTorsion to Atom
void SpeciesAtom::addTorsion(SpeciesTorsion* torsion, double scaling14)
{
	torsions_.push_back(torsion);

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

// Remove torsion reference
void SpeciesAtom::removeTorsion(SpeciesTorsion* torsion)
{
	torsions_.erase(find(torsions_.begin(), torsions_.end(), torsion));
}

// Return the number of Torsions in which the Atom is involved
int SpeciesAtom::nTorsions() const
{
	return torsions_.size();
}

// Return specified torsion
SpeciesTorsion* SpeciesAtom::torsion(int index)
{
	return torsions_.at(index);
}

// Return array of Torsions in which the Atom is involved
const std::vector<SpeciesTorsion*>& SpeciesAtom::torsions() const
{
	return torsions_;
}

// Return scaling factor to employ with specified Atom
double SpeciesAtom::scaling(const SpeciesAtom* j) const
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

// Set coordinate
void SpeciesAtom::setCoordinate(int index, double value)
{
	r_.set(index, value);
}

// Set coordinates
void SpeciesAtom::setCoordinates(double x, double y, double z)
{
	r_.x = x;
	r_.y = y;
	r_.z = z;
}

// Set coordinates (from Vec3)
void SpeciesAtom::setCoordinates(const Vec3<double>& newr)
{
	r_ = newr;
}

// Translate coordinates of atom
void SpeciesAtom::translateCoordinates(const Vec3<double>& delta)
{
	r_ += delta;
}
