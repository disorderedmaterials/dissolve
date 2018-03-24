/*
	*** dUQ Methods - Upkeep
	*** src/main/methods_upkeep.cpp
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

#include "main/duq.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/grain.h"
#include "classes/changestore.h"

// Update Cell contents
void Configuration::updateCellContents()
{
	// Fold the coordinates of each atom into the box, and then check its Cell location, moving if necessary.
	Atom** atoms = atoms_.array();
	for (int n = 0; n < atoms_.nItems(); ++n) updateCellLocation(atoms[n]);
}

// Update Cell location of specified Atom
void Configuration::updateCellLocation(Atom* i)
{
	// Fold Atom coordinates into Box
	i->setCoordinates(box_->fold(i->r()));

	// Determine new Cell position
	Cell* cell = cells_.cell(i->r());

	// Need to move?
	if (cell != i->cell())
	{
		if (i->cell()) i->cell()->removeAtom(i);
		cell->addAtom(i);
	}
}

// Update Cell location of specified Molecule
void Configuration::updateCellLocation(Molecule* mol)
{
	for (int n = 0; n < mol->nAtoms(); ++n) updateCellLocation(mol->atom(n));
}

// Update Cell location of specified Atoms (in array)
void Configuration::updateCellLocation(int nAtoms, Atom** atoms)
{
	for (int n = 0; n < nAtoms; ++n) updateCellLocation(atoms[n]);
}
