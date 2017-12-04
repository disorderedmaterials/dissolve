/*
	*** Cell Distributor
	*** src/classes/celldistributor.cpp
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/celldistributor.h"
#include "classes/configuration.h"
#include "classes/cell.h"
// #include "classes/grain.h"
#include "base/processpool.h"

// Constructor
CellDistributor::CellDistributor(const CellArray& cellArray, bool cellContentsWillBeModified, bool repeatsAllowed) : cellArray_(cellArray), cellContentsWillBeModified_(cellContentsWillBeModified), repeatsAllowed_(repeatsAllowed)
{
	// Initialise flag array
	nCells_ = cellArray_.nCells();
	cellLocks_.initialise(nCells_);
	cellStatus_.initialise(nCells_);

	for (int n=0; n<nCells_; ++n)
	{
		cellLocks_[n] = 0;
		cellStatus_[n] = CellDistributor::WaitingFlag;
	}

	nCellsDistributed_ = 0;
	lastCellDistributed_ = -1;
}

// Destructor
CellDistributor::~CellDistributor()
{
}

/*
 * Locks
 */

// Add lock
bool CellDistributor::addLock(int cellIndex)
{
	if (cellStatus_[cellIndex] == CellDistributor::BeingModifiedFlag)
	{
		Messenger::print("BAD_USAGE - Can't add a lock to a Cell index which is being modified (cellLocks_[cellIndex] = %i).\n", cellLocks_[cellIndex]);
		return false;
	}

	++cellLocks_[cellIndex];

	return true;
}

// Remove lock
bool CellDistributor::removeLock(int cellIndex)
{
	if (cellStatus_[cellIndex] == CellDistributor::BeingModifiedFlag)
	{
		Messenger::print("BAD_USAGE - Can't remove a lock from a Cell index which is being modified (cellLocks_[cellIndex] = %i).\n", cellLocks_[cellIndex]);
		return false;
	}
	if (cellLocks_[cellIndex] == 0)
	{
		Messenger::print("BAD_USAGE - Can't remove a lock from a Cell index which is not locked (cellLocks_[cellIndex] = %i).\n", cellLocks_[cellIndex]);
		return false;
	}

	--cellLocks_[cellIndex];

	return true;
}

// Return lock count
int CellDistributor::lockCount(int cellIndex) const
{
	return cellLocks_.value(cellIndex);
}

// Clear locks
void CellDistributor::clearLocks(int cellIndex)
{
	if (cellLocks_[cellIndex] != 0)
	{
		Vec3<int> gridRef = cellArray_.cell(cellIndex)->gridReference();
		Messenger::warn("Cleared existing locks (%i) from Cell id %i (%i,%i,%i).\n", cellLocks_[cellIndex], cellIndex, gridRef.x, gridRef.y, gridRef.z);
	}
	cellLocks_[cellIndex] = 0;
}

// Check lock possibility
bool CellDistributor::canLock(int cellIndex) const
{
	if (cellLocks_.value(cellIndex) != 0) return false;

	Cell* cell = cellArray_.cell(cellIndex);
	int id;

	// Check lock status of local Cell neighbours
	for (int n=0; n<cell->nCellNeighbours(); ++n)
	{
		id = cell->cellNeighbour(n)->index();
		if (cellLocks_.value(id) == -1) return false;
	}

	// Check lock status of minimum image Cell neighbours
	for (int n=0; n<cell->nMimCellNeighbours(); ++n)
	{
		id = cell->mimCellNeighbour(n)->index();
		if (cellLocks_.value(id) == -1) return false;
	}

	return true;
}

// Lock self and neighbouring cells, so the Cell can be distributed for work
bool CellDistributor::lockForDistribution(int cellIndex)
{
	if (cellLocks_.value(cellIndex) != 0)
	{
		Vec3<int> gridRef = cellArray_.cell(cellIndex)->gridReference();
		Messenger::print("BAD_USAGE - Can't lock Cell %i (%i,%i,%i) for modification since cellLocks_[cellIndex] != 0 (%i).\n", cellIndex, gridRef.x, gridRef.y, gridRef.z, cellLocks_[cellIndex]);
		return false;
	}

	addLock(cellIndex);

	// Lock surrounding Cells if we are modifying the central one, and set the correct flag
	if (cellContentsWillBeModified_)
	{
		cellStatus_[cellIndex] = CellDistributor::BeingModifiedFlag;

		Cell* cell = cellArray_.cell(cellIndex);

		// Check lock status of local Cell neighbours
		for (int n=0; n<cell->nCellNeighbours(); ++n) addLock(cell->cellNeighbour(n)->index());

		// Check lock status of minimum image Cell neighbours
		for (int n=0; n<cell->nMimCellNeighbours(); ++n) addLock(cell->mimCellNeighbour(n)->index());
	}

	return true;
}

// Unlock self and neighbouring Cells, once the Cell has been returned
bool CellDistributor::unlockAfterDistribution(int cellIndex)
{
	// Unlock surrounding Cells if we were modifying the central one, and reset its flag
	if (cellContentsWillBeModified_)
	{
		cellStatus_[cellIndex] = CellDistributor::WaitingFlag;

		Cell* cell = cellArray_.cell(cellIndex);

		// Check lock status of local Cell neighbours
		for (int n=0; n<cell->nCellNeighbours(); ++n) removeLock(cell->cellNeighbour(n)->index());

		// Check lock status of minimum image Cell neighbours
		for (int n=0; n<cell->nMimCellNeighbours(); ++n) removeLock(cell->mimCellNeighbour(n)->index());
	}

	removeLock(cellIndex);

	return true;
}

/*
 * Distribution
 */

// Return next available Cell for calculation
int CellDistributor::nextAvailableCell(ProcessPool& procPool)
{
	/*
	 * All processes should call here together. The local master process will distribute the index of the next available Cell to each process.
	 * If all Cells have been distributed once then Cell::AllCellsComplete is returned to all processes. Otherwise, the next available 'unused' Cell index
	 * is returned. If the routine runs out of available cells during a distribution cycle, then what is returned is determined by the value of
	 * 'allowRepeats'. If 'allowRepeats' is true then Cells which have already been sent once will be sent again, in order for all processes to be
	 * doing useful work at all times. If 'allowRepeats' is false, then repeat calculations on Cells are not allowed, and NoneAvailable is returned
	 * instead. The latter is relevant when property calculation is being performed, and each Cell must be subjected to calculation once and once only.
	 * 
	 * Any routine which employs Cell distribution in this way implicitly operates in parallel with process groups.
	 */
#ifdef PARALLEL
	int proc, group,m, cellId, rootCellId = -1;
	int* procList;
	if (procPool.isMaster())
	{
		Messenger::printVerbose("NextAvailableCell: nCellsDistributed_ = %i\n", nCellsDistributed_);
		// All cells already distributed?
		if (nCellsDistributed_ == nCells_)
		{
			for (int n=1; n<procPool.nProcesses(); ++n) if (!procPool.send(CellDistributor::AllComplete, n)) Messenger::error("MPI error in CellDistributor::nextAvailableCell() when all Cells were distributed.\n");
			return CellDistributor::AllComplete;
		}
		
		// There are still Cells which haven't been worked on yet, so find and distribute them.
		for (group = 0; group<procPool.nProcessGroups(); ++group)
		{
			Messenger::printVerbose("Finding next Cell for process group %i\n", group);

			// Loop from 1 to nCells_
			for (m=1; m<=nCells_; ++m)
			{
				// Get (wrapped) Cell index and check availability
				cellId = (lastCellDistributed_+m)%nCells_;
				if ((cellLocks_[cellId] != 0) || (cellContentsWillBeModified_ && (!canLock(cellId))))
				{
					// Reset cellId and continue
					cellId = CellDistributor::NoneAvailable;
					continue;
				}

				// If we have not distributed all cells...
				if (nCellsDistributed_ < nCells_)
				{
					// ... then we want one we've not yet used, and which isn't currently locked by another process
					if (cellStatus_[cellId]) continue;
					
					Messenger::printVerbose("Cell id %i will be sent to process group %i.\n", cellId, group);
					++nCellsDistributed_;
					lastCellDistributed_ = cellId;
					break;
				}
				else if (repeatsAllowed_)
				{
					// ... otherwise if 'allowRepeats' we're allowed to send an unlocked one which has already been used
					Messenger::printVerbose("Cell id %i will be sent to process group %i (although it has already been used).\n", cellId, group);
					break;
				}
				
				// Reset id if we get here...
				cellId = CellDistributor::NoneAvailable;
			}
			
			// Did we find a suitable Cell?
			if (cellId == CellDistributor::NoneAvailable)
			{
				Messenger::printVerbose("Warning: Couldn't find valid Cell to send to process group %i. Probably an odd number of Cells and an even number of process groups.\n", group);
			}
			else
			{
				// Lock the Cell
				lockForDistribution(cellId);
			}
			
			// Broadcast Cell index to all processes in current ProcessGroup
			for (proc=0; proc<procPool.nProcessesInGroup(group); ++proc)
			{
				int poolRank = procPool.processGroup(group)->poolRank(proc);
				// If this world rank is our own, no need to send it - just store it for later return below
				if (poolRank == procPool.poolRank()) rootCellId = cellId;
				else if (!procPool.send(cellId, poolRank)) printf("MPI error in CellDistributor::nextAvailableCell() sending next cell to pool rank %i.\n", poolRank);
			}
		}

		// All done - return our own cellId
		return rootCellId;
	}
	else
	{
		// Slaves just receive the next Cell index
		if (!procPool.receive(cellId, 0)) printf("MPI error in CellDistributor::nextAvailableCell() when all cells were distributed.\n");
		Messenger::printVerbose("Slave with rank %i (world rank %i) received Cell id %i\n", procPool.poolRank(), procPool.worldRank(), cellId);
		return cellId;
	}
#else
	// If we have distributed all Cells already, return -1
	if (nCellsDistributed_ == nCells_) return CellDistributor::AllComplete;

	++lastCellDistributed_;
	++nCellsDistributed_;
	cells_[lastCellDistributed_].lock(willBeModified);
	cellStatus_[lastCellDistributed_] = true;

	return lastCellDistributed_;
#endif
}

// Unlock Cell specified, once calculation is complete
bool CellDistributor::finishedWithCell(ProcessPool& procPool, int cellId)
{
	/*
	 * All processes should call this routine once they have finished performing manipulations of the Cell they were given from
	 * CellDistributor::nextAvailableCell(), passing the cellId back to this function.
	 */
#ifdef PARALLEL
	if (procPool.isMaster())
	{
		// Receive all used cellId's from process group leaders
		for (int group=0; group<procPool.nProcessGroups(); ++group)
		{
			// If this is *not* the masters group, receive data from the slave process leader
			if (group != procPool.groupIndex())
			{
				if (!procPool.receive(cellId, procPool.processGroup(group)->poolRank(0))) Messenger::print("MPI error in CellDistributor::finishedWithCell() receiving from process with rank %i.\n", group);
			}
			if (cellId >= 0) unlockAfterDistribution(cellId);
		}
	}
	else
	{
		// Slaves just send their id to the master, provided they are process group leader
		if (procPool.groupLeader())
		{
			if (!procPool.send(cellId, 0)) Messenger::print("MPI error in CellDistributor::finishedWithCell().\n");
		}
	}
	
	// Collect all processes in the pool together
	if (!procPool.wait(ProcessPool::Pool)) return false;
#else
	if (cellId >= 0) unlockAfterDistribution(cellId);
#endif
	return true;
}
