/*
	*** Cell Definition
	*** src/classes/cell.cpp
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

#include "classes/cell.h"
#include "classes/cellneighbour.h"
#include "classes/box.h"
#include "classes/atom.h"

// Constructor
Cell::Cell()
{
	index_ = -1;
	lockCount_ = 0;
	cellNeighbours_ = NULL;
	mimCellNeighbours_ = NULL;
	allCellNeighbours_ = NULL;
	nCellNeighbours_ = 0;
	nMimCellNeighbours_ = 0;
}

// Destructor
Cell::~Cell()
{
	if (cellNeighbours_) delete[] cellNeighbours_;
	if (mimCellNeighbours_) delete[] mimCellNeighbours_;
	if (allCellNeighbours_) delete[] allCellNeighbours_;
}

/*
 * Identity
 */

// Set grid reference
void Cell::setGridReference(int x, int y, int z)
{
	gridReference_.set(x,y,z);
}

// Return grid reference
const Vec3<int>& Cell::gridReference() const
{
	return gridReference_;
}

// Set unique index
void Cell::setIndex(int id)
{
	// To prevent misuse, the identity of a Cell can be changed from its starting value of -1 only once.
	if (id == -1) Messenger::print("BAD_USAGE - Refused to set the ID of a Cell more than once.\n");
	else index_ = id;
}

// Return unique index
int Cell::index() const
{
	return index_;
}

// Set real-space cell centre
void Cell::setCentre(Vec3<double> r)
{
	centre_ = r;
}

// Return real-space cell centre
const Vec3< double >& Cell::centre() const
{
	return centre_;
}

// Add lock
bool Cell::addLock()
{
	if (lockCount_ == -1)
	{
		Messenger::print("BAD_USAGE - Can't add a lock to a Cell which is being modified (lockCount_ = %i).\n", lockCount_);
		return false;
	}
	++lockCount_;
	return true;
}

// Remove lock
bool Cell::removeLock()
{
	if (lockCount_ == -1)
	{
		Messenger::print("BAD_USAGE - Can't remove a lock from a Cell which is being modified (lockCount_ = %i).\n", lockCount_);
		return false;
	}
	if (lockCount_ == 0)
	{
		Messenger::print("BAD_USAGE - Can't remove a lock from a Cell which is not locked (lockCount_ = %i).\n", lockCount_);
		return false;
	}
	--lockCount_;
	return true;
}

// Return lock count
int Cell::lockCount() const
{
	return lockCount_;
}

// Clear locks
void Cell::clearLocks()
{
	if (lockCount_ != 0) Messenger::print("Warning: Cleared existing locks (%i) from Cell id %i (%i,%i,%i).\n", lockCount_, index_, gridReference_.x, gridReference_.y, gridReference_.z);
	lockCount_ = 0;
}

// Check lock possibility
bool Cell::canLock() const
{
	if (lockCount_ != 0) return false;
	for (int n=0; n<nCellNeighbours_; ++n) if (cellNeighbours_[n]->lockCount() == -1) return false;
	for (int n=0; n<nMimCellNeighbours_; ++n) if (mimCellNeighbours_[n]->lockCount() == -1) return false;
	return true;
}

// Lock self and neighbouring cells
bool Cell::lock(bool willBeModified)
{
	if (lockCount_ != 0)
	{
		Messenger::print("BAD_USAGE - Can't lock Cell %i (%i,%i,%i) for modification since lockCount_ != 0 (%i).\n", index_, gridReference_.x, gridReference_.y, gridReference_.z, lockCount_);
		return false;
	}

	// Lock surrounding Cells if we are modifying the central one
	if (willBeModified)
	{
		for (int n=0; n<nCellNeighbours_; ++n) cellNeighbours_[n]->addLock();
		for (int n=0; n<nMimCellNeighbours_; ++n) mimCellNeighbours_[n]->addLock();
	}
	lockCount_ = -1;

	return true;
}

// Unlock self and neighbouring cells
bool Cell::unlock(bool willBeModified)
{
	if (lockCount_ != -1)
	{
		Messenger::print("BAD_USAGE - Can't unlock Cell %i (%i,%i,%i) since lockCount_ != -1 (%i).\n", index_, gridReference_.x, gridReference_.y, gridReference_.z, lockCount_);
		return false;
	}

	// Unlock surrounding cells if we were modifying the central one
	if (willBeModified)
	{
		for (int n=0; n<nCellNeighbours_; ++n) cellNeighbours_[n]->removeLock();
		for (int n=0; n<nMimCellNeighbours_; ++n) mimCellNeighbours_[n]->removeLock();
	}
	lockCount_ = 0;

	return true;
}

/*
 * Contents
 */

// Return Atom list
OrderedPointerList<Atom>& Cell::atoms()
{
	return atoms_;
}

// Return number of Atoms in list
int Cell::nAtoms() const
{
	return atoms_.nItems();
}

// Add atom to Cell
bool Cell::addAtom(Atom* i)
{
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::print("NULL_POINTER - NULL Atom pointer given to Cell::addAtom().\n");
		return false;
	}
#endif
	// Add Atom to our OrderedPointerList 
	atoms_.add(i);

	if (i->cell()) Messenger::warn("About to set Cell pointer in Atom %i, but this will overwrite an existing value.\n", i->arrayIndex());
	i->setCell(this);

	return true;
}

// Remove Atom from Cell
bool Cell::removeAtom(Atom* i)
{
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::print("NULL_POINTER - NULL Atom pointer given to Cell::removeAtom().\n");
		return false;
	}
#endif
	if (!atoms_.removeIfPresent(i)) Messenger::warn("Tried to remove Atom %i from Cell %i, but it was not present.\n", i->arrayIndex(), index_);

	// Add Atom to our OrderedPointerList
	atoms_.add(i);

	// Set its Cell pointer
	if (i->cell() != this) Messenger::warn("About to remove Cell pointer from Atom %i, and it does not match the current Cell.\n", i->arrayIndex());
	i->setCell(NULL);

	return true;
}

/*
 * Neighbours
 */

// Add Cell neighbours
void Cell::addCellNeighbours(OrderedPointerList<Cell>& neighbours, OrderedPointerList<Cell>& mimNeighbours, int allCells)
{
	int n, m, count, indexN, indexM;

	// Create normal (non-mim) neighbour array
	nCellNeighbours_ = neighbours.nItems();
	cellNeighbours_ = new Cell*[nCellNeighbours_];
	for (n=0; n<nCellNeighbours_; ++n) cellNeighbours_[n] = neighbours.objects()[n];

	// Create mim'd neighbour array
	nMimCellNeighbours_ = mimNeighbours.nItems();
	mimCellNeighbours_ = new Cell*[nMimCellNeighbours_];
	for (n=0; n<nMimCellNeighbours_; ++n) mimCellNeighbours_[n] = mimNeighbours.objects()[n];

	// Create ordered list of CellNeighbours (including cells from both lists)
	allCellNeighbours_ = new CellNeighbour[nCellNeighbours_+nMimCellNeighbours_];
	n = 0;
	m = 0;
	indexN = (nCellNeighbours_ > 0 ? cellNeighbours_[0]->index() : allCells);
	indexM = (nMimCellNeighbours_ > 0 ? mimCellNeighbours_[0]->index() : allCells);
	count = 0;
	while (count < nCellNeighbours_+nMimCellNeighbours_)
	{
		if (indexN < indexM)
		{
			allCellNeighbours_[count++].set(cellNeighbours_[n++], false);
			indexN = (n == nCellNeighbours_ ? allCells : cellNeighbours_[n]->index());
			continue;
		}
		if (indexM < indexN)
		{
			allCellNeighbours_[count++].set(mimCellNeighbours_[m++], true);
			indexM = (m == nMimCellNeighbours_ ? allCells : mimCellNeighbours_[m]->index());
			continue;
		}
		if (indexN == indexM)
		{
			Messenger::error("Cell neighbour lists are corrupt - same cell found in both near and mim lists.\n");
			return;
		}
	}
}

// Return number of adjacent cell neighbours
int Cell::nCellNeighbours()
{
	return nCellNeighbours_;
}

// Return total number of cell neighbours
int Cell::nMimCellNeighbours()
{
	return nMimCellNeighbours_;
}

// Return total number of cell neighbours
int Cell::nTotalCellNeighbours()
{
	return nCellNeighbours_ + nMimCellNeighbours_;
}

// Return cell neighbour list
Cell** Cell::cellNeighbours()
{
	return cellNeighbours_;
}

// Return cell neighbour list requiring mim
Cell** Cell::mimCellNeighbours()
{
	return mimCellNeighbours_;
}

// Return list of all cell neighbours
CellNeighbour* Cell::allCellNeighbours()
{
	return allCellNeighbours_;
}
