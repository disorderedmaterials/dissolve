/*
	*** Species Definition - Atomic Information
	*** src/classes/species_atomic.cpp
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

#include "classes/species.h"
#include "data/atomicmass.h"

// Add a new atom to the Species
SpeciesAtom* Species::addAtom(Element* element, Vec3<double> r, double q)
{
	SpeciesAtom* i = atoms_.add();
	i->setSpecies(this);
	i->set(element, r.x, r.y, r.z, q);
	i->setIndex(atoms_.nItems()-1);

	++version_;

	return i;
}

// Remove the specified atom from the species
void Species::removeAtom(SpeciesAtom* i)
{
	/*
	 * Note: This is a deliberately simplistic function, and is intended only for use when creating / editing basic
	 * species definitions upon which the simulation has no dependencies.
	 */

	// Remove any bond terms that involve 'i'
	while (i->nBonds()) removeBond(i, i->bond(0)->partner(i));

	// Now remove the atom
	atoms_.remove(i);

	++version_;
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

// Set coordinates of specified atom
void Species::setAtomCoordinates(SpeciesAtom* i, Vec3<double> r)
{
	if (!i) return;

	i->setCoordinates(r);

	++version_;
}

// Set coordinates of specified atom (by index and individual coordinates)
void Species::setAtomCoordinates(int id, double x, double y, double z)
{
#ifdef CHECKS
	if ((id < 0) || (id >= atoms_.nItems()))
	{
		Messenger::error("Atom index %i is out of range - nAtoms = %i\n", id, atoms_.nItems());
		return;
	}
#endif

	atoms_[id]->setCoordinates(x, y, z);
}

// Transmute specified SpeciesAtom
void Species::transmuteAtom(SpeciesAtom* i, Element* el)
{
	if (!i) return;

	// Nothing to do if current element matches that supplied
	if (i->element() == el) return;

	// Remove any existing AtomType assignment
	i->setAtomType(NULL);
	i->setElement(el);

	++version_;
}

// Clear current Atom selection
void Species::clearAtomSelection()
{
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next()) i->setSelected(false);

	selectedAtoms_.clear();

	++atomSelectionVersion_;
}

// Add Atom to selection
void Species::selectAtom(SpeciesAtom* i)
{
	if (!i->isSelected())
	{
		i->setSelected(true);

		selectedAtoms_.append(i);

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
	for (const auto* bond : i->bonds())
	{
		// Is this either of the excluded bonds?
		if (exclude == bond) continue;
		if (excludeToo == bond) continue;

		// Get the partner atom in the bond and select it (if it is not selected already)
		auto* partner = bond->partner(i);

		if (selectedAtoms_.contains(partner)) continue;
		selectFromAtom(partner, exclude);
	}
}

// Return current atom selection
const RefList<SpeciesAtom>& Species::selectedAtoms() const
{
	return selectedAtoms_;
}

// Return nth selected Atom
SpeciesAtom* Species::selectedAtom(int n)
{
	RefListItem<SpeciesAtom>* ri = selectedAtoms_[n];
	if (ri == NULL) return NULL;
	else return ri->item();
}

// Return total charge of species from local atomic charges
double Species::totalChargeOnAtoms()
{
	double totalQ = 0.0;
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next()) totalQ += i->charge();
	return totalQ;
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
int Species::atomSelectionVersion() const
{
	return atomSelectionVersion_;
}

// Return total atomic mass of Species
double Species::mass() const
{
	double m = 0.0;
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next()) m += AtomicMass::mass(i->element());
	return m;
}

// Bump AtomTypes version
void Species::bumpAtomTypesVersion()
{
	++atomTypesVersion_;
}

// Return used AtomTypesList
const AtomTypeList& Species::usedAtomTypes()
{
	if (usedAtomTypesPoint_ != atomTypesVersion_)
	{
		usedAtomTypes_.clear();
		for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next()) if (i->atomType()) usedAtomTypes_.add(i->atomType(), 1);
	
		usedAtomTypesPoint_ = atomTypesVersion_;
	}

	return usedAtomTypes_;
}

// Clear AtomType assignments for all atoms
void Species::clearAtomTypes()
{
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next()) i->setAtomType(NULL);

	++atomTypesVersion_;
}
