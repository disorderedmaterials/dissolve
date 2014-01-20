/*
	*** Cell Neighbour
	*** src/lib/classes/cellneighbour.cpp
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

#include "classes/cellneighbour.h"
#include <cstdlib>

/*!
 * \brief Constructor
 * \details Constructor for CellNeighbour. 
 */
CellNeighbour::CellNeighbour()
{
	cell_ = NULL;
	useMim_ = false;
}

/*!
 * \brief Destructor
 * \details Constructor for CellNeighbour. 
 */
CellNeighbour::~CellNeighbour()
{
}

/*!
 * \brief Set cell and mim flag
 */
void CellNeighbour::set(Cell* cell, bool useMim)
{
	cell_ = cell;
	useMim_ = useMim;
}

// Return referenced cell
Cell* CellNeighbour::cell()
{
	return cell_;
}

/*!
 * \brief Return whether mim should be applied
 */
bool CellNeighbour::useMim()
{
	return useMim_;
}
