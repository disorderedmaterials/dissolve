// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/distributor.h"

// Forward Declarations
/* none */

// Cell Distributor
class CellDistributor : public Distributor
{
    public:
    CellDistributor(const CellArray &cellArray, ProcessPool &procPool, ProcessPool::DivisionStrategy strategy,
                    bool repeatsAllowed);
    ~CellDistributor();

    /*
     * Data
     */
    private:
    // Source CellArray
    const CellArray &cells_;

    /*
     * Cells
     */
    private:
    // Return array of Cells that we must hard lock in order to modify the object with index specified
    Array<Cell *> cellsToBeModifiedForObject(int objectId);
};
