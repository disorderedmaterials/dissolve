// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/cellDistributor.h"
#include "classes/cell.h"

CellDistributor::CellDistributor(const CellArray &cellArray, bool repeatsAllowed)
    : Distributor(cellArray.nCells(), cellArray, repeatsAllowed), cells_(cellArray)
{
}

CellDistributor::~CellDistributor() = default;

/*
 * Cells
 */

// Return array of Cells that we must hard lock in order to modify the object with index specified
std::vector<const Cell *> CellDistributor::cellsToBeModifiedForObject(int objectId)
{
    return std::vector<const Cell *>({cells_.cell(objectId)});
}
