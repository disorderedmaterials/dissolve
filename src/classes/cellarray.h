/*
	*** Cell Array
	*** src/classes/cellarray.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_CELLARRAY_H
#define DUQ_CELLARRAY_H

#include "math/matrix3.h"
#include "templates/list.h"

// Forward Declarations
class Box;
class Cell;

// Cell Array
class CellArray
{
	public:
	// Constructor
	CellArray();
	// Destructor
	~CellArray();


	/*
	 * Cell Data
	 */
	private:
	// Cell divisions along each axis
	Vec3<int> divisions_;
	// Fractional Cell size
	Vec3<double> fractionalCellSize_;
	// Real Cell size
	Vec3<double> realCellSize_;
	// Cell extents out from given central cell
	Vec3<int> extents_;
	// List of Cell neighbour indices (within pair potential range)
	List< ListVec3<int> > neighbourIndices_;
	// Cell axes
	Matrix3 axes_;
	// Total number of Cells in Box
	int nCells_;
	// Cell array (one-dimensional)
	Cell* cells_;
	// Box associated with this cell division scheme
	const Box* box_;

	public:
	// Generate array for provided Box
	bool generate(const Box* box, double cellSize, double pairPotentialRange, double atomicDensity);
	// Clear Cell arrays
	void clear();
	// Return number of Cells for box
	int nCells() const;
	// Return cell divisions along each axis
	Vec3<int> divisions() const;
	// Return real Cell dimensions
	Vec3<double> realCellSize() const;
	// Return cell extents out from given central cell
	Vec3<int> extents() const;
	// Return list of Cell neighbour indices
	List< ListVec3<int> > neighbourIndices() const;
	// Retrieve Cell with (wrapped) grid reference specified
	Cell* cell(int x, int y, int z) const;
	// Retrieve Cell with id specified
	Cell* cell(int id) const;
	// Return Cell which contains specified coordinate
	Cell* cell(const Vec3<double> r) const;
	// Return whether two Cells need minimum image calculation
	bool useMim(const Cell* a, const Cell* b) const;
	// Return if any Atoms in the supplied Cells are within the range supplied
	bool withinRange(const Cell* a, const Cell* b, double distance);
	// Return the minimum image grid delta between the two specified Cells
	Vec3<int> mimGridDelta(const Cell* a, const Cell* b) const;
};

#endif
