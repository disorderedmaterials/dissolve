/*
    *** Cell Neighbour
    *** src/classes/cellneighbour.h
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

#pragma once

// Forward Declarations
class Cell;

// Cell Neighbour
class CellNeighbour
{
    /*
     * Simple class encapsulating a pointer to a Cell, and a flag indicating whether or not mimimum image calculations
     * should be applied when considering the contents of this cell with the cell which owns the array in which it appears.
     */
    public:
    CellNeighbour();
    ~CellNeighbour();

    private:
    // Cell pointer
    Cell *cell_;
    // Whether mim should be used with this Cell
    bool useMim_;

    public:
    // Set cell and mim flag
    void set(Cell *cell, bool useMim);
    // Return referenced cell
    const Cell *cell() const;
    // Return whether mim should be applied
    bool useMim();
};
