// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"
#include "classes/cellarray.h"
#include "templates/array.h"

// Distributor
class Distributor
{
    public:
    Distributor(int nObjects, const CellArray &cellArray, ProcessPool &procPool, ProcessPool::DivisionStrategy strategy,
                bool allowRepeats);
    virtual ~Distributor();
    // Object Status Flag
    enum ObjectStatusFlag
    {
        WaitingFlag,
        DistributedFlag,
        CompletedFlag
    };
    // Lock Flags
    enum LockFlag
    {
        HardLocked = -1,
        NoLocks = 0
    };
    // Special Indices
    enum SpecialIndices
    {
        AllComplete = -1,
        NoneAvailable = -2
    };

    /*
     * Cells
     */
    private:
    // Source CellArray
    const CellArray &cellArray_;
    // Cell lock counters
    Array<int> cellLocks_;
    // Cell contents modification indicators
    Array<int> cellContentsModifiedBy_;

    private:
    // Add soft lock to specified Cell index
    bool addSoftLock(int cellIndex);
    // Remove soft lock from specified Cell index
    bool removeSoftLock(int cellIndex);
    // Add hard lock to specified Cell index, soft-locking surrounding Cells automatically
    bool addHardLocks(int cellIndex);
    // Remove hard lock from specified Cell index, soft-unlocking surrounding Cells automatically
    bool removeHardLocks(int cellIndex);
    // Add hard locks to specified Cells, soft-locking surrounding Cells automatically
    bool addHardLocks(Array<Cell *> cells);
    // Remove hard lock from specified Cells, soft-unlocking surrounding Cells automatically
    bool removeHardLocks(Array<Cell *> cells);
    // Return lockCount on specified Cell index
    int lockCount(int cellIndex) const;
    // Check hard lock possibility for specified Cell index
    bool canHardLock(int cellIndex) const;
    // Check hard lock possibility for list of Cells
    bool canHardLock(Array<Cell *> cells) const;
    // Return list of unique cells surrounding the supplied list of 'central' ones
    Array<Cell *> surroundingCells(Array<Cell *> centralCells);
    // Return array of Cells that we must hard lock in order to modify the object with index specified
    virtual Array<Cell *> cellsToBeModifiedForObject(int objectId) = 0;

    /*
     * Distribution
     */
    private:
    // ProcessPool that we are using
    const ProcessPool &processPool_;
    // Division strategy to employ
    const ProcessPool::DivisionStrategy divisionStrategy_;
    // Number of processes / groups we are dealing with each time
    int nProcessesOrGroups_;
    // Our process / group index we have in
    int processOrGroupIndex_;
    // Whether repeated distribution of completed objects until all are completed is allowed
    bool repeatsAllowed_;
    // Number of objects in distribution
    int nObjects_;
    // Object status array
    Array<ObjectStatusFlag> objectStatus_;
    // Counter for distributed objects
    int nObjectsDistributed_;
    // Last objects distributed to process or group
    Array<int> lastObjectDistributed_;
    // Arrays of cells that were hard-locked in the last distribution (per process/group)
    Array<Cell *> *lastHardLockedCells_;
    // Number of instances where no viable object was available
    int nUnavailableInstances_;
    // Number of times a change broadcast was required
    int nChangeBroadcastsRequired_;

    public:
    // Return next available object for calculation
    int nextAvailableObject(bool &changesBroadcastRequired);
    // Let the distributor know that the object is now finished with
    bool finishedWithObject();
    // Return number of instances where no viable object was available
    int nUnavailableInstances() const;
    // Return number of times a change broadcast was required
    int nChangeBroadcastsRequired() const;
};
