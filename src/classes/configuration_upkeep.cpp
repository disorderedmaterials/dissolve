/*
    *** Configuration - Upkeep
    *** src/classes/configuration_upkeep.cpp
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

#include "classes/atom.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/changestore.h"
#include "main/dissolve.h"

// Update Cell contents
void Configuration::updateCellContents()
{
    // Fold the coordinates of each atom into the box, and then check its Cell location, moving if necessary.
    Atom **atoms = atoms_.array();
    for (int n = 0; n < atoms_.nItems(); ++n)
        updateCellLocation(atoms[n]);
}

// Update Cell location of specified Atom
void Configuration::updateCellLocation(Atom *i)
{
    // Fold Atom coordinates into Box
    i->setCoordinates(box_->fold(i->r()));

    // Determine new Cell position
    Cell *cell = cells_.cell(i->r());

    // Need to move?
    if (cell != i->cell())
    {
        if (i->cell())
            i->cell()->removeAtom(i);
        cell->addAtom(i);
    }
}

// Update Cell location of specified Molecule
void Configuration::updateCellLocation(std::shared_ptr<Molecule> mol)
{
    for (int n = 0; n < mol->nAtoms(); ++n)
        updateCellLocation(mol->atom(n));
}

// Update Cell location of specified Atom indices
void Configuration::updateCellLocation(const std::vector<int> &targetAtoms, int indexOffset)
{
    for (const auto i : targetAtoms)
        updateCellLocation(atoms_[i + indexOffset]);
}
