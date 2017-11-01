/*
	*** Cell Distributor
	*** src/classes/celldistributor.h
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

#ifndef DUQ_CELLDISTRIBUTOR_H
#define DUQ_CELLDISTRIBUTOR_H

#include "classes/cellarray.h"
#include "math/matrix3.h"
#include "templates/array.h"

// Forward Declarations
class Configuration;
class ProcessPool;

// Cell Distributor
class CellDistributor
{
	public:
	// Constructor
	CellDistributor(const CellArray& cellArray, bool cellContentsWillBeModified, bool repeatsAllowed);
	// Destructor
	~CellDistributor();
	// Status Flag
	enum StatusFlag { WaitingFlag, BeingModifiedFlag, CompletedFlag };
	// Special Indices
	enum SpecialIndices { AllComplete = -1, NoneAvailable = -2 };


	/*
	 * Data
	 */
	private:
	// Source CellArray
	const CellArray& cellArray_;
	// Whether distributed cells will have their contents modified
	bool cellContentsWillBeModified_;
	// Whether repeated distribution of completed cells until all are completed is allowed
	bool repeatsAllowed_;
	// Number of Cells in distribution
	int nCells_;
	// Cell lock counters
	Array<int> cellLocks_;
	// Cell status array
	Array<StatusFlag> cellStatus_;


	/*
	 * Locks
	 */
	private:
	// Add lock on specified Cell index
	bool addLock(int cellIndex);
	// Remove lock from specified Cell index
	bool removeLock(int cellIndex);
	// Return lockCount on specified Cell index
	int lockCount(int cellIndex) const;
	// Clear locks from specified Cell index
	void clearLocks(int cellIndex);
	// Check lock possibility for specified Cell index
	bool canLock(int cellIndex) const;
	// Lock self and neighbouring cells, so the Cell can be distributed for work
	bool lockForDistribution(int cellIndex);
	// Unlock self and neighbouring Cells, once the Cell has been returned
	bool unlockAfterDistribution(int cellIndex);


	/*
	 * Distribution
	 */
	public:
	// Counter for distributed Cells
	int nCellsDistributed_;
	// Last Cell distributed
	int lastCellDistributed_;

	public:
	// Initialise distributor for specified Configuration
	void initialise(Configuration* cfg);
	// Return next available Cell for calculation
	int nextAvailableCell(ProcessPool& procPool);
	// Unlock Cell specified, once calculation is complete
	bool finishedWithCell(ProcessPool& procPool, int cellId);
};

#endif
