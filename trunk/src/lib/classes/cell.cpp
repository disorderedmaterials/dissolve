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
	nextAvailableAtom_ = 0;
	atoms_ = NULL;
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
	for (RefListItem<Cell,bool>* ri = neighbours_.first(); ri != NULL; ri = ri->next) if (ri->item->lockCount() == -1) return false;
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
	if (willBeModified) for (RefListItem<Cell,bool>* ri = neighbours_.first(); ri != NULL; ri = ri->next) ri->item->addLock();
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
	if (willBeModified) for (RefListItem<Cell,bool>* ri = neighbours_.first(); ri != NULL; ri = ri->next) ri->item->removeLock();
	lockCount_ = 0;
	return true;
}

/*
// Contents
*/

/*!
 * \brief Create Atom array
 */
void Cell::createAtomArray(int maxAtoms)
{
#ifdef CHECKS
	if (maxAtoms <= 0)
	{
		msg.print("BAD_SIZE - Bad size for atom array (%i) given to Cell::createAtomArray().\n", maxAtoms);
		return;
	}
#endif
	maxAtoms_ = maxAtoms;
	atoms_ = new Atom[maxAtoms_];
	nextAvailableAtom_ = 0;
}

/*!
 * \brief Return Atom array
 */
Atom* Cell::atoms() const
{
	return atoms_;
}

/*!
 * \brief Return pointer to specified atom
 */
Atom* Cell::atom(int id)
{
#ifdef CHECKS
	static Atom dummy;
	if ((id < 0) || (id >= maxAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom id passed to Cell::atom() (%i) is out of range (maxAtoms_ = %i)\n", id, maxAtoms_);
		return &dummy;
	}
#endif
	return &atoms_[id];
}

/*!
 * \brief Return reference to specified atom
 */
Atom& Cell::atomReference(int id)
{
#ifdef CHECKS
	static Atom dummy;
	if ((id < 0) || (id >= maxAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom id passed to Cell::atomReference() (%i) is out of range (maxAtoms_ = %i)\n", id, maxAtoms_);
		return dummy;
	}
#endif
	return atoms_[id];
}

/*!
 * \brief Return maximum size of atom array
 */
int Cell::maxAtoms() const
{
	return maxAtoms_;
}

/*!
 * \brief Return whether specified atom index is used
 */
bool Cell::atomUsed(int id) const
{
#ifdef CHECKS
	if ((id < 0) || (id >= maxAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index given to Cell::atomUsed() (%i) is out of range (maxAtoms_ = %i)\n", id, maxAtoms_);
		return true;
	}
#endif
	return (atoms_[id].index() != Atom::UnusedAtom);
}

/*!
 * \brief Add specified atom to this Cell
 * \details Add (copy) data from the supplied atom into a free space in the cell's atom array. The original atom is not 
 */
bool Cell::moveAtom(int id, Cell* targetCell, Atom** atomReferences)
{
#ifdef CHECKS
	if ((id < 0) || (id >= maxAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index given to Cell::moveAtom() (%i) is out of range (maxAtoms_ = %i)\n", id, maxAtoms_);
		return false;
	}
	if (targetCell == NULL)
	{
		msg.print("NULL_POINTER - NULL Cell pointer given to Cell::moveAtom().\n");
		return false;
	}
#endif
	// Check target atom
	if (atoms_[id].index() == Atom::UnusedAtom)
	{
		msg.print("Error - Trying to move an atom which is already unused (index %i).\n", id);
		return false;
	}

	// Check available space in target cell
	int targetId = targetCell->nextUnusedAtom();
	if (targetId == Cell::NoAtomsAvailable)
	{
		msg.error("Can't move atom to target cell because no space is available. Increase the CellDensityMultiplier and restart the calculation.\n");
		return false;
	}

	// Copy source atom data to target atom, and set new reference to atom
	atomReferences[atoms_[id].index()] = &targetCell->atoms_[targetId];
	targetCell->atoms_[targetId].copyProperties(&atoms_[id]);
	targetCell->atoms_[targetId].setCell(targetCell);
	atoms_[id].setIndex(Atom::UnusedAtom);

	// Set available atom indices in source cell (since we know what it will be
	nextAvailableAtom_ = id;
	
	return true;
}

/*!
 * \brief Find and return next available (unused) atom index
 */
int Cell::nextUnusedAtom()
{
	// Check current atom position referenced by nextAvailableAtom_
	if ((nextAvailableAtom_ < 0) || (nextAvailableAtom_ >= maxAtoms_)) nextAvailableAtom_ = 0;
	if (atoms_[nextAvailableAtom_].index() == Atom::UnusedAtom) return nextAvailableAtom_;
	
	// Need to locate one...
	int n, newId = Cell::NoAtomsAvailable;
	// -- First half of loop - current position to end of array
	for (n=nextAvailableAtom_+1; n<maxAtoms_; ++n)
	{
		if (atoms_[n].index() == Atom::UnusedAtom)
		{
			newId = n;
			break;
		}
	}
	// -- Second half of loop - start of array up to current atom position (only if we haven't already found an empty slot)
	if (newId == Cell::NoAtomsAvailable) for (n=0; n<nextAvailableAtom_; ++n)
	{
		if (atoms_[n].index() == Atom::UnusedAtom)
		{
			newId = n;
			break;
		}
	}

	// Store and return the result
	nextAvailableAtom_ = newId;
	return nextAvailableAtom_;
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
void Cell::addNeighbour(Cell* cell, bool isImage)
{
	neighbours_.add(cell, isImage);
}

/*!
 * \brief Return number of neighbours in list
 */
int Cell::nNeighbours()
{
	return neighbours_.nItems();
}

/*!
 * \brief Return first neighbour in list
 */
RefList<Cell,bool>& Cell::neighbours()
{
	return neighbours_;
}
