// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/cellArray.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/pairPotential.h"
#include "math/mathFunc.h"

/*
 * Cell Data
 */

// Return number of Cells for box
int CellArray::nCells() const { return static_cast<int>(cells_.size()); }

// Return cell divisions along each axis
Vector3i CellArray::divisions() const { return divisions_; }

// Return real Cell dimensions
Vector3 CellArray::realCellSize() const { return realCellSize_; }

// Return cell extents out from given central cell
Vector3i CellArray::extents() const { return extents_; }

// Clear all atom pointers from cells
void CellArray::clearAtoms()
{
    for (auto &cell : cells_)
        cell.clearAtoms();
}

// Retrieve Cell with (wrapped) grid reference specified
const Cell *CellArray::cell(int x, int y, int z) const
{
    x = x % divisions_.x;
    if (x < 0)
        x += divisions_.x;
    y = y % divisions_.y;
    if (y < 0)
        y += divisions_.y;
    z = z % divisions_.z;
    if (z < 0)
        z += divisions_.z;
    return &cells_[x * divisions_.y * divisions_.z + y * divisions_.z + z];
}

// Retrieve Cell with id specified
const Cell *CellArray::cell(int id) const
{
    assert(id >= 0 && id < cells_.size());

    return &cells_[id];
}

// Return Cell which contains specified coordinate
const Cell *CellArray::cell(const Vector3 r) const
{
    auto foldFracR = box_->foldFrac(r);
    Vector3i indices;
    indices.x = foldFracR.x / fractionalCellSize_.x;
    indices.y = foldFracR.y / fractionalCellSize_.y;
    indices.z = foldFracR.z / fractionalCellSize_.z;
    indices.x %= divisions_.x;
    indices.y %= divisions_.y;
    indices.z %= divisions_.z;

    return &cells_[indices.x * divisions_.y * divisions_.z + indices.y * divisions_.z + indices.z];
}
Cell *CellArray::cell(const Vector3 r)
{
    auto foldFracR = box_->foldFrac(r);
    Vector3i indices;
    indices.x = foldFracR.x / fractionalCellSize_.x;
    indices.y = foldFracR.y / fractionalCellSize_.y;
    indices.z = foldFracR.z / fractionalCellSize_.z;
    indices.x %= divisions_.x;
    indices.y %= divisions_.y;
    indices.z %= divisions_.z;

    return &cells_[indices.x * divisions_.y * divisions_.z + indices.y * divisions_.z + indices.z];
}

// Return whether it is possible for any pair of Atoms in the supplied cells to be within the specified mim distance
bool CellArray::withinMinimumImageRange(const Cell *a, const Cell *b, double mimDistance)
{
    assert(a != nullptr);
    assert(b != nullptr);

    // Get relevant index in the lookup array
    auto v = mimGridDelta(a, b);

    // If the minimum corner distance is less than the specified distance, the cells are within literal distance range
    return cellMinimumDistances_[{v.x, v.y, v.z}] <= mimDistance;
}

// Return the minimum image grid delta between the two specified Cells
Vector3i CellArray::mimGridDelta(const Cell *a, const Cell *b) const
{
    return mimGridDelta(b->gridReference() - a->gridReference());
}

// Return the minimum image equivalent of the supplied grid delta
Vector3i CellArray::mimGridDelta(const Vector3i delta) const
{
    auto result = delta;
    if (result.x > divisions_.x * 0.5)
        result.x -= divisions_.x;
    else if (result.x < -divisions_.x * 0.5)
        result.x += divisions_.x;
    if (result.y > divisions_.y * 0.5)
        result.y -= divisions_.y;
    else if (result.y < -divisions_.y * 0.5)
        result.y += divisions_.y;
    if (result.z > divisions_.z * 0.5)
        result.z -= divisions_.z;
    else if (result.z < -divisions_.z * 0.5)
        result.z += divisions_.z;
    return result;
}

// Return wrapped cell grid reference
Vector3i CellArray::wrappedGridRef(const Vector3i &gridRef) const
{
    auto result = gridRef;

    if (result.x < 0)
        result.x += divisions_.x;
    else if (result.x >= divisions_.x)
        result.x -= divisions_.x;
    if (result.y < 0)
        result.y += divisions_.y;
    else if (result.y >= divisions_.y)
        result.y -= divisions_.y;
    if (result.z < 0)
        result.x += divisions_.z;
    else if (result.z >= divisions_.z)
        result.z -= divisions_.z;

    return result;
}

/*
 * Cell Neighbour
 */

// Construct cell neighbour pairs
void CellArray::createCellNeighbourPairs()
{
    auto nPairs = DissolveMath::triangularIncDiagonals(cells_.size());
    neighbourPairs_.clear();
    neighbourPairs_.reserve(nPairs);
    for (auto &cell : cells_)
        for (auto &nbr : neighbours(cell))
            if (cell.index() <= nbr.cell.index())
                neighbourPairs_.emplace_back(cell, nbr.cell, nbr.requiresMIM);
}

// Return neighbour vector for specified cell, including self as first item
const std::vector<CellNeighbour> &CellArray::neighbours(const Cell &cell) const { return neighbours_[cell.index()]; }

// Return vector of all unique cell neighbour pairs
const std::vector<CellNeighbourPair> &CellArray::getCellNeighbourPairs() const { return neighbourPairs_; }

// Return whether minimum image calculation is required between the supplied cells
bool CellArray::minimumImageRequired(const Cell &a, const Cell &b) const
{
    auto it = std::find(neighbours_[a.index()].begin(), neighbours_[a.index()].end(), b);
    if (it == neighbours_[a.index()].end())
        return false;
    return it->requiresMIM;
}

/*
 * Generation
 */

// Generate Cells for Box
bool CellArray::generate(const Box *box, double cellSize)
{
    // We need to regenerate the cell array only if it is currently empty or the pair potential range has changed
    if (!cells_.empty() && pairPotentialRangeCreatedAt_ && pairPotentialRangeCreatedAt_.value() == PairPotential::range())
        return true;

    clear();

    const auto minCellsPerSide = 3;
    const auto tolerance = 0.01;

    pairPotentialRangeCreatedAt_ = PairPotential::range();
    box_ = box;

    Messenger::print("Generating cells for box - minimum cells per side is {}, cell size is {}...\n", minCellsPerSide,
                     cellSize);

    // Get Box axis lengths and divide through by cellSize
    Vector3 boxLengths(box_->axisLength(0), box_->axisLength(1), box_->axisLength(2));
    Vector3i divisions(boxLengths.x / cellSize, boxLengths.y / cellSize, boxLengths.z / cellSize);
    int minEl;
    divisions_.zero();
    realCellSize_.zero();

    Messenger::print("Initial divisions based on cell size are (x,y,z) = ({},{},{})\n", divisions.x, divisions.y, divisions.z);

    // How does the smallest length compare with the PairPotential range?
    if (divisions.min() < minCellsPerSide)
    {
        Messenger::warn("Box size only allows for {} whole divisions of the cell size ({}) along one or more axes, "
                        "while we require at least {}.\n",
                        divisions.min(), cellSize, minCellsPerSide);

        // We must now take the shortest box length and divide by 3 to get the absolute maximum length to use on that side
        minEl = boxLengths.minElement();
        realCellSize_[minEl] = boxLengths[minEl] / minCellsPerSide;
        divisions_[minEl] = minCellsPerSide;
    }
    else
    {
        // All Box lengths are long enough to be partitioned in to at least minCellsPerSide
        // Just find shortest of them and set its exact distance
        minEl = boxLengths.minElement();
        realCellSize_[minEl] = boxLengths[minEl] / divisions[minEl];
        divisions_[minEl] = divisions[minEl];
    }

    Messenger::print("Shortest side (axis {}) will have cell length of {} Angstroms.\n", minEl, realCellSize_[minEl]);

    // Now, set our other cellLengths_ based on the minimum value we have just set
    // We try to get all lengths as similar as possible
    for (auto n = 1; n < 3; ++n)
    {
        auto el = (minEl + n) % 3;
        double x = boxLengths[el] / realCellSize_[minEl];
        double remainder = x - int(x);

        // If we're within a tolerance to the nearest integer, use this number....
        // If not, we can round 'x' to the nearest integer, but can only round up if ppRange allows us to...
        if (remainder > (1.0 - tolerance))
        {
            divisions_[el] = int(x) + 1;
            realCellSize_[el] = boxLengths[el] / divisions_[el];
            Messenger::print("Accepted cell length of {} Angstroms ({} divisions) for axis {}, since it was within "
                             "tolerance (-{:e}).\n",
                             realCellSize_[minEl], divisions_[el], el, remainder);
        }
        else if (remainder < tolerance)
        {
            divisions_[el] = int(x);
            realCellSize_[el] = boxLengths[el] / divisions_[el];
            Messenger::print("Accepted cell length of {} Angstroms ({} divisions) for axis {}, since it was within "
                             "tolerance (+{:e}).\n",
                             realCellSize_[minEl], divisions_[el], el, remainder);
        }
        else if (remainder < 0.5)
        {
            // Can't fit more than half another cell in, so reduce number of divisions...
            divisions_[el] = int(x);
            realCellSize_[el] = boxLengths[el] / divisions_[el];
            Messenger::print("Decreased cell length for axis {} to {} Angstroms ({} divisions).\n", el, realCellSize_[el],
                             divisions_[el]);
        }
        else
        {
            // Can fit more than half another Cell in - can we increase to the next integer?
            divisions_[el] = int(x) + 1;
            realCellSize_[el] = boxLengths[el] / divisions_[el];
            if (realCellSize_[el] < cellSize)
            {
                --divisions_[el];
                realCellSize_[el] = boxLengths[el] / divisions_[el];
                Messenger::print("Forced decrease of cell length for axis {} to {} Angstroms ({} divisions) "
                                 "since increasing it gave a length larger than the cell size.\n",
                                 el, realCellSize_[el], divisions_[el]);
            }
            else
                Messenger::print("Increased cell length for axis {} to {} Angstroms ({} divisions).\n", el, realCellSize_[el],
                                 divisions_[el]);
        }
    }

    // Summarise
    fractionalCellSize_.set(1.0 / divisions_.x, 1.0 / divisions_.y, 1.0 / divisions_.z);
    Messenger::print("Final cell partitioning is (x,y,z) = ({},{},{}), giving {} cells in total.\n", divisions_.x, divisions_.y,
                     divisions_.z, divisions_.x * divisions_.y * divisions_.z);
    Messenger::print("Fractional cell size is ({},{},{}).\n", fractionalCellSize_.x, fractionalCellSize_.y,
                     fractionalCellSize_.z);

    // Construct Cell arrays
    clear();
    auto nCells = divisions_.x * divisions_.y * divisions_.z;
    Messenger::print("Constructing array of {} cells...\n", nCells);
    cells_.resize(nCells);
    Vector3 fracCentre(fractionalCellSize_.x * 0.5, 0.0, 0.0);
    auto count = 0;
    for (auto x = 0; x < divisions_.x; ++x)
    {
        fracCentre.y = fractionalCellSize_.y * 0.5;
        for (auto y = 0; y < divisions_.y; ++y)
        {
            fracCentre.z = fractionalCellSize_.z * 0.5;
            for (auto z = 0; z < divisions_.z; ++z)
            {
                cells_[count] = Cell(count, Vector3i(x, y, z), box_->getReal(fracCentre));
                fracCentre.z += fractionalCellSize_.z;
                ++count;
            }
            fracCentre.y += fractionalCellSize_.y;
        }
        fracCentre.x += fractionalCellSize_.x;
    }

    // Calculate Cell axes matrix
    axes_ = box_->axes();
    axes_.columnMultiply(fractionalCellSize_);

    // Create cell distance matrix giving us the minimum "corner distances" between a cell at 0,0,0 and the max cell divisions.
    // These represent the minimum and maximum possible contact distances between any atoms located in each cell.
    cellMinimumDistances_.initialise(-divisions_.x, divisions_.x, -divisions_.y, divisions_.y, -divisions_.z, divisions_.z);
    std::vector<Vector3i> neighbourIndices;
    std::set<const Cell *> neighbourCells;
    std::vector<std::pair<int, int>> edges = {{0, 1}, {2, 3}, {4, 5}, {6, 7}, {0, 2}, {1, 3},
                                              {4, 6}, {5, 7}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};
    for (auto x = -divisions_.x; x <= divisions_.x; ++x)
    {
        for (auto y = -divisions_.y; y <= divisions_.y; ++y)
        {
            for (auto z = -divisions_.z; z <= divisions_.z; ++z)
            {
                if (x == 0 && y == 0 && z == 0)
                    continue;

                // Determine shortest distance between the edges of the surrounding cell j and the corners of the central cell
                cellMinimumDistances_[{x, y, z}] = 1.0e10;
                auto minLiteral = 1.0e10;

                for (const auto &[cornerB, cornerC] : edges)
                {
                    // Get coordinates at the endpoints of the defined line BC (cell edge)
                    auto B = axes_ * Vector3(x + (cornerB & 1 ? 1 : 0), y + (cornerB & 2 ? 1 : 0), z + (cornerB & 4 ? 1 : 0));
                    auto C = axes_ * Vector3(x + (cornerC & 1 ? 1 : 0), y + (cornerC & 2 ? 1 : 0), z + (cornerC & 4 ? 1 : 0));

                    // Determine normalised line vector d
                    auto d = C - B;
                    auto dMag = d.magAndNormalise();

                    // Loop over corners of the central cell
                    for (auto cornerA = 0; cornerA < 8; ++cornerA)
                    {
                        // Get coordinates of A
                        auto A = axes_ * Vector3(cornerA & 1 ? 1 : 0, cornerA & 2 ? 1 : 0, cornerA & 4 ? 1 : 0);

                        // Determine vector v = BA
                        auto v = A - B;

                        // Determine the distance of the intersection on the line from point B
                        auto t = v.dp(d);

                        // Determine closest point on line
                        Vector3 P;
                        if (t < 0.0)
                            P = B;
                        else if (t > dMag)
                            P = C;
                        else
                            P = B + d * t;

                        // Get shortest distance
                        auto rAP = (P - A).magnitude();
                        if (rAP < minLiteral)
                            minLiteral = rAP;

                        // Get minimum image'd shortest distance
                        auto rAPmim = box_->minimumDistance(A, P);
                        if (rAPmim < cellMinimumDistances_[{x, y, z}])
                            cellMinimumDistances_[{x, y, z}] = rAPmim;
                    }
                }

                // If the minimum possible literal distance between the cells is less than the pairpotential range, store it
                if (minLiteral <= (PairPotential::range()))
                {
                    // Check that the cell is not already in the list by querying the cellNbrs vector
                    auto *nbr = cell(x, y, z);
                    if (!neighbourCells.contains(nbr))
                    {
                        neighbourIndices.emplace_back(x, y, z);
                        neighbourCells.insert(nbr);
                    }
                }
            }
        }
    }
    Messenger::print("There are {} cells neighbours within the pair potential range.\n", neighbourIndices.size());

    // Construct neighbour arrays for individual Cells
    Messenger::print("Creating cell neighbour lists...\n");
    neighbours_.clear();
    neighbours_.resize(cells_.size());
    for (auto &nbrVector : neighbours_)
        nbrVector.reserve(neighbourIndices.size() + 1);

    for (auto &centralCell : cells_)
    {
        auto &nbrs = neighbours_[centralCell.index()];
        nbrs.emplace_back(centralCell, false);

        for (auto &index : neighbourIndices)
        {
            // Find neighbour with the relative indices provided
            auto relIndex = centralCell.gridReference() + index;

            // Add neighbour, flagging closer minimum image cell if present
            nbrs.emplace_back(*cell(relIndex.x, relIndex.y, relIndex.z), wrappedGridRef(relIndex) != index);
        }
    }

    // Generate neighbour pairs
    createCellNeighbourPairs();

    return true;
}

// Clear Cell arrays
void CellArray::clear() { cells_.clear(); }

/*
 * Operations
 */

// Scale Cells by supplied factors along each axis
void CellArray::scale(Vector3 scaleFactors)
{
    realCellSize_.multiply(scaleFactors);
    axes_.columnMultiply(scaleFactors);
}
