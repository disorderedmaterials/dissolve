/*
	*** Cell Definition
	*** src/lib/classes/cell.cpp
	Copyright T. Youngs 2012-2014

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

/*!
 * \brief Constructor
 * \details Constructor for Cell. 
 */
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

/*!
 * \brief Destructor
 * \details Constructor for Cell. 
 */
Cell::~Cell()
{
}

/*
// Identity
*/

/*!
 * \brief Set grid reference
 */
void Cell::setGridReference(int x, int y, int z)
{
	gridReference_.set(x,y,z);
}

/*!
 * \brief Return grid reference
 */
const Vec3<int>& Cell::gridReference() const
{
	return gridReference_;
}

/*!
 * \brief Set unique index
 * \details To prevent misuse, the identity of a Cell can be changed from its starting value of
 * -1 only once.
 */
void Cell::setIndex(int id)
{
	if (id == -1) msg.print("BAD_USAGE - Refused to set the ID of a Cell more than once.\n");
	else index_ = id;
}

/*!
 * \brief Return unique index
 */
int Cell::index() const
{
	return index_;
}

/*!
 * \brief Set real-space cell centre
 */
void Cell::setCentre(Vec3<double> r)
{
	centre_ = r;
}

/*! 
 * \brief Return real-space cell centre
 */
const Vec3< double >& Cell::centre() const
{
	return centre_;
}

/*!
 * \brief Add lock
 */
bool Cell::addLock()
{
	if (lockCount_ == -1)
	{
		msg.print("BAD_USAGE - Can't add a lock to a Cell which is being modified (lockCount_ = %i).\n", lockCount_);
		return false;
	}
	++lockCount_;
	return true;
}

/*!
 * \brief Remove lock
 */
bool Cell::removeLock()
{
	if (lockCount_ == -1)
	{
		msg.print("BAD_USAGE - Can't remove a lock from a Cell which is being modified (lockCount_ = %i).\n", lockCount_);
		return false;
	}
	if (lockCount_ == 0)
	{
		msg.print("BAD_USAGE - Can't remove a lock from a Cell which is not locked (lockCount_ = %i).\n", lockCount_);
		return false;
	}
	--lockCount_;
	return true;
}

/*!
 * \brief Return lock count
 */
int Cell::lockCount()
{
	return lockCount_;
}

/*!
 * \brief Clear locks
 */
void Cell::clearLocks()
{
	if (lockCount_ != 0) msg.print("Warning: Cleared existing locks (%i) from Cell id %i (%i,%i,%i).\n", lockCount_, index_, gridReference_.x, gridReference_.y, gridReference_.z);
	lockCount_ = 0;
}

/*!
 * \brief Check lock possibility
 * \details Check this Cell and the status of its neighbours to see if a lock is possible.
 */
bool Cell::canLock()
{
	if (lockCount_ != 0) return false;
	for (int n=0; n<nCellNeighbours_; ++n) if (cellNeighbours_[n]->lockCount() == -1) return false;
	for (int n=0; n<nMimCellNeighbours_; ++n) if (mimCellNeighbours_[n]->lockCount() == -1) return false;
	return true;
}

/*!
 * \brief Lock self and neighbouring cells
 */
bool Cell::lock(bool willBeModified)
{
	if (lockCount_ != 0)
	{
		msg.print("BAD_USAGE - Can't lock Cell %i (%i,%i,%i) for modification since lockCount_ != 0 (%i).\n", index_, gridReference_.x, gridReference_.y, gridReference_.z, lockCount_);
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

/*!
 * \brief Unlock self and neighbouring cells
 */
bool Cell::unlock(bool willBeModified)
{
	if (lockCount_ != -1)
	{
		msg.print("BAD_USAGE - Can't unlock Cell %i (%i,%i,%i) since lockCount_ != -1 (%i).\n", index_, gridReference_.x, gridReference_.y, gridReference_.z, lockCount_);
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
// Contents
*/

/*!
 * \brief Return Atom list
 */
OrderedList<Atom>& Cell::atoms()
{
	return atoms_;
}

/*!
 * \brief Add specified atom to this Cell
 * \details Add (copy) data from the supplied atom into a free space in the cell's atom array. The original atom is not 
 */
bool Cell::moveAtom(Atom* i, Cell* targetCell)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.print("NULL_POINTER - NULL Atom pointer given to Cell::moveAtom().\n");
		return false;
	}
	if (targetCell == NULL)
	{
		msg.print("NULL_POINTER - NULL Cell pointer given to Cell::moveAtom().\n");
		return false;
	}
#endif
	// Move atom from this cell to target cell
	atoms_.move(i->index(), targetCell->atoms_);
	i->setCell(targetCell);

	// Now need to update atom neighbour lists on cells affected by this move
	// Traverse the cell neighbour arrays from this cell and the target cell, comparing indices
	int n = 0, m = 0;
	CellNeighbour* oldNeighbours = allCellNeighbours_, *newNeighbours = targetCell->allCellNeighbours_;
	Cell* oldNeighbour = oldNeighbours[0].cell(), *newNeighbour = newNeighbours[0].cell();
	int oldIndex = oldNeighbour->index(), newIndex = newNeighbour->index();
	int nNeighbours = nTotalCellNeighbours();
	while (true)
	{
		// If the cell neighbour appears only in the current list (oldIndex < newIndex) then the atom should just be removed from this cell's atom list,
		// since it has moved out of range.
		if (oldIndex < newIndex)
		{
			oldNeighbour->removeAtomFromNeighbourList(i, oldNeighbours[n].useMim());
			++n;
			if (n == nNeighbours) break;
			oldNeighbour = oldNeighbours[n].cell();
			oldIndex = oldNeighbour->index();
			continue;
		}

		// If the cell neighbour appears only in the new list (newIndex < oldIndex) then the atom should just be added to this cell's atom list,
		// since is has moved within range
		if (newIndex < oldIndex)
		{
			newNeighbour->addAtomToNeighbourList(i, newNeighbours[m].useMim());
			++m;
			if (m == nNeighbours) break;
			newNeighbour = newNeighbours[m].cell();
			newIndex = newNeighbour->index();
			continue;
		}

		// If the cell reference appears in both lists, we must check the mim flag of the cell reference
		// If the atom has moved into a cell with the same mim state, then there is nothing to do. Otherwise, must move atom to correct list
		if (oldNeighbours[n].useMim() != newNeighbours[m].useMim())
		{
			oldNeighbour->removeAtomFromNeighbourList(i, oldNeighbours[n].useMim());
			oldNeighbour->addAtomToNeighbourList(i, !oldNeighbours[n].useMim());
		}

		++n;
		++m;
		if (n == nNeighbours) break;
		if (m == nNeighbours) break;
		oldNeighbour = oldNeighbours[n].cell();
		oldIndex = oldNeighbour->index();
		newNeighbour = newNeighbours[m].cell();
		newIndex = newNeighbour->index();
	}

	// If we have not yet gone through all the items in either list, add them to the relevant unique results list
	for (int o = n; o<nNeighbours; ++o) oldNeighbours[o].cell()->removeAtomFromNeighbourList(i, oldNeighbours[o].useMim());
	for (int o = m; o<nNeighbours; ++o) newNeighbours[o].cell()->addAtomToNeighbourList(i, newNeighbours[o].useMim());

	return true;
}

// Add atom to Cell
bool Cell::addAtom(Atom* atom)
{
#ifdef CHECKS
	if (atom == NULL)
	{
		msg.print("NULL_POINTER - NULL Atom pointer given to Cell::addAtom().\n");
		return false;
	}
#endif
	// Copy source atom data to target atom, and set new reference to atom
	atoms_.add(atom);
	atom->setCell(this);

	return true;
}

/*!
 * \brief Add Grain to Cell
 */
void Cell::addGrain(Grain* grain)
{
	// Has the Grain already had its Cell pointer set?
	if (grain->cell() == NULL) grain->setCell(this, grains_.nItems());
	else if (grain->cell() != this)
	{
		msg.print("BAD_USAGE - Grain should have been removed from its Cell before calling Cell::addGrain() with it.\n");
		return;
	}

	grains_.addUnique(grain);
}

/*!
 * \brief Remove Grain from Cell
 */
void Cell::removeGrain(Grain* grain)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer passed to Cell::removeGrain().\n");
		return;
	}
	if (!grains_.contains(grain))
	{
		msg.print("BAD_USAGE - Cell %i does not contain the supplied Grain (index %i).\n", index_, grain->index());
		return;
	}
#endif
	// Has the Grain already been removed from the cell?
	if (grain->cell() == this) grain->removeFromCell(this);
	else if (grain->cell() && (grain->cell() != this))
	{
		msg.print("BAD_USAGE - Trying to remove a Grain from a Cell which it doesn't think its in.\n");
		return;
	}
	grains_.remove(grain);
	
	// Now need to update indices of remaining Grains
	int index = 0;
	for (RefListItem<Grain,int>* ri = grains_.first(); ri != NULL; ri = ri->next) ri->item->setCell(this, index++);
}

/*!
 * \brief Return number of Grains in Cell
 */
int Cell::nGrains() const
{
	return grains_.nItems();
}

/*!
 * \brief Return first Grain in list
 */
RefListItem<Grain,int>* Cell::grains() const
{
	return grains_.first();
}

/*!
 * \brief Return nth Grain in list
 */
Grain* Cell::grain(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= grains_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Grain index (%i) passed to Cell::grain() is out of range (nGrains_ = %i).\n", n, grains_.nItems());
		return NULL;
	}
#endif
	return grains_[n]->item;
}

/*
// Neighbours
*/

/*!
 * \brief Add Cell neighbours
 */
void Cell::addCellNeighbours(OrderedList<Cell>& neighbours, OrderedList<Cell>& mimNeighbours, int allCells)
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
			msg.error("Cell neighbour lists are corrupt - same cell found in both near and mim lists.\n");
			return;
		}
	}
}

/*!
 * \brief Return number of adjacent cell neighbours
 */
int Cell::nCellNeighbours()
{
	return nCellNeighbours_;
}

/*!
 * \brief Return total number of cell neighbours
 */
int Cell::nMimCellNeighbours()
{
	return nMimCellNeighbours_;
}

/*!
 * \brief Return total number of cell neighbours
 */
int Cell::nTotalCellNeighbours()
{
	return nCellNeighbours_ + nMimCellNeighbours_;
}

/*!
 * \brief Return cell neighbour list
 */
Cell** Cell::cellNeighbours()
{
	return cellNeighbours_;
}

/*!
 * \brief Return cell neighbour list requiring mim
 */
Cell** Cell::mimCellNeighbours()
{
	return mimCellNeighbours_;
}

/*!
 * \brief Return list of all cell neighbours
 */
CellNeighbour* Cell::allCellNeighbours()
{
	return allCellNeighbours_;
}

/*!
 * \brief Clear atom neighbour list
 */
void Cell::clearAtomNeighbourList()
{
	atomNeighbours_.clear();
	mimAtomNeighbours_.clear();
}

/*!
 * \brief Add atom to neighbour list
 */
void Cell::addAtomToNeighbourList(Atom* i, bool useMim, bool atEnd)
{
	if (useMim)
	{
		if (atEnd) mimAtomNeighbours_.addAtEnd(i);
		else mimAtomNeighbours_.add(i);
	}
	else
	{
		if (atEnd) atomNeighbours_.addAtEnd(i);
		else atomNeighbours_.add(i);
	}
}

/*!
 * \brief Remove atom from neighbour list
 */
bool Cell::removeAtomFromNeighbourList(Atom* i, bool useMim)
{
	if (useMim) return mimAtomNeighbours_.removeIfPresent(i->index());
	else return atomNeighbours_.removeIfPresent(i->index());
}

/*!
 * \brief Return total number of atom neighbours
 */
int Cell::nTotalAtomNeighbours()
{
	return atomNeighbours_.nItems() + mimAtomNeighbours_.nItems();
}

/*!
 * \brief Return atom neighbour list
 */
OrderedList<Atom>& Cell::atomNeighbours()
{
	return atomNeighbours_;
}

/*!
 * \brief Return atom neighbour list requiring mim
 */
OrderedList<Atom>& Cell::mimAtomNeighbours()
{
	return mimAtomNeighbours_;
}
