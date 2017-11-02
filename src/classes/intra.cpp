/*
	*** Intramolecular Base Definition
	*** src/classes/intra.cpp
	Copyright T. Youngs 2012-2017

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

// Create attached Atom array
void Intra::createAttachedAtomArray(int terminus, int size)
{
	// If the current array size is large enough to contain the new one, just empty it
	if (attached_[terminus] && (size <= arraySize_[terminus]))
	{
// 		nAttached_ = 
	}

	if (attached_[terminus] != NULL) delete[] attached_[terminus];
	attached_[terminus] = NULL;
	nAttached_[terminus] = size;

	if (nAttached_[terminus] != 0)
	{
		attached_[terminus] = new Atom*[nAttached_[terminus]];
		for (int n=0; n<nAttached_[terminus]; ++n) attached_[terminus][n] = NULL;
	}
}

// Set attached Atoms for terminus specified
void Intra::setAttachedAtoms(int terminus, const RefList<Atom,int>& atoms)
{
	createAttachedAtomArray(terminus, atoms.nItems());
	int index = 0;
	for (RefListItem<Atom,int>* refAtom = atoms.first(); refAtom != NULL; refAtom = refAtom->next) attached_[terminus][index++] = refAtom->item;
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
