// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/celldistributor.h"
#include "classes/cell.h"

CellDistributor::CellDistributor(const CellArray &cellArray, ProcessPool &procPool, ProcessPool::DivisionStrategy strategy,
                                 bool repeatsAllowed)
    : Distributor(cellArray.nCells(), cellArray, procPool, strategy, repeatsAllowed), cells_(cellArray)
{
}

CellDistributor::~CellDistributor() = default;

/*
 * Cells
 */

// Return array of Cells that we must hard lock in order to modify the object with index specified
std::vector<Cell *> CellDistributor::cellsToBeModifiedForObject(int objectId)
{
    return std::vector<Cell *>({cells_.cell(objectId)});
}
