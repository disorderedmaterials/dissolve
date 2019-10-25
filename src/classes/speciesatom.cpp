/*
	*** SpeciesAtom Definition
	*** src/classes/speciesatom.cpp
	Copyright T. Youngs 2012-2019

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
	grain_ = NULL;
	r_.zero();
	index_= -1;
	selected_ = false;
}

// Destructor
SpeciesAtom::~SpeciesAtom()
{
}

// Set Species parent
void SpeciesAtom::setParent(Species* sp)
{
	parent_ = sp;
}

// Return species parent
Species* SpeciesAtom::parent()
{
	return parent_;
}

// Set basic SpeciesAtom properties
void SpeciesAtom::set(Element* element, double rx, double ry, double rz)
{
	element_ = element;
	r_.set(rx, ry, rz);
}

// Set basic SpeciesAtom properties
void SpeciesAtom::set(Element* element, const Vec3<double> r)
{
	element_ = element;
	r_ = r;
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
 * Containing Grain
 */

// Set grain to which this atom belongs
void SpeciesAtom::setGrain(SpeciesGrain* grain)
{
	grain_ = grain;
}

// Return grain to which this atom belongs
SpeciesGrain* SpeciesAtom::grain()
{
	return grain_;
}

/*
 * Bond Information
 */

// Add Bond reference
void SpeciesAtom::addBond(SpeciesBond* b)
{
	bonds_.addUnique(b);
}

// Remove Bond reference
void SpeciesAtom::removeBond(SpeciesBond* b)
{
	bonds_.remove(b);
}

// Clear all Bond references
void SpeciesAtom::clearBonds()
{
	bonds_.clear();
}

// Return number of Bond references
int SpeciesAtom::nBonds() const
{
	return bonds_.nItems();
}

// Return specified bond
SpeciesBond* SpeciesAtom::bond(int index)
{
	return bonds_.item(index);
}

// Return bonds list
const RefList<SpeciesBond>& SpeciesAtom::bonds() const
{
	return bonds_;
}

// Return whether Bond to specified Atom exists
SpeciesBond* SpeciesAtom::hasBond(SpeciesAtom* j)
{
	for (RefListItem<SpeciesBond>* ri = bonds_.first(); ri != NULL; ri = ri->next()) if (ri->item()->partner(this) == j) return ri->item();
	return NULL;
}

/*
 * Coordinate Manipulation
 */

// Set coordinate
void SpeciesAtom::setCoordinate(int index, double value)
{
	r_.set(index, value);
}

// Set coordinates of atom
void SpeciesAtom::setCoordinates(const Vec3<double>& newr)
{
	r_ = newr;
}

// Translate coordinates of atom
void SpeciesAtom::translateCoordinates(const Vec3<double>& delta)
{
	r_ += delta;
}
