/*
	*** Cell Definition
	*** src/classes/cell.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_CELL_H
#define DISSOLVE_CELL_H

#include "classes/atom.h"
#include "templates/array.h"
#include "templates/vector3.h"
#include "templates/reflist.h"
#include "templates/ordereddaoarray.h"
#include "templates/orderedpointerarray.h"

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

	public:
	// Set grid reference
	void setGridReference(int x, int y, int z);
	// Return grid reference
	const Vec3<int>& gridReference() const;
	// Set unique index
	void setIndex(int id);
	// Return unique index
	int index() const;
	// Set real-space Cell centre
	void setCentre(Vec3<double> r);
	// Return real-space Cell centre
	const Vec3<double>& centre() const;


	/*
	 * Contents
	 */
	private:
	// Array of Atoms contained in this Cell
	OrderedPointerArray<Atom> atoms_;
	// Return array of contained Atoms, ordered by their array indices
	OrderedDAOArray<Atom> indexOrderedAtoms_;

	public:
	// Return array of contained Atoms
	OrderedPointerArray<Atom>& atoms();
	// Return array of contained Atoms, ordered by their array indices
	Atom** indexOrderedAtoms() const;
	// Return number of Atoms in array
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
	void addCellNeighbours(OrderedPointerArray<Cell>& nearNeighbours, OrderedPointerArray<Cell>& mimNeighbours);
	// Return number of Cell near-neighbours, not requiring minimum image calculation
	int nCellNeighbours() const;
	// Return number of Cell neighbours requiring minimum image calculation
	int nMimCellNeighbours() const;
	// Return total number of Cell neighbours
	int nTotalCellNeighbours() const;
	// Return adjacent Cell neighbour list
	Cell** cellNeighbours();
	// Return specified adjacent Cell neighbour
	Cell* cellNeighbour(int id) const;
	// Return list of Cell neighbours requiring minimum image calculation
	Cell** mimCellNeighbours();
	// Return specified Cell neighbour, requiring minimum image calculation
	Cell* mimCellNeighbour(int id) const;
	// Return list of all Cell neighbours
	CellNeighbour* allCellNeighbours();
	// Return array of adjacent Cell neighbours
	const Array<Cell*>& adjacentCellNeighbours();
};

#endif
