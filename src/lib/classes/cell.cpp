/*
	*** Cell Definition
	*** src/lib/classes/cell.cpp
	Copyright T. Youngs 2012-2013

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
#include "classes/box.h"

/*!
 * \brief Constructor
 * \details Constructor for Cell. 
 */
Cell::Cell()
{
	index_ = -1;
	lockCount_ = 0;
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
	Cell** neighbours = cellNeighbours_.objects();
	for (int n=0; n<cellNeighbours_.nItems(); ++n) if (neighbours[n]->lockCount() == -1) return false;
	Cell** mimNeighbours = mimCellNeighbours_.objects();
	for (int n=0; n<mimCellNeighbours_.nItems(); ++n) if (mimNeighbours[n]->lockCount() == -1) return false;
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
		Cell** neighbours = cellNeighbours_.objects();
		for (int n=0; n<cellNeighbours_.nItems(); ++n) neighbours[n]->addLock();
		Cell** mimNeighbours = mimCellNeighbours_.objects();
		for (int n=0; n<mimCellNeighbours_.nItems(); ++n) mimNeighbours[n]->addLock();
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
		Cell** neighbours = cellNeighbours_.objects();
		for (int n=0; n<cellNeighbours_.nItems(); ++n) neighbours[n]->removeLock();
		Cell** mimNeighbours = mimCellNeighbours_.objects();
		for (int n=0; n<mimCellNeighbours_.nItems(); ++n) mimNeighbours[n]->removeLock();
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
	// Need to first remove atom from all surrounding neighbour cells
	// TODO Speedup - Really lazy way of doing this...
// 	for (int n=0; n<cellNeighbours_.nItems(); ++n) cellNeighbours_.objects()[n]->removeAtomFromNeighbourList(i, false);
// 	for (int n=0; n<mimCellNeighbours_.nItems(); ++n) mimCellNeighbours_.objects()[n]->removeAtomFromNeighbourList(i, true);


	// Move atom from this cell to target cell
	atoms_.move(i->index(), targetCell->atoms_);
	i->setCell(targetCell);

	// Now must add atom to neighbouring atom lists of new cell
// 	for (int n=0; n<targetCell->cellNeighbours_.nItems(); ++n) targetCell->cellNeighbours_.objects()[n]->addAtomToNeighbourList(i, false);
// 	for (int n=0; n<targetCell->mimCellNeighbours_.nItems(); ++n) targetCell->mimCellNeighbours_.objects()[n]->addAtomToNeighbourList(i, true);
	// TODO This really needs to check whether the atom is in range or not...?? Is it worth it when using a small cell size?

	XXX Need a general list of all neighbour cells (regardless of near/mimd) on each cell
	XXX Then do comparison between this list on the current and targetCells
	XXX If 
	// Traverse the cell neighbour arrays from this cell and the target cell, comparing indices
	int indexA = 0, indexB = 0;
	Cell** oldNeighbours = cellNeighbours_.objects(), **newNeighbours = targetCell->cellNeighbours_.objects();
	int objectIndexA = oldNeighbours[indexA]->index(), objectIndexB = newNeighbours[indexB]->index();
	int nNeighbours = cellNeighbours_.nItems();
	while ((indexA < nItems_) && (indexB < targetCell->cellNeighbours_.nItems()))
	{
		// If objectAndexA is less than objectIndexB, then the item in this_ list at indexA is unique
		if (objectIndexA < objectIndexB)
		{
			uniqueToA.addAtEnd(oldNeighbours[indexA]);
			++indexA;
			objectIndexA = oldNeighbours[indexA]->objectIndex();
			continue;
		}

		// If indexB is less than indexA, then the item in listB at indexB is unique
		if (objectIndexB < objectIndexA)
		{
			uniqueToB.addAtEnd(newNeighbours[indexB]);
			++indexB;
			objectIndexB = newNeighbours[indexB]->objectIndex();
			continue;
		}

		// The indices are the same, so this is common element to both lists
		commonItems.addAtEnd(oldNeighbours[indexA]);
		++indexA;
		objectIndexA = oldNeighbours[indexA]->objectIndex();
		++indexB;
		objectIndexB = newNeighbours[indexB]->objectIndex();
	}

	// If we have not yet gone through all the items in either list, add them to the relevant unique results list
	for (int n = indexA; n<nItems_; ++n) uniqueToA.addAtEnd(oldNeighbours[n]);
	for (int n = indexB; n<listB.nItems_; ++n) uniqueToB.addAtEnd(newNeighbours[n]);
	
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
 * \brief Add Cell neighbour
 */
void Cell::addCellNeighbour(Cell* cell, bool mimRequired)
{
	if (mimRequired) mimCellNeighbours_.add(cell);
	else cellNeighbours_.add(cell);
}

/*!
 * \brief Return total number of cell neighbours
 */
int Cell::nTotalCellNeighbours()
{
	return cellNeighbours_.nItems() + mimCellNeighbours_.nItems();
}

/*!
 * \brief Return cell neighbour list
 */
OrderedList<Cell>& Cell::cellNeighbours()
{
	return cellNeighbours_;
}

/*!
 * \brief Return cell neighbour list requiring mim
 */
OrderedList<Cell>& Cell::mimCellNeighbours()
{
	return mimCellNeighbours_;
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
void Cell::removeAtomFromNeighbourList(Atom* i, bool useMim)
{
	if (useMim) mimAtomNeighbours_.removeIfPresent(i->index());
	else atomNeighbours_.removeIfPresent(i->index());
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
