/*
	*** Molecule Distributor
	*** src/classes/moleculedistributor.cpp
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

#include "classes/moleculedistributor.h"
#include "classes/atom.h"

// Constructor
MoleculeDistributor::MoleculeDistributor(const DynamicArray<Molecule>& moleculeArray, const CellArray& cellArray, ProcessPool& procPool, ProcessPool::DivisionStrategy strategy, bool repeatsAllowed) : Distributor(moleculeArray.nItems(), cellArray, procPool, strategy, repeatsAllowed), moleculeArray_(moleculeArray)
{
}

// Destructor
MoleculeDistributor::~MoleculeDistributor()
{
}

/*
 * Cells
 */

// Return array of Cells that we must hard lock in order to modify the object with index specified
Array<Cell*> MoleculeDistributor::cellsToBeModifiedForObject(int objectId)
{
	// Grab specified molecule
	const Molecule* molecule = moleculeArray_.constValue(objectId);

	// Loop over Atoms in the Molecule, and add the (unique) cellID each Atom is in
	Array<Cell*> cells;
	int n;
	for (int i=0; i<molecule->nAtoms(); ++i)
	{
		Cell* cell = molecule->atom(i)->cell();

		// Is it already in the list?
		for (n=0; n<cells.nItems(); ++n) if (cells.constAt(n) == cell) break;
		if (n == cells.nItems()) cells.add(cell);
	}

	return cells;
}
