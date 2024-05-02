// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/distributor.h"
#include "base/messenger.h"
#include "base/processPool.h"
#include "classes/cell.h"

Distributor::Distributor(int nObjects, const CellArray &cellArray, ProcessPool &procPool,
                         ProcessPool::DivisionStrategy strategy, bool allowRepeats)
    : cellArray_(cellArray), processPool_(procPool), divisionStrategy_(strategy)
{
    // Cells
    cellLocks_.resize(cellArray_.nCells());
    std::fill(cellLocks_.begin(), cellLocks_.end(), 0);
    cellContentsModifiedBy_.resize(cellArray_.nCells());
    std::fill(cellContentsModifiedBy_.begin(), cellContentsModifiedBy_.end(), -1);

    // Distribution
    nObjects_ = nObjects;
    objectStatus_.resize(nObjects_);
    std::fill(objectStatus_.begin(), objectStatus_.end(), Distributor::WaitingFlag);
    nObjectsDistributed_ = 0;
    nUnavailableInstances_ = 0;
    nChangeBroadcastsRequired_ = 0;

    nProcessesOrGroups_ = processPool_.strategyNDivisions(divisionStrategy_);
    processOrGroupIndex_ = processPool_.strategyProcessIndex(divisionStrategy_);
    lastObjectDistributed_.resize(nProcessesOrGroups_);
    std::fill(lastObjectDistributed_.begin(), lastObjectDistributed_.end(), Distributor::NoneAvailable);
    lastHardLockedCells_ = new std::vector<const Cell *>[nProcessesOrGroups_];

    repeatsAllowed_ = allowRepeats;
}

Distributor::~Distributor() { delete[] lastHardLockedCells_; }

/*
 * Cells
 */

// Add soft lock to specified Cell index
bool Distributor::addSoftLock(int cellIndex)
{
    // Add a soft lock to the Cell - increase the current lock counter.
    // We can't soft-lock a cell if it is currently being modified ( == HardLocked )
    if (cellLocks_[cellIndex] == HardLocked)
    {
        Messenger::error("Can't add a soft lock to Cell index {} since it is being modified.\n", cellIndex);
        return false;
    }

    ++cellLocks_[cellIndex];

    return true;
}

// Remove soft lock from specified Cell index
bool Distributor::removeSoftLock(int cellIndex)
{
    // Remove a soft lock from the Cell - decrease the current lock counter
    // We can't soft-unlock a cell if it is currently being modified ( == HardLocked ) or if there are no soft locks left to
    // remove
    if (cellLocks_[cellIndex] == NoLocks)
    {
        Messenger::error("Can't remove a soft lock from Cell index {} since there are no soft locks to remove.\n", cellIndex);
        return false;
    }
    else if (cellLocks_[cellIndex] == HardLocked)
    {
        Messenger::error("Can't remove a soft lock from Cell index {} since it is being modified (there are no soft "
                         "locks to remove).\n",
                         cellIndex);
        return false;
    }

    --cellLocks_[cellIndex];

    return true;
}

// Add hard lock to specified Cell index, soft-locking surrounding Cells automatically
bool Distributor::addHardLocks(int cellIndex) { return addHardLocks(std::vector<const Cell *>({cellArray_.cell(cellIndex)})); }

// Remove hard lock from specified Cell index, soft-unlocking surrounding Cells automatically
bool Distributor::removeHardLocks(int cellIndex)
{
    return removeHardLocks(std::vector<const Cell *>({cellArray_.cell(cellIndex)}));
}

// Add hard locks to specified Cells, soft-locking surrounding Cells automatically
bool Distributor::addHardLocks(const std::vector<const Cell *> &cells)
{
    int cellId;

    // Form a list of unique surrounding cells that we need to soft-lock
    std::vector<const Cell *> softCells = surroundingCells(cells);
    Messenger::printVerbose("Hard-locking {} cells and soft-locking {} surrounding ones...\n", cells.size(), softCells.size());

    // Loop over Cells to hard-lock
    for (auto &cell : cells)
    {
        cellId = cell->index();

        // Add a hard lock to the Cell - set the lock counter to -1.
        // We can't hard-lock a cell if it is currently being modified ( == HardLocked ) or is soft-locked ( > 0 )
        if (cellLocks_[cellId] == HardLocked)
        {
            Messenger::error("Can't add a hard lock to Cell index {} since it is already being modified.\n", cellId);
            return false;
        }
        else if (cellLocks_[cellId] > NoLocks)
        {
            Messenger::error("Can't add a hard lock to Cell index {} since it already has soft locks ({}).\n", cellId,
                             cellLocks_[cellId]);
            return false;
        }

        cellLocks_[cellId] = HardLocked;
    }

    // Must now soft-lock all the surrounding Cells
    if (!std::all_of(softCells.begin(), softCells.end(), [&](const auto &cell) { return addSoftLock(cell->index()); }))
        return false;

    return true;
}

// Remove hard lock from specified Cells, soft-unlocking surrounding Cells automatically
bool Distributor::removeHardLocks(const std::vector<const Cell *> &cells)
{
    int cellId;

    // Form a list of unique surrounding cells that we need to soft-lock
    std::vector<const Cell *> softCells = surroundingCells(cells);
    Messenger::printVerbose("Hard-unlocking {} cells and soft-unlocking {} surrounding ones...\n", cells.size(),
                            softCells.size());

    // Loop over Cells to hard-unlock
    for (auto &cell : cells)
    {
        cellId = cell->index();

        // Remove a hard lock from the Cell - set lock counter to zero
        // We can't hard-unlock a cell unless it is currently hard-locked (obviously!)
        if (cellLocks_[cellId] == NoLocks)
        {
            Messenger::error("Can't remove a hard lock from Cell index {} since there is no hard lock to remove.\n", cellId);
            return false;
        }
        else if (cellLocks_[cellId] > NoLocks)
        {
            Messenger::error("Can't remove a hard lock from Cell index {} since there are only soft locks to remove.\n",
                             cellId);
            return false;
        }

        cellLocks_[cellId] = NoLocks;
    }

    // Must now soft-unlock all the surrounding Cells
    if (!std::all_of(softCells.begin(), softCells.end(), [&](const auto &cell) { return removeSoftLock(cell->index()); }))
        return false;

    return true;
}

// Return lock count
int Distributor::lockCount(int cellIndex) const { return cellLocks_.at(cellIndex); }

// Check hard lock possibility
bool Distributor::canHardLock(int cellIndex) const
{
    if (cellLocks_[cellIndex] != NoLocks)
        return false;

    auto *cell = cellArray_.cell(cellIndex);

    // For the specified Cell to be hard lockable its neighbours must not be HardLocked
    if (std::find_if(cellArray_.neighbours(*cell).begin(), cellArray_.neighbours(*cell).end(),
                     [&](const auto &nbr)
                     { return cellLocks_[nbr.neighbour_.index()] == HardLocked; }) != cellArray_.neighbours(*cell).end())
        return false;

    return true;
}

// Check hard lock possibility for list of Cells
bool Distributor::canHardLock(std::vector<const Cell *> cells) const
{
    return std::all_of(cells.begin(), cells.end(), [&](const auto &cell) { return canHardLock(cell->index()); });
}

// Return list of unique cells surrounding the supplied list of 'central' ones
std::vector<const Cell *> Distributor::surroundingCells(std::vector<const Cell *> centralCells)
{
    std::vector<const Cell *> surroundingCells;
    for (auto n = 0; n < centralCells.size(); ++n)
    {
        // Local Cell neighbours
        for (auto &nbr : cellArray_.neighbours(*centralCells[n]))
        {
            if (nbr.requiresMIM_)
                continue;

            // Check presence in central cells list
            if (std::find(centralCells.begin(), centralCells.end(), &nbr.neighbour_) != centralCells.end())
                continue;

            // Check presence in surrounding cells list
            if (std::find(surroundingCells.begin(), surroundingCells.end(), &nbr.neighbour_) != surroundingCells.end())
                continue;

            surroundingCells.push_back(&nbr.neighbour_);
        }

        // MIM Cell neighbours
        for (auto &nbr : cellArray_.neighbours(*centralCells[n]))
        {
            if (!nbr.requiresMIM_)
                continue;

            // Check presence in central cells list
            if (std::find(centralCells.begin(), centralCells.end(), &nbr.neighbour_) != centralCells.end())
                continue;

            // Check presence in surrounding cells list
            if (std::find(surroundingCells.begin(), surroundingCells.end(), &nbr.neighbour_) != surroundingCells.end())
                continue;

            surroundingCells.push_back(&nbr.neighbour_);
        }
    }

    return surroundingCells;
}

/*
 * Distribution
 */

// Return next available object for calculation
int Distributor::nextAvailableObject(bool &changesBroadcastRequired)
{
    /*
     * All processes should call here together. We will work out the index of the next object that should be the target for
     * each process or group of processes.
     */

    std::vector<const Cell *> hardLocksRequired, softLocksRequired;
    changesBroadcastRequired = false;

    // Initial check - if all objects have been distributed, we can return the AllComplete flag
    if (nObjectsDistributed_ >= nObjects_)
    {
        Messenger::printVerbose("All objects distributed.\n");
        std::fill(lastObjectDistributed_.begin(), lastObjectDistributed_.end(), Distributor::AllComplete);
        // 		lastHardLockedCells_ = Array<Cell*>();

        return Distributor::AllComplete;
    }

    // Loop over target groups / processes
    // Every process will determine the next object for every group / process, and return the relevant one at the end of the
    // routine
    for (auto processOrGroup = 0; processOrGroup < nProcessesOrGroups_; ++processOrGroup)
    {
        // Find the next object for this process / group, starting from the last one we found.
        // If there is no last object, work out a sensible starting point
        auto startIndex = lastObjectDistributed_[processOrGroup];
        if (startIndex == Distributor::NoneAvailable)
        {
            // TODO Better algorithm required here!
            startIndex = (nObjects_ / nProcessesOrGroups_) * processOrGroup - 1;
        }

        // Increment the index to step away from the last object sent
        ++startIndex;

        // If there is only one process/group, no need to establish lock status etc., just set object counter
        // Perform a straight loop from zero to nObjects_, adding it to the startIndex to get our new test index
        bool hardCellModified;
        int nextObject = Distributor::NoneAvailable;
        if (nProcessesOrGroups_ == 1)
        {
            nextObject = startIndex;
        }
        else
            for (auto n = 0; n < nObjects_; ++n)
            {
                // Get wrapped object index
                auto index = (startIndex + n) % nObjects_;

                // If this object is already distributed, move on
                if (objectStatus_[index] == Distributor::DistributedFlag)
                    continue;

                // If the object is waiting for distribution *OR* has already been completed, we are allowing
                // repeats, and we have already sent all objects, properly check its validity for distribution
                if ((objectStatus_[index] == Distributor::WaitingFlag) ||
                    ((nObjectsDistributed_ >= nObjects_) && repeatsAllowed_ &&
                     (objectStatus_[index] == Distributor::CompletedFlag)))
                {
                    // We now check the possibility for locking the Cells necessary for modifying the object

                    // First, get the Cells that we need to hard-lock if the object is to be modifeid
                    hardLocksRequired = cellsToBeModifiedForObject(index);

                    // If any of these Cells have been modified by a process other than this one, continue
                    // the search
                    // TODO Should we just broadcast changes instead of continuing the search?
                    hardCellModified = false;
                    for (auto *lock : hardLocksRequired)
                    {
                        if (cellContentsModifiedBy_[lock->index()] == -1)
                            continue;
                        if (cellContentsModifiedBy_[lock->index()] != processOrGroup)
                        {
                            hardCellModified = true;
                            break;
                        }
                    }
                    if (hardCellModified)
                        continue;

                    // If we can't hard-lock these Cells, move on
                    if (!canHardLock(hardLocksRequired))
                        continue;

                    // We can hard-lock the Cells, so this looks like a viable object to use as a target.
                    // Check now to see if any of the surrounding cells we need to soft-lock have the
                    // 'cellContentsModifiedBy_' values. If they have been set by someone else, there are
                    // changes to that Cell which we need, and so must broadcast any stored changes before
                    // the calculation proceeds Check the current value of changesBroadcastRequired first -
                    // if it is already 'true', there's no point making further checks
                    if (!changesBroadcastRequired)
                    {
                        softLocksRequired = surroundingCells(hardLocksRequired);
                        for (auto *lock : softLocksRequired)
                        {
                            // Get index of surrounding Cell
                            auto cellIndex = lock->index();

                            // If this Cell hasn't been modified, then great! Move on...
                            if (cellContentsModifiedBy_[cellIndex] == -1)
                                continue;

                            // If this Cell has been modified by the present process/group, that's
                            // OK, since it will have the new coordinates already
                            if (cellContentsModifiedBy_[cellIndex] == processOrGroup)
                                continue;

                            // It's been modified by someone else, so we will need to distribute
                            // changes before the calculation begins
                            changesBroadcastRequired = true;
                            ++nChangeBroadcastsRequired_;
                            Messenger::printVerbose("Changes broadcast required - Cell {} is required by "
                                                    "process/group {}, and process/group {} has modified it.\n",
                                                    cellIndex, processOrGroup, cellContentsModifiedBy_[cellIndex]);

                            // Reset the modified by flags here - this makes the assumption that the
                            // calling routine will honour the truth of 'changesBroadcastRequired'
                            std::fill(cellContentsModifiedBy_.begin(), cellContentsModifiedBy_.end(), -1);
                            break;
                        }
                    }

                    // All good - set the nextObject index
                    nextObject = index;
                }

                // If we have found a viable object, break out of the loop
                if (nextObject != Distributor::NoneAvailable)
                    break;
            }

        // Did we find a valid object?
        if (nextObject == Distributor::NoneAvailable)
        {
            Messenger::printVerbose("No viable objects left to distribute for group / process {}.\n", processOrGroup);

            lastObjectDistributed_[processOrGroup] = Distributor::NoneAvailable;

            // Make sure the hardLocksRequired array is empty
            hardLocksRequired.clear();

            ++nUnavailableInstances_;
        }
        else
        {
            // We have found a viable object for this process / group
            Messenger::printVerbose("Next object for process/group {} is {} (number already distributed = {}).\n",
                                    processOrGroup, nextObject, nObjectsDistributed_);
            lastObjectDistributed_[processOrGroup] = nextObject;
            lastHardLockedCells_[processOrGroup] = hardLocksRequired;

            // If the object hasn't already been distributed, increase our counter
            if (objectStatus_[nextObject] == Distributor::WaitingFlag)
                ++nObjectsDistributed_;

            // Lock the necessary Cells
            addHardLocks(hardLocksRequired);
        }
    }

    return lastObjectDistributed_[processOrGroupIndex_];
}

// Let the distributor know that the object is now finished with
bool Distributor::finishedWithObject()
{
    // Unlock all cells that we hard/soft-locked in the last distribution, and set flags on modified Cells
    // We assume that changes were made to all hard-locked cells by all processes / groups. This may not be the case, but to
    // find out the true picture we would need to perform further MPI broadcast of information each time, and that is what
    // we are trying to avoid!
    for (auto processOrGroup = 0; processOrGroup < nProcessesOrGroups_; ++processOrGroup)
    {
        auto objectIndex = lastObjectDistributed_[processOrGroup];

        // Mark object as completed
        if (objectIndex >= 0)
            objectStatus_[objectIndex] = Distributor::CompletedFlag;

        // Mark hard-locked Cells as being modified
        for (auto *cell : lastHardLockedCells_[processOrGroup])
        {
            auto cellIndex = cell->index();

            // If the current process/group was the last to modify it, that's OK.
            if (cellContentsModifiedBy_[cellIndex] == processOrGroup)
                continue;

            // Sanity check - has this cell been modified by somebody else?
            if (cellContentsModifiedBy_[cellIndex] != -1)
                Messenger::warn("Cell index {} contents modified twice ({} and {})?\n", cellIndex,
                                cellContentsModifiedBy_[cellIndex], processOrGroup);

            cellContentsModifiedBy_[cellIndex] = processOrGroup;
        }

        // Unlock the Cells
        removeHardLocks(lastHardLockedCells_[processOrGroup]);

        // Reset the array
        lastHardLockedCells_[processOrGroup].clear();
    }

    return true;
}

// Return number of instances where no viable object was available
int Distributor::nUnavailableInstances() const { return nUnavailableInstances_; }

// Return number of times a change broadcast was required
int Distributor::nChangeBroadcastsRequired() const { return nChangeBroadcastsRequired_; }
