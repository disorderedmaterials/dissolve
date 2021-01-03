// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/distributor.h"
#include "classes/molecule.h"
#include <deque>
#include <memory>

// Forward Declarations
/* none */

// Molecule Distributor
class MoleculeDistributor : public Distributor
{
    public:
    MoleculeDistributor(const std::deque<std::shared_ptr<Molecule>> &moleculeArray, const CellArray &cellArray,
                        ProcessPool &procPool, ProcessPool::DivisionStrategy strategy, bool repeatsAllowed);
    ~MoleculeDistributor();

    /*
     * Data
     */
    private:
    // Source Molecule Array
    const std::deque<std::shared_ptr<Molecule>> &moleculeArray_;

    /*
     * Cells
     */
    private:
    // Return array of Cells that we must hard lock in order to modify the object with index specified
    Array<Cell *> cellsToBeModifiedForObject(int objectId);
};
