/*
	*** Species Definition - Atomic Information
	*** src/lib/classes/species_atomic.cpp
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

#include "main/flags.h"
#include "classes/species.h"

/*!
 * \brief Add a new atom to the Species
 * \details Create a new Atom within the Species, of the specified atomic element and at the position specified.
 * The coordinates are used to determine bonding within the species and to provide initial starting
 * geometries for all occurrences of this Species within the Model. As such, they should be set so as to provide
 * a reasonable starting geometry for the species (i.e. a chemically reasonable one).
 */
SpeciesAtom *Species::addAtom(int element, double rx, double ry, double rz)
{
	SpeciesAtom* i = atoms_.add();
	i->setParent(this);
	i->set(element, rx, ry, rz);
	i->setIndex(atoms_.nItems()-1);
	return i;
}

/*!
 * \brief Return the number of Atoms in the Species
 * \details Returns the total number of Atoms defined in the Species.
 */
int Species::nAtoms() const
{
	return atoms_.nItems();
}

/*!
 * \brief Return the first Atom in the Species
 * \details Return the first Atom defined in the Species (if one exists, otherwise NULL will be returned)
 */
SpeciesAtom *Species::atoms() const
{
	return atoms_.first();
}

/*!
 * \brief Return the nth Atom in the Species
 * \details Return the nth Atom defined in the Species (or NULL if n is out of range)
 */
SpeciesAtom *Species::atom(int n)
{
	return atoms_[n];
}

/*!
 * \brief Clear current Atom selection
 * \details Remove all Atoms from the current selection.
 */
void Species::clearAtomSelection()
{
	selectedAtoms_.clear();
}

/*!
 * \brief Add Atom to selection
 * \details Add the specified Atom to the list of selected Atoms. Atom selections are used solely by
 * the GUI, in order to allow easy selection of Grains etc.
 */
void Species::selectAtom(SpeciesAtom* i)
{
	selectedAtoms_.addUnique(i);
}

/*!
 * \brief Select Atoms along any path from the specified one
 */
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

/*!
 * \brief Return first selected Atom reference
 */
RefListItem<SpeciesAtom,int>* Species::selectedAtoms() const
{
	return selectedAtoms_.first();
}

/*!
 * \brief Return nth selected Atom
 */
SpeciesAtom* Species::selectedAtom(int n)
{
	RefListItem<SpeciesAtom,int>* ri = selectedAtoms_[n];
	if (ri == NULL) return NULL;
	else return ri->item;
}

/*!
 * \brief Return number of selected Atoms
 */
int Species::nSelectedAtoms() const
{
	return selectedAtoms_.nItems();
}

/*!
 * \brief Return whether specified Atom is selected
 */
bool Species::isAtomSelected(SpeciesAtom* i) const
{
	return selectedAtoms_.contains(i);
}

/*!
 * \brief Change element of specified Atom
 * \details Transmute the specified Atom into the element specified, assigning the provided AtomType.
 * Note that it is necessary to update Isotopologue data after any calls to this function, through either
 * of the Species-specific or global (dUQ) updateIsotopologues() functions.
 */
void Species::changeAtomElement(SpeciesAtom* i, int el, AtomType* at)
{
	// Check for NULL pointer
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to Species::changeAtomElement.\n");
		return;
	}
	
	// First, change element within Atom (if necessary)
	if (i->element() != el) i->setElement(el);
	else return;
	
	// Now, must assign an AtomType. Isotopologue data should be updated after this function!
	i->setAtomType(at);

	Flags::wave(Flags::SpeciesChanged);
}

/*!
 * \brief Return total atomic mass of Species
 */
double Species::mass() const
{
	double m = 0.0;
	for (SpeciesAtom* i = atoms_.first(); i != NULL; i = i->next) m += PeriodicTable::element(i->element()).isotopes()->atomicWeight();
	return m;
}
