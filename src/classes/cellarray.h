// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "math/matrix3.h"
#include "templates/list.h"

// Forward Declarations
class Box;
class Cell;

// Cell Array
class CellArray
{
    public:
    CellArray();
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
    // Cell axes
    Matrix3 axes_;
    // Cell array (one-dimensional)
    std::vector<std::unique_ptr<Cell>> cells_;
    // Box associated with this cell division scheme
    const Box *box_;

    public:
    // Generate array for provided Box
    bool generate(const Box *box, double cellSize, double pairPotentialRange);
    // Scale Cells sizes by supplied factor
    void scale(double factor);
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
    // Retrieve Cell with (wrapped) grid reference specified
    Cell *cell(int x, int y, int z) const;
    // Retrieve Cell with id specified
    Cell *cell(int id) const;
    // Return Cell which contains specified coordinate
    Cell *cell(const Vec3<double> r) const;
    // Check if it is possible for any pair of Atoms in the supplied cells to be within the specified distance
    bool withinRange(const Cell *a, const Cell *b, double distance);
    // Check if minimum image calculation is necessary for any potential pair of atoms in the supplied cells
    bool minimumImageRequired(const Cell *a, const Cell *b, double distance);
    // Return the minimum image grid delta between the two specified Cells
    Vec3<int> mimGridDelta(const Cell *a, const Cell *b) const;
    // Return the minimum image equivalent of the supplied grid delta
    Vec3<int> mimGridDelta(Vec3<int> delta) const;
};
