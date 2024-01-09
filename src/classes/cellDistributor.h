// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/distributor.h"

// Cell Distributor
class CellDistributor : public Distributor
{
    public:
    CellDistributor(const CellArray &cellArray, ProcessPool &procPool, ProcessPool::DivisionStrategy strategy,
                    bool repeatsAllowed);
    ~CellDistributor() override;

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
    std::vector<const Cell *> cellsToBeModifiedForObject(int objectId) override;
};
