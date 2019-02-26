/*
	*** Species Definition - Atomic Information
	*** src/classes/species_atomic.cpp
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

#include "classes/species.h"
#include "data/atomicmass.h"

// Add a new atom to the Species
SpeciesAtom* Species::addAtom(Element* element, Vec3<double> r)
{
	SpeciesAtom* i = atoms_.add();
	i->setParent(this);
	i->set(element, r.x, r.y, r.z);
	i->setIndex(atoms_.nItems()-1);

	++version_;

	return i;
}

// Return the number of Atoms in the Species
int Species::nAtoms() const
{
	return atoms_.nItems();
}

// Return the first Atom in the Species
SpeciesAtom* Species::firstAtom() const
{
	return atoms_.first();
}

// Return the nth Atom in the Species
SpeciesAtom* Species::atom(int n)
{
	return atoms_[n];
}

// Return the list of SpeciesAtoms
const List<SpeciesAtom>& Species::atoms() const
{
	return atoms_;
}

// Clear current Atom selection
void Species::clearAtomSelection()
{
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) i->setSelected(false);

	selectedAtoms_.clear();

	++atomSelectionVersion_;
}

// Add Atom to selection
void Species::selectAtom(SpeciesAtom* i)
{
	if (!i->isSelected())
	{
		i->setSelected(true);

		selectedAtoms_.add(i);

		++atomSelectionVersion_;
	}
}

// Remove atom from selection
void Species::deselectAtom(SpeciesAtom* i)
{
	if (i->isSelected())
	{
		i->setSelected(false);

		selectedAtoms_.remove(i);

		++atomSelectionVersion_;
	}
}

// Toggle selection state of specified atom
void Species::toggleAtomSelection(SpeciesAtom* i)
{
	if (i->isSelected()) deselectAtom(i);
	else selectAtom(i);
}

// Select Atoms along any path from the specified one, ignoring the bond(s) provided
void Species::selectFromAtom(SpeciesAtom* i, SpeciesBond* exclude, SpeciesBond* excludeToo)
{
	// Loop over Bonds on specified Atom
	selectAtom(i);
	SpeciesAtom* j;
	for (RefListItem<SpeciesBond,int>* refBond = i->bonds(); refBond != NULL; refBond = refBond->next)
	{
		// Is this either of the excluded bonds?
		if (exclude == refBond->item) continue;
		if (excludeToo == refBond->item) continue;

		// Get partner atom and continue selection
		j = refBond->item->partner(i);
		if (selectedAtoms_.contains(j)) continue;
		selectFromAtom(j, exclude);
	}
}

// Return current atom selection
const RefList<SpeciesAtom,bool>& Species::selectedAtoms() const
{
	return selectedAtoms_;
}

// Return nth selected Atom
SpeciesAtom* Species::selectedAtom(int n)
{
	RefListItem<SpeciesAtom,bool>* ri = selectedAtoms_[n];
	if (ri == NULL) return NULL;
	else return ri->item;
}

// Return number of selected Atoms
int Species::nSelectedAtoms() const
{
	return selectedAtoms_.nItems();
}

// Return whether specified Atom is selected
bool Species::isAtomSelected(SpeciesAtom* i) const
{
	return selectedAtoms_.contains(i);
}

// Return version of the atom selection
const int Species::atomSelectionVersion() const
{
	return atomSelectionVersion_;
}

// Return total atomic mass of Species
double Species::mass() const
{
	double m = 0.0;
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) m += AtomicMass::mass(i->element());
	return m;
}

// Update used AtomTypeList
void Species::updateUsedAtomTypes()
{
	usedAtomTypes_.clear();
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) usedAtomTypes_.add(i->atomType(), 1);
}

// Return used AtomTypesList
const AtomTypeList& Species::usedAtomTypes()
{
	return usedAtomTypes_;
}
