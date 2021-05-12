// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/moleculedistributor.h"
#include "classes/atom.h"

MoleculeDistributor::MoleculeDistributor(const std::deque<std::shared_ptr<Molecule>> &moleculeArray, const CellArray &cellArray,
                                         ProcessPool &procPool, ProcessPool::DivisionStrategy strategy, bool repeatsAllowed)
    : Distributor(moleculeArray.size(), cellArray, procPool, strategy, repeatsAllowed), moleculeArray_(moleculeArray)
{
}

MoleculeDistributor::~MoleculeDistributor() = default;

/*
 * Cells
 */

// Return array of Cells that we must hard lock in order to modify the object with index specified
std::vector<Cell *> MoleculeDistributor::cellsToBeModifiedForObject(int objectId)
{
    // Grab specified molecule
    std::shared_ptr<const Molecule> molecule = moleculeArray_[objectId];

    // Loop over Atoms in the Molecule, and add the (unique) cellID each Atom is in
    std::vector<Cell *> cells;
    int n;
    for (auto i = 0; i < molecule->nAtoms(); ++i)
    {
        auto *cell = molecule->atom(i)->cell();

        // Is it already in the list?
        if (std::find(cells.begin(), cells.end(), cell) == cells.end())
            cells.push_back(cell);
    }

    return cells;
}
