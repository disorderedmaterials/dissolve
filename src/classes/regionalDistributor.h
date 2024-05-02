// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/processPool.h"
#include "classes/cellArray.h"
#include <deque>
#include <memory>
#include <set>
#include <vector>

// Forward Declarations
class Molecule;

// Regional Distributor
class RegionalDistributor
{
    public:
    RegionalDistributor(const int nMolecules, const CellArray &cellArray, const ProcessPool &procPool,
                        ProcessPool::DivisionStrategy strategy);
    ~RegionalDistributor() = default;
    // Molecule Status Flag
    enum class MoleculeStatusFlag
    {
        ToDo,
        Assigned,
        Completed
    };
    // Return string for specified MoleculeStatusFlag
    std::string_view moleculeStatusFlag(MoleculeStatusFlag flag);
    // Cell Status Flag
    enum class CellStatusFlag
    {
        Unused,
        LockedForEditing,
        ReadByOne,
        ReadByMany
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
    std::vector<std::set<Cell *>> lockedCells_;
    // Cell process/group owners
    std::vector<int> cellLockOwners_;
    // Cell status flags
    std::vector<CellStatusFlag> cellStatusFlags_;

    private:
    // Return whether the specified processOrGroup can lock the given Cell index
    bool canLockCellForEditing(int processOrGroup, int cellIndex);

    /*
     * Molecule Data
     */
    private:
    // Total number of molecules in the system
    int nMolecules_;
    // Number of Molecules to distribute
    int nMoleculesToDistribute_;
    // Counter for distributed Molecules
    int nMoleculesDistributed_;
    // Molecule status array
    std::vector<MoleculeStatusFlag> moleculeStatus_;
    // Arrays of Molecule IDs assigned to each process / group
    std::vector<std::vector<int>> assignedMolecules_;

    private:
    // Assign Molecule to process/group if possible
    bool assignMolecule(const std::shared_ptr<const Molecule> &mol, int processOrGroup);
    // Try to assign a Molecule from the specified Cell to the process/group
    std::shared_ptr<Molecule> assignMolecule(const Cell *cell, int processOrGroup);
    // Try to find a Molecule target for the process/group
    std::shared_ptr<Molecule> assignMolecule(int processOrGroup);

    public:
    // Set target molecules for the distributor
    void setTargetMolecules(const std::vector<int> &targetMoleculeIndices);
    // Return next set of Molecule IDs assigned to this process
    std::vector<int> &assignedMolecules();

    /*
     * Helper Functions
     */
    public:
    // Return whether to collect statistics (counts, energies etc. - ensures correct summation)
    bool collectStatistics();
    // Increment integer counter, but only if the process should (depends on the current strategy)
    void increment(int &counter);
    // Add to double variable, but only if the process should (depends on the current strategy)
    void increase(double &var, double value);
};
