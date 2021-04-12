// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/celldistributor.h"
#include "classes/cell.h"

CellDistributor::CellDistributor(const CellArray &cellArray, ProcessPool &procPool, ProcessPool::DivisionStrategy strategy,
                                 bool repeatsAllowed)
    : Distributor(cellArray.nCells(), cellArray, procPool, strategy, repeatsAllowed), cells_(cellArray)
{
}

CellDistributor::~CellDistributor() {}

/*
 * Cells
 */

// Return array of Cells that we must hard lock in order to modify the object with index specified
Array<Cell *> CellDistributor::cellsToBeModifiedForObject(int objectId)
{
    Array<Cell *> cells;

    cells.add(cells_.cell(objectId));

    return cells;
}
