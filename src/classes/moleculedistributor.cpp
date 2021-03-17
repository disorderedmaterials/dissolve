// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/moleculedistributor.h"
#include "classes/atom.h"

MoleculeDistributor::MoleculeDistributor(const std::deque<std::shared_ptr<Molecule>> &moleculeArray, const CellArray &cellArray,
                                         ProcessPool &procPool, ProcessPool::DivisionStrategy strategy, bool repeatsAllowed)
    : Distributor(moleculeArray.size(), cellArray, procPool, strategy, repeatsAllowed), moleculeArray_(moleculeArray)
{
}

MoleculeDistributor::~MoleculeDistributor() {}

/*
 * Cells
 */

// Return array of Cells that we must hard lock in order to modify the object with index specified
Array<Cell *> MoleculeDistributor::cellsToBeModifiedForObject(int objectId)
{
    // Grab specified molecule
    std::shared_ptr<const Molecule> molecule = moleculeArray_[objectId];

    // Loop over Atoms in the Molecule, and add the (unique) cellID each Atom is in
    Array<Cell *> cells;
    int n;
    for (auto i = 0; i < molecule->nAtoms(); ++i)
    {
        Cell *cell = molecule->atom(i)->cell();

        // Is it already in the list?
        for (n = 0; n < cells.nItems(); ++n)
            if (cells.at(n) == cell)
                break;
        if (n == cells.nItems())
            cells.add(cell);
    }

    return cells;
}
