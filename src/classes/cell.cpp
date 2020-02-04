/*
	*** Cell Definition
	*** src/classes/cell.cpp
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

#include "classes/cell.h"
#include "classes/cellneighbour.h"
#include "classes/box.h"
#include "classes/atom.h"

// Constructor
Cell::Cell()
{
	index_ = -1;
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

// Set real-space Cell centre
void Cell::setCentre(Vec3<double> r)
{
	centre_ = r;
}

// Return real-space Cell centre
const Vec3< double >& Cell::centre() const
{
	return centre_;
}

/*
 * Contents
 */

// Return array of contained Atoms
OrderedPointerArray<Atom>& Cell::atoms()
{
	return atoms_;
}

// Return array of contained Atoms, ordered by their array indices
Atom** Cell::indexOrderedAtoms() const
{
	return indexOrderedAtoms_.constItems();
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
	// Add Atom to our pointer- and index-ordered arrays
	atoms_.add(i);
	indexOrderedAtoms_.add(i);

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
	// Remove atom from this cell
	if (atoms_.remove(i))
	{
		indexOrderedAtoms_.remove(i);
		i->setCell(NULL);
	}
	else
	{
		Messenger::error("Tried to remove Atom %i from Cell %i, but it was not present.\n", i->arrayIndex(), index_);
		return false;
	}

	return true;
}

/*
 * Neighbours
 */

// Add Cell neighbours
void Cell::addCellNeighbours(OrderedPointerArray<Cell>& nearNeighbours, OrderedPointerArray<Cell>& mimNeighbours)
{
	int n;

	// Create near-neighbour array of Cells not requiring minimum image to be applied
	nCellNeighbours_ = nearNeighbours.nItems();
	cellNeighbours_ = new Cell*[nCellNeighbours_];
	for (n=0; n<nCellNeighbours_; ++n) cellNeighbours_[n] = nearNeighbours[n];

	// Create array of neighbours that require minimum image calculation
	nMimCellNeighbours_ = mimNeighbours.nItems();
	mimCellNeighbours_ = new Cell*[nMimCellNeighbours_];
	for (n=0; n<nMimCellNeighbours_; ++n) mimCellNeighbours_[n] = mimNeighbours[n];

	// Create ordered list of CellNeighbours (including cells from both lists)
	OrderedPointerDataArray<Cell,bool> allCells;
	for (n=0; n<nearNeighbours.nItems(); ++n) allCells.add(nearNeighbours[n], false);
	for (n=0; n<mimNeighbours.nItems(); ++n) allCells.add(mimNeighbours[n], true);

	if (allCells.nItems() != (nCellNeighbours_+nMimCellNeighbours_)) Messenger::error("Cell neighbour lists are corrupt - same cell found in both near and mim lists.\n");
	allCellNeighbours_ = new CellNeighbour[allCells.nItems()];
	for (n=0; n<allCells.nItems(); ++n) allCellNeighbours_[n].set(allCells.pointer(n), allCells.data(n));
}

// Return number of Cell near-neighbours, not requiring minimum image calculation
int Cell::nCellNeighbours() const
{
	return nCellNeighbours_;
}

// Return total number of Cell neighbours requiring minimum image calculation
int Cell::nMimCellNeighbours() const
{
	return nMimCellNeighbours_;
}

// Return total number of Cell neighbours
int Cell::nTotalCellNeighbours() const
{
	return nCellNeighbours_ + nMimCellNeighbours_;
}

// Return adjacent Cell neighbour list
Cell** Cell::cellNeighbours()
{
	return cellNeighbours_;
}

// Return specified adjacent Cell neighbour
Cell* Cell::cellNeighbour(int id) const
{
	return cellNeighbours_[id];
}

// Return list of Cell neighbours requiring minimum image calculation
Cell** Cell::mimCellNeighbours()
{
	return mimCellNeighbours_;
}

// Return specified Cell neighbour, requiring minimum image calculation
Cell* Cell::mimCellNeighbour(int id) const
{
	return mimCellNeighbours_[id];
}

// Return list of all Cell neighbours
CellNeighbour* Cell::allCellNeighbours()
{
	return allCellNeighbours_;
}
