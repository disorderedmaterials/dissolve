/*
	*** Cell Neighbour
	*** src/lib/classes/cellneighbour.h
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

#ifndef DUQ_CELLNEIGHBOUR_H
#define DUQ_CELLNEIGHBOUR_H

// Forward Declarations
class Cell;

/*!
 * \brief Cell Neighbour
 * \details Simple class encapsulating a pointer to a Cell, and a flag indicating whether or not mimimum image calculations should be applied when 
 * considering the contents of this cell with the cell which owns the OrderedList in which it appears.
 */
class CellNeighbour
{
	public:
	// Constructor
	CellNeighbour();
	// Destructor
	~CellNeighbour();

	private:
	// Cell pointer
	Cell* cell_;
	// Whether mim should be used with this Cell
	bool useMim_;

	public:
	// Set cell and mim flag
	void set(Cell* cell, bool useMim);
	// Return referenced cell
	Cell* cell();
	// Return whether mim should be applied
	bool useMim();
};

#endif
