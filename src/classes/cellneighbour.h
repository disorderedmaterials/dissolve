// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
