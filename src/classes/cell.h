/*
	*** Cell Definition
	*** src/classes/cell.h
	Copyright T. Youngs 2012-2016

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

#ifndef DUQ_CELL_H
#define DUQ_CELL_H

#include "classes/grain.h"
#include "templates/vector3.h"
#include "templates/reflist.h"
#include "templates/orderedpointerlist.h"

// Forward Declarations
class Box;
class CellNeighbour;

/*
 * Cell Definition
 */
class Cell
{
	public:
	// Constructor
	Cell();
	// Destructor
	~Cell();
	// Special Cell Indices
	enum SpecialCellIndices { AllCellsComplete = -1, NoCellsAvailable = -2 };


	/*
	 * Identity
	 */
	private:
	// Grid reference
	Vec3<int> gridReference_;
	// Unique index
	int index_;
	// Real-space coordinates at the centre of this cell
	Vec3<double> centre_;
	// Number of locks on this Cell (-1 indicates subject of calculation)
	int lockCount_;

	public:
	// Set grid reference
	void setGridReference(int x, int y, int z);
	// Return grid reference
	const Vec3<int>& gridReference() const;
	// Set unique index
	void setIndex(int id);
	// Return unique index
	int index() const;
	// Set real-space cell centre
	void setCentre(Vec3<double> r);
	// Return real-space cell centre
	const Vec3<double>& centre() const;
	// Add lock
	bool addLock();
	// Remove lock
	bool removeLock();
	// Return lockCount
	int lockCount();
	// Clear locks
	void clearLocks();
	// Check lock possibility
	bool canLock();
	// Lock self and neighbouring cells
	bool lock(bool willBeModified);
	// Unlock self and neighbouring cells
	bool unlock(bool willBeModified);


	/*
	 * Contents
	 */
	private:
	// List of atoms contained in this cell
	OrderedPointerList<Atom> atoms_;
	// List of Grains in Cell
	RefList<Grain,int> grains_;

	public:
	// Return list of contained atoms
	OrderedPointerList<Atom>& atoms();
	// Move specified atom to specified Cell
	bool moveAtom(Atom* i, Cell* targetCell);
	// Add atom to Cell
	bool addAtom(Atom* atom);
	// Add Grain to Cell
	void addGrain(Grain* grain);
	// Remove Grain from Cell
	void removeGrain(Grain* grain);
	// Return number of Grains in Cell
	int nGrains() const;
	// Return first Grain in list
	RefListItem<Grain,int>* grains() const;
	// Return nth Grain in list
	Grain* grain(int n);


	/*
	 * Neighbours
	 */
	private:
	// Arrays of neighbouring cells, within the defined potential cutoff (from anywhere in the cell)
	Cell** cellNeighbours_, **mimCellNeighbours_;
	// Array of all neighbouring cells
	CellNeighbour* allCellNeighbours_;
	// Number of cells in cell arrays
	int nCellNeighbours_, nMimCellNeighbours_;
	// Lists of neighbouring atoms, within the defined potential cutoff (from anywhere in the cell)
	OrderedPointerList<Atom> atomNeighbours_, mimAtomNeighbours_;

	public:
	// Add Cell neighbours
	void addCellNeighbours(OrderedPointerList<Cell>& neighbours, OrderedPointerList<Cell>& mimNeighbours, int allCells);
	// Return number of normal cell neighbours
	int nCellNeighbours();
	// Return number of mim cell neighbours
	int nMimCellNeighbours();
	// Return total number of cell neighbours
	int nTotalCellNeighbours();
	// Return cell neighbour list, not requiring mim
	Cell** cellNeighbours();
	// Return cell neighbour list requiring mim
	Cell** mimCellNeighbours();
	// Return list of all cell neighbours
	CellNeighbour* allCellNeighbours();
	// Clear atom neighbour list
	void clearAtomNeighbourList();
	// Add atom to neighbour list
	void addAtomToNeighbourList(Atom* i, bool useMim, bool atEnd = false);
	// Remove atom from neighbour list
	bool removeAtomFromNeighbourList(Atom* i, bool useMim);
	// Return total number of atom neighbours
	int nTotalAtomNeighbours();
	// Return atom neighbour list
	OrderedPointerList<Atom>& atomNeighbours();
	// Return atom neighbour list requiring mim
	OrderedPointerList<Atom>& mimAtomNeighbours();
};

#endif
