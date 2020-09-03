/*
    *** Regional Distributor
    *** src/classes/regionaldistributor.h
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

#pragma once

#include "base/processpool.h"
#include "classes/cellarray.h"
#include "templates/array.h"
#include "templates/dynamicarray.h"
#include "templates/orderedpointerlist.h"
#include <deque>
#include <memory>
#include <vector>

// Forward Declarations
class ProcessPool;
class Molecule;

// Regional Distributor
class RegionalDistributor
{
    public:
    RegionalDistributor(const std::deque<std::shared_ptr<Molecule>> &moleculeArray, const CellArray &cellArray,
                        ProcessPool &procPool, ProcessPool::DivisionStrategy strategy);
    ~RegionalDistributor();
    // Molecule Status Flag
    enum MoleculeStatusFlag
    {
        WaitingFlag,
        DistributedFlag
    };
    // Return string for specified MoleculeStatusFlag
    std::string_view moleculeStatusFlag(MoleculeStatusFlag flag);
    // Cell Status Flag
    enum CellStatusFlag
    {
        UnusedFlag,
        LockedForEditingFlag,
        ReadByOneFlag,
        ReadByManyFlag
    };
    // Return string for specified CellStatusFlag
    std::string_view cellStatusFlag(CellStatusFlag flag);

    /*
     * Core
     */
    private:
    // ProcessPool that we are using
    const ProcessPool &processPool_;
    // Original parallel strategy to employ
    const ProcessPool::DivisionStrategy originalStrategy_;
    // Current parallel strategy in effect
    ProcessPool::DivisionStrategy currentStrategy_;
    // Number of processes / groups we are dealing with each time
    int nProcessesOrGroups_;
    // Our process / group index (assigned differently to each process / group of processes)
    int processOrGroupIndex_;
    // Number of cycles the distributor has been run for
    int nCycles_;

    private:
    // Set process/group limits based on supplied strategy
    bool setProcessOrGroupLimits(ProcessPool::DivisionStrategy strategy);

    public:
    // Set up next distribution of Molecules amongst processes/groups, returning false if there are no more Molecules to
    // distribute
    bool cycle();
    // Return current parallel strategy, based on last cycle
    ProcessPool::DivisionStrategy currentStrategy();

    /*
     * Cell Data
     */
    private:
    // Source CellArray
    const CellArray &cellArray_;
    // Lists of Cells locked by each process/group
    OrderedPointerList<Cell> *lockedCells_;
    // Cell process/group owners
    Array<int> cellLockOwners_;
    // Cell status flags
    Array<CellStatusFlag> cellStatusFlags_;

    private:
    // Return whether the specified processOrGroup can lock the given Cell index
    bool canLockCellForEditing(int processOrGroup, int cellIndex);

    /*
     * Molecule Data
     */
    private:
    // Source Molecule Array
    const std::deque<std::shared_ptr<Molecule>> &moleculeArray_;
    // Number of Molecules to distribute
    int nMoleculesToDistribute_;
    // Counter for distributed Molecules
    int nMoleculesDistributed_;
    // Molecule status array
    Array<MoleculeStatusFlag> moleculeStatus_;
    // Arrays of Molecule IDs assigned to each process / group
    std::vector<std::vector<int>> assignedMolecules_;

    private:
    // Assign Molecule to process/group if possible
    bool assignMolecule(std::shared_ptr<const Molecule> mol, int processOrGroup);
    // Try to assign a Molecule from the specified Cell to the process/group
    std::shared_ptr<Molecule> assignMolecule(Cell *cell, int processOrGroup);
    // Try to find a Molecule target for the process/group
    std::shared_ptr<Molecule> assignMolecule(int processOrGroup);

    public:
    // Return next set of Molecule IDs assigned to this process
    std::vector<int> &assignedMolecules();

    /*
     * Helper Functions
     */
    public:
    // Return whether to collect statistics (counts, energies etc. - ensures correct summation)
    bool collectStatistics();
    // Increment integer counter, but only if the process should (depend on the current strategy)
    void increment(int &counter);
    // Add to double variable, but only if the process should (depend on the current strategy)
    void increase(double &var, double value);
};
