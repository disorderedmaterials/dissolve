/*
	*** Species Definition - Atomic Information
	*** src/classes/species_atomic.cpp
	Copyright T. Youngs 2012-2018

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

// Add a new atom to the Species
SpeciesAtom* Species::addAtom(int element, double rx, double ry, double rz)
{
	SpeciesAtom* i = atoms_.add();
	i->setParent(this);
	i->set(element, rx, ry, rz);
	i->setIndex(atoms_.nItems()-1);
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
List<SpeciesAtom>& Species::atoms()
{
	return atoms_;
}

// Clear current Atom selection
void Species::clearAtomSelection()
{
	selectedAtoms_.clear();
}

// Add Atom to selection
void Species::selectAtom(SpeciesAtom* i)
{
	selectedAtoms_.addUnique(i);
}

// Select Atoms along any path from the specified one
void Species::selectFromAtom(SpeciesAtom* i, SpeciesBond* exclude)
{
	// Loop over Bonds on specified Atom
	selectAtom(i);
	SpeciesAtom* j;
	for (RefListItem<SpeciesBond,int>* refBond = i->bonds(); refBond != NULL; refBond = refBond->next)
	{
		// Is this the excluded Bond?
		if (exclude == refBond->item) continue;
		j = refBond->item->partner(i);
		if (selectedAtoms_.contains(j)) continue;
		selectFromAtom(j, exclude);
	}
}

// Return first selected Atom reference
RefListItem<SpeciesAtom,int>* Species::selectedAtoms() const
{
	return selectedAtoms_.first();
}

// Return nth selected Atom
SpeciesAtom* Species::selectedAtom(int n)
{
	RefListItem<SpeciesAtom,int>* ri = selectedAtoms_[n];
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

// Change element of specified Atom
void Species::changeAtomElement(SpeciesAtom* i, int el, AtomType* at)
{
	// Check for NULL pointer
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer passed to Species::changeAtomElement.\n");
		return;
	}
	
	// First, change element within Atom (if necessary)
	if (i->element() != el) i->setElement(el);
	else return;
	
	// Now, must assign an AtomType. Isotopologue data should be updated after this function!
	i->setAtomType(at);
}

// Return total atomic mass of Species
double Species::mass() const
{
	double m = 0.0;
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) m += PeriodicTable::element(i->element()).isotope(Isotope::NaturalIsotope)->atomicWeight();
	return m;
}
