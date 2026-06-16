// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/cell.h"
#include "math/matrix3.h"
#include "templates/array3D.h"

// Forward Declarations
class Box;

// Cell Neighbour
struct CellNeighbour
{
    CellNeighbour(const Cell &n, bool mim) : cell(n), requiresMIM(mim) {}
    const Cell &cell;
    bool requiresMIM{false};

    bool operator==(const Cell &other) const { return this->cell.index() == other.index(); }
};

// Cell Neighbour Pair
struct CellNeighbourPair
{
    CellNeighbourPair(const Cell &c, const Cell &n, bool mim) : cell(c), neighbour(n), requiresMIM(mim) {}
    const Cell &cell;
    const Cell &neighbour;
    bool requiresMIM = false;

    bool operator==(const CellNeighbourPair &other) const
    {
        return (&this->cell == &other.cell && &this->neighbour == &other.neighbour) ||
               (&this->neighbour == &other.cell && &this->cell == &other.neighbour);
    }
};

// Cell Array
class CellArray
{
    /*
     * Cell Data
     */
    private:
    // Cell divisions along each axis
    Vector3i divisions_;
    // Fractional Cell size
    Vector3 fractionalCellSize_;
    // Real Cell size
    Vector3 realCellSize_;
    // Cell extents out from given central cell
    Vector3i extents_;
    // Cell axes
    Matrix3 axes_;
    // Cell array (one-dimensional)
    std::vector<Cell> cells_;
    // Pair potential range at which the array was last created
    std::optional<double> pairPotentialRangeCreatedAt_;
    // Box axes at which the array was last created
    std::optional<Matrix3> axesCreatedAt_;
    // Box associated with this cell division scheme
    const Box *box_{nullptr};

    public:
    // Return number of Cells for box
    int nCells() const;
    // Return cell divisions along each axis
    Vector3i divisions() const;
    // Return real Cell dimensions
    Vector3 realCellSize() const;
    // Return cell extents out from given central cell
    Vector3i extents() const;
    // Clear all atom pointers from cells
    void clearAtoms();
    // Retrieve Cell with (wrapped) grid reference specified
    const Cell *cell(int x, int y, int z) const;
    // Retrieve Cell with id specified
    const Cell *cell(int id) const;
    // Return Cell which contains specified coordinate
    Cell *cell(const Vector3 r);
    const Cell *cell(const Vector3 r) const;
    // Return the cell array
    const std::vector<Cell> &cells() const;
    // Return whether it is possible for any pair of Atoms in the supplied cells to be within the specified mim distance
    bool withinMinimumImageRange(const Cell *a, const Cell *b, double mimDistance);
    // Return the minimum image grid delta between the two specified Cells
    Vector3i mimGridDelta(const Cell *a, const Cell *b) const;
    // Return the minimum image equivalent of the supplied grid delta
    Vector3i mimGridDelta(Vector3i delta) const;
    // Return wrapped cell grid reference
    Vector3i wrappedGridRef(const Vector3i &gridRef) const;

    /*
     * Cell Neighbours
     */
    private:
    // Neighbour pair array (one-dimensional)
    std::vector<CellNeighbourPair> neighbourPairs_;
    // Neighbour array per Cell
    std::vector<std::vector<CellNeighbour>> neighbours_;
    // Minimum possible distances betwen cells around the origin
    OffsetArray3D<double> cellMinimumDistances_;

    private:
    // Construct cell neighbour pairs
    void createCellNeighbourPairs();

    public:
    // Return neighbour vector for specified cell, including self as first item
    const std::vector<CellNeighbour> &neighbours(const Cell &cell) const;
    // Return vector of all unique cell neighbour pairs
    const std::vector<CellNeighbourPair> &getCellNeighbourPairs() const;
    // Return whether minimum image calculation is required between the supplied cells
    bool minimumImageRequired(const Cell &a, const Cell &b) const;

    /*
     * Generation
     */
    public:
    // Generate array for provided Box
    bool generate(const Box &box, double cellSize);
    // Clear Cell arrays
    void clear();

    /*
     * Operations
     */
    public:
    // Scale Cells by supplied factors along each axis
    void scale(Vector3 scaleFactors);
};
