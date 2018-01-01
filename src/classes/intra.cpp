/*
	*** Intramolecular Base Definition
	*** src/classes/intra.cpp
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

#include "classes/intra.h"
#include "classes/atom.h"

// Constructor
Intra::Intra()
{
	molecule_ = NULL;
	nAttached_[0] = 0;
	nAttached_[1] = 0;
	attached_[0] = NULL;
	attached_[1] = NULL;
	arraySize_[0] = 0;
	arraySize_[1] = 0;
}

// Destructor
Intra::~Intra()
{
}

/*
 * Basic Data
 */

// Set Molecule that this Intra exists in
void Intra::setMolecule(Molecule* parent)
{
	molecule_ = parent;
}

// Return Molecule that this Intra exists in
Molecule* Intra::molecule() const
{
	return molecule_;
}

/*
 * Connections
 */

// Clear and delete all arrays
void Intra::deleteAttachedAtomArrays()
{
	for (int n=0; n<2; ++n)
	{
		if (attached_[n] != NULL) delete[] attached_[n];
		attached_[n] = NULL;
		nAttached_[n] = 0;
		arraySize_[n] = 0;
	}
}

// Set attached Atoms for the terminus specified
void Intra::setAttachedAtoms(int terminus, const RefList<Atom,bool>& atoms)
{
	// Is the current array non-existent or too small to hold the new list?
	if ((!attached_[terminus]) || (atoms.nItems() > arraySize_[terminus]))
	{
		// Delete existing array if it is there
		if (attached_[terminus]) delete[] attached_[terminus];

		// Create new array just big enough to hold the number of Atoms in the list
		arraySize_[terminus] = atoms.nItems();
		attached_[terminus] = new Atom*[arraySize_[terminus]];
	}

	// Zero the current count of items in the array
	nAttached_[terminus] = 0;

	// Add the Atoms in the list
	for (RefListItem<Atom,bool>* refAtom = atoms.first(); refAtom != NULL; refAtom = refAtom->next) attached_[terminus][nAttached_[terminus]++] = refAtom->item;
}

// Set attached Atoms for terminus specified (single Atom)
void Intra::setAttachedAtoms(int terminus, Atom* atom)
{
	RefList<Atom,bool> atoms;
	atoms.add(atom);
	setAttachedAtoms(terminus, atoms);
}

// Return number of attached Atoms for terminus specified
int Intra::nAttached(int terminus) const
{
	return nAttached_[terminus];
}

// Return array of attached Atoms for terminus specified
Atom** Intra::attached(int terminus) const
{
	return attached_[terminus];
}
