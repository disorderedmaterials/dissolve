/*
	*** Cell Definition
	*** src/lib/classes/cell.h
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

#ifndef DUQ_CELL_H
#define DUQ_CELL_H

#include "classes/grain.h"
#include "templates/vector3.h"
#include "templates/orderedlist.h"
#include "templates/reflist.h"

// Forward Declarations
class Box;

/*!
 * \brief Cell Definition
 * \details XXX TODO
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
	// Special Atom Reference Indices
	enum SpecialAtomIndices { NoAtomsAvailable = -2 };


	/*!
	 * \name Identity
	 */
	///@{
	private:
	// Grid reference
	Vec3<int> gridReference_;
	// Unique index
	int index_;
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
	///@}


	/*!
	 * \name Contents
	 */
	///@{
	private:
	// List of atoms contained in this cell
	OrderedList<Atom> atoms_;
	// List of Grains in Cell
	RefList<Grain,int> grains_;

	public:
	// Return list of contained atoms
	OrderedList< Atom >& atoms();
	// Move specified atom to specified Cell
	bool moveAtom(Atom* i, Cell* targetCell);
	// Add atom to Cell
	bool addAtom(Atom& atom);
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
	///@}


	/*!
	 * \name Neighbours
	 */
	///@{
	private:
	// List of neighbouring Cells, and whether they are images
	RefList<Cell,bool> neighbours_;
	// Maximum number of atoms in neighbour list
	int maxAtomNeighbours_;
	// Number of atoms in neighbour list
	int nAtomNeighbours_;
	// Number of atoms in neighbour list requiring mim
	int nMimAtomNeighbours_;
	// Arrays of neighbouring atoms, within the defined potential cutoff (from anywhere in the cell)
	Atom** atomNeighbours_, **mimAtomNeighbours_;

	public:
	// Add Cell neighbour
	void addNeighbour(Cell* cell, bool isImage);
	// Return number of neighbours in list
	int nNeighbours();
	// Return neighbour list
	RefList<Cell,bool>& neighbours();
	// Initialise atom neighbour list
	void initialiseAtomNeighbourList(double atomicDensity, double ppRange, Vec3< double > realCellSize);
	// Clear atom neighbour list
	void clearAtomNeighbourList();
	// Add atom to neighbour list
	void addAtomToNeighbourList(Atom* i, bool useMim);
	// Return atom neighbour list
	Atom** atomNeighbours();
	// Return number of atom neighbours in list
	int nAtomNeighbours();
	// Return atom neighbour list requiring mim
	Atom** mimAtomNeighbours();
	// Return number of atom neighbours in list requiring mim
	int nMimAtomNeighbours();
	///@}
};

#endif
