/*
	*** Cell Definition
	*** src/classes/cell.h
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
	int lockCount() const;
	// Clear locks
	void clearLocks();
	// Check lock possibility
	bool canLock() const;
	// Lock self and neighbouring Cells
	bool lock(bool willBeModified);
	// Unlock self and neighbouring Cells
	bool unlock(bool willBeModified);


	/*
	 * Contents
	 */
	private:
	// List of Atoms contained in this Cell
	OrderedPointerList<Atom> atoms_;

	public:
	// Return list of contained Atoms
	OrderedPointerList<Atom>& atoms();
	// Return number of Atoms in list
	int nAtoms() const;
	// Add atom to Cell
	bool addAtom(Atom* atom);
	// Remove Atom from Cell
	bool removeAtom(Atom* atom);


	/*
	 * Neighbours
	 */
	private:
	// Arrays of neighbouring cells, within the defined potential cutoff (from anywhere in the Cell)
	Cell** cellNeighbours_, **mimCellNeighbours_;
	// Array of all neighbouring cells
	CellNeighbour* allCellNeighbours_;
	// Number of cells in cell arrays
	int nCellNeighbours_, nMimCellNeighbours_;

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
};

#endif
