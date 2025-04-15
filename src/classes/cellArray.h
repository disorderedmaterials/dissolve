// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/cellNeighbour.h"
#include "math/matrix3.h"
#include "templates/array3D.h"

// Forward Declarations
class Box;
class Cell;

// Corner Distances Structure
struct CornerDistances
{
    CornerDistances() {}
    CornerDistances(double minLiteral, double maxLiteral, double minMim, double maxMim)
        : minimumLiteral(minLiteral), maximumLiteral(maxLiteral), minimumMim(minMim), maximumMim(maxMim)
    {
    }

    // Used to store the minimum and maximum corner distances, literal and mim'd, between cells
    double minimumLiteral{0.0};
    double maximumLiteral{0.0};
    double minimumMim{0.0};
    double maximumMim{0.0};
};

// Cell Array
class CellArray
{
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
    std::vector<Cell> cells_;
    // Box associated with this cell division scheme
    const Box *box_{nullptr};

    public:
    // Return number of Cells for box
    int nCells() const;
    // Return cell divisions along each axis
    Vec3<int> divisions() const;
    // Return real Cell dimensions
    Vec3<double> realCellSize() const;
    // Return cell extents out from given central cell
    Vec3<int> extents() const;
    // Clear all atom pointers from cells
    void clearAtoms();
    // Retrieve Cell with (wrapped) grid reference specified
    const Cell *cell(int x, int y, int z) const;
    // Retrieve Cell with id specified
    const Cell *cell(int id) const;
    // Return Cell which contains specified coordinate
    Cell *cell(const Vec3<double> r);
    const Cell *cell(const Vec3<double> r) const;
    // Return whether it is possible for any pair of Atoms in the supplied cells to be within the specified literal distance
    bool withinLiteralRange(const Cell *a, const Cell *b, double literalDistance);
    // Return whether it is possible for any pair of Atoms in the supplied cells to be within the specified mim distance
    bool withinMinimumImageRange(const Cell *a, const Cell *b, double mimDistance);
    // Return the minimum image grid delta between the two specified Cells
    Vec3<int> mimGridDelta(const Cell *a, const Cell *b) const;
    // Return the minimum image equivalent of the supplied grid delta
    Vec3<int> mimGridDelta(const Vec3<int> &delta) const;
    // Return wrapped cell grid reference
    Vec3<int> wrappedGridRef(const Vec3<int> &gridRef) const;

    /*
     * Cell Neighbours
     */
    private:
    // Neighbour pair array (one-dimensional)
    std::vector<CellNeighbourPair> neighbourPairs_;
    // Neighbour array per Cell
    std::vector<std::vector<CellNeighbour>> neighbours_;
    // Corner distances between cells
    Array3D<CornerDistances> cornerDistances_;
    // Grid reference for central cell (0,0,0) in cornerDistances_
    Vec3<int> cornerDistancesOrigin_;

    private:
    // Add neighbour to cell vector
    void addNeighbour(const Cell &cell, const Cell &nbr, bool useMim);
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
    bool generate(const Box *box, double cellSize, double pairPotentialRange);
    // Clear Cell arrays
    void clear();

    /*
     * Operations
     */
    public:
    // Scale Cells by supplied factors along each axis
    void scale(Vec3<double> scaleFactors);
};
