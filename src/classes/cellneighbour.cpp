/*
    *** Cell Neighbour
    *** src/classes/cellneighbour.cpp
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

#include "classes/cellneighbour.h"
#include <cstdlib>

CellNeighbour::CellNeighbour()
{
    cell_ = nullptr;
    useMim_ = false;
}

CellNeighbour::~CellNeighbour() {}

// Set cell and mim flag
void CellNeighbour::set(Cell *cell, bool useMim)
{
    cell_ = cell;
    useMim_ = useMim;
}

// Return referenced cell
const Cell *CellNeighbour::cell() const { return cell_; }

// Return whether mim should be applied
bool CellNeighbour::useMim() { return useMim_; }
