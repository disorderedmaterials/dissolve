/*
	*** Cell Distributor
	*** src/classes/celldistributor.h
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

#ifndef DISSOLVE_CELLDISTRIBUTOR_H
#define DISSOLVE_CELLDISTRIBUTOR_H

#include "classes/distributor.h"

// Forward Declarations
/* none */

// Cell Distributor
class CellDistributor : public Distributor
{
	public:
	// Constructor
	CellDistributor(const CellArray& cellArray, ProcessPool& procPool, ProcessPool::DivisionStrategy strategy, bool repeatsAllowed);
	// Destructor
	~CellDistributor();


	/*
	 * Data
	 */
	private:
	// Source CellArray
	const CellArray& cells_;


	/*
	 * Cells
	 */
	private:
	// Return array of Cells that we must hard lock in order to modify the object with index specified
	Array<Cell*> cellsToBeModifiedForObject(int objectId);
};

#endif
