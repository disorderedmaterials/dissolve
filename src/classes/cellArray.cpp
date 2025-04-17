// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/cellArray.h"
#include "classes/box.h"
#include "classes/cell.h"

/*
 * Cell Data
 */

// Return number of Cells for box
int CellArray::nCells() const { return static_cast<int>(cells_.size()); }

// Return cell divisions along each axis
Vec3<int> CellArray::divisions() const { return divisions_; }

// Return real Cell dimensions
Vec3<double> CellArray::realCellSize() const { return realCellSize_; }

// Return cell extents out from given central cell
Vec3<int> CellArray::extents() const { return extents_; }

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
const Cell *CellArray::cell(const Vec3<double> r) const
{
    auto foldFracR = box_->foldFrac(r);
    Vec3<int> indices;
    indices.x = foldFracR.x / fractionalCellSize_.x;
    indices.y = foldFracR.y / fractionalCellSize_.y;
    indices.z = foldFracR.z / fractionalCellSize_.z;
    indices.x %= divisions_.x;
    indices.y %= divisions_.y;
    indices.z %= divisions_.z;

    return &cells_[indices.x * divisions_.y * divisions_.z + indices.y * divisions_.z + indices.z];
}
Cell *CellArray::cell(const Vec3<double> r)
{
    auto foldFracR = box_->foldFrac(r);
    Vec3<int> indices;
    indices.x = foldFracR.x / fractionalCellSize_.x;
    indices.y = foldFracR.y / fractionalCellSize_.y;
    indices.z = foldFracR.z / fractionalCellSize_.z;
    indices.x %= divisions_.x;
    indices.y %= divisions_.y;
    indices.z %= divisions_.z;

    return &cells_[indices.x * divisions_.y * divisions_.z + indices.y * divisions_.z + indices.z];
}

// Return whether it is possible for any pair of Atoms in the supplied cells to be within the specified literal distance
bool CellArray::withinLiteralRange(const Cell *a, const Cell *b, double literalDistance)
{
    assert(a != nullptr);
    assert(b != nullptr);

    // Get relevant index in the lookup array
    auto v = mimGridDelta(a, b) + cornerDistancesOrigin_;

    // If the minimum corner distance is less than the specified distance, the cells are within literal distance range
    return cornerDistances_[{v.x, v.y, v.z}].minimumLiteral <= literalDistance;
}

// Return whether it is possible for any pair of Atoms in the supplied cells to be within the specified mim distance
bool CellArray::withinMinimumImageRange(const Cell *a, const Cell *b, double mimDistance)
{
    assert(a != nullptr);
    assert(b != nullptr);

    // Get relevant index in the lookup array
    auto v = mimGridDelta(a, b) + cornerDistancesOrigin_;

    // If the minimum corner distance is less than the specified distance, the cells are within literal distance range
    return cornerDistances_[{v.x, v.y, v.z}].minimumMim <= mimDistance;
}

// Return the minimum image grid delta between the two specified Cells
Vec3<int> CellArray::mimGridDelta(const Cell *a, const Cell *b) const
{
    return mimGridDelta(b->gridReference() - a->gridReference());
}

// Return the minimum image equivalent of the supplied grid delta
Vec3<int> CellArray::mimGridDelta(const Vec3<int> &delta) const
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
Vec3<int> CellArray::wrappedGridRef(const Vec3<int> &gridRef) const
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
    auto nPairs = cells_.size() * ((cells_.size() + 1) / 2);
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
bool CellArray::generate(const Box *box, double cellSize, double pairPotentialRange)
{
    clear();

    const auto minCellsPerSide = 3;
    const auto tolerance = 0.01;

    box_ = box;

    Messenger::print("Generating cells for box - minimum cells per side is {}, cell size is {}...\n", minCellsPerSide,
                     cellSize);

    // Get Box axis lengths and divide through by cellSize
    Vec3<double> boxLengths(box_->axisLength(0), box_->axisLength(1), box_->axisLength(2));
    Vec3<int> divisions(boxLengths.x / cellSize, boxLengths.y / cellSize, boxLengths.z / cellSize);
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
    Vec3<double> fracCentre(fractionalCellSize_.x * 0.5, 0.0, 0.0);
    auto count = 0;
    for (auto x = 0; x < divisions_.x; ++x)
    {
        fracCentre.y = fractionalCellSize_.y * 0.5;
        for (auto y = 0; y < divisions_.y; ++y)
        {
            fracCentre.z = fractionalCellSize_.z * 0.5;
            for (auto z = 0; z < divisions_.z; ++z)
            {
                cells_[count] = Cell(count, Vec3<int>(x, y, z), box_->getReal(fracCentre));
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
    cornerDistances_.initialise(divisions_.x * 2 - 1, divisions_.y * 2 - 1, divisions_.z * 2 - 1);
    cornerDistancesOrigin_ = divisions_ - Vec3<int>(1, 1, 1);
    for (auto x = -divisions_.x + 1; x < divisions_.x; ++x)
    {
        for (auto y = -divisions_.y + 1; y < divisions_.y; ++y)
        {
            for (auto z = -divisions_.z + 1; z < divisions_.z; ++z)
            {
                // Determine corner distance extrema
                auto minLiteral = 1.0e6, maxLiteral = 0.0, minMim = 1.0e6, maxMim = 0.0;
                for (auto iCorner = 0; iCorner < 8; ++iCorner)
                {
                    // Set integer vertex of corner on 'central' box
                    const auto i = Vec3<int>(iCorner & 1 ? 1 : 0, iCorner & 2 ? 1 : 0, iCorner & 4 ? 1 : 0);

                    // Get real coordinates of i
                    const auto ri = axes_ * Vec3<double>(i.x, i.y, i.z);

                    for (auto jCorner = 0; jCorner < 8; ++jCorner)
                    {
                        // Set integer vertex of corner on 'other' box
                        Vec3<int> j(x + (jCorner & 1 ? 1 : 0), y + (jCorner & 2 ? 1 : 0), z + (jCorner & 4 ? 1 : 0));

                        // Get real coordinates of j
                        const auto rj = axes_ * Vec3<double>(j.x, j.y, j.z);
                        // Calculate corner distance and update literal extrema
                        auto rij = (ri - rj).magnitude();
                        if (rij < minLiteral)
                            minLiteral = rij;
                        else if (rij > maxLiteral)
                            maxLiteral = rij;

                        // Get minimum image grid position and corner distance
                        auto rijmim = box_->minimumDistance(ri, rj);
                        if (rijmim < minMim)
                            minMim = rijmim;
                        else if (rijmim > maxMim)
                            maxMim = rijmim;
                    }
                }

                cornerDistances_[{x + cornerDistancesOrigin_.x, y + cornerDistancesOrigin_.y, z + cornerDistancesOrigin_.z}] =
                    CornerDistances(minLiteral, maxLiteral, minMim, maxMim);
            }
        }
    }

    // Construct Cell neighbour lists
    Messenger::print("Creating cell neighbour lists...\n");

    // Make a list of integer vectors which we'll then use to pick Cells for the neighbour lists
    Vec3<double> r;
    Matrix3 cellAxes = box_->axes();
    cellAxes.applyScaling(fractionalCellSize_.x, fractionalCellSize_.y, fractionalCellSize_.z);
    extents_.zero();

    // First, establish a maximal extent in principal directions...
    for (auto n = 0; n < 3; ++n)
    {
        do
        {
            r.zero();
            ++extents_[n];
            r[n] = extents_[n];
            r = cellAxes * r;
        } while (r[n] < pairPotentialRange);

        // If we require a larger number of cells than the box physically has along this direction, reduce it accordingly
        if ((extents_[n] * 2 + 1) > divisions_[n])
            extents_[n] = divisions_[n] / 2;
    }
    Messenger::print("Cell extents required to cover PairPotential range are (x,y,z) = ({},{},{}).\n", extents_.x, extents_.y,
                     extents_.z);

    // Now, loop over extent integers and construct list of gridReferences within range
    std::vector<Vec3<int>> neighbourIndices;
    std::vector<const Cell *> cellNbrs;
    Vec3<int> i, j;
    const Cell *nbr;
    for (auto x = -extents_.x; x <= extents_.x; ++x)
    {
        for (auto y = -extents_.y; y <= extents_.y; ++y)
        {
            for (auto z = -extents_.z; z <= extents_.z; ++z)
            {
                if ((x == 0) && (y == 0) && (z == 0))
                    continue;

                // Check a nominal central cell at (0,0,0) and this grid reference to see if any pairs of
                // corners are in range
                auto close = false;
                for (auto iCorner = 0; iCorner < 8; ++iCorner)
                {
                    // Set integer vertex of corner on 'central' box
                    i.set(iCorner & 1 ? 1 : 0, iCorner & 2 ? 1 : 0, iCorner & 4 ? 1 : 0);

                    for (auto jCorner = 0; jCorner < 8; ++jCorner)
                    {
                        // Set integer vertex of corner on 'other' box
                        j.set(x + (jCorner & 1 ? 1 : 0), y + (jCorner & 2 ? 1 : 0), z + (jCorner & 4 ? 1 : 0));

                        // Get minimum image of vertex j w.r.t. i
                        j = mimGridDelta(j - i);

                        r.set(j.x, j.y, j.z);
                        r = cellAxes * r;
                        if (r.magnitude() < pairPotentialRange)
                        {
                            close = true;
                            break;
                        }
                    }
                    if (close)
                        break;
                }
                if (!close)
                    continue;

                // Check that the cell is not already in the list by querying the cellNbrs vector
                nbr = cell(x, y, z);
                if (std::find(cellNbrs.begin(), cellNbrs.end(), nbr) != cellNbrs.end())
                    continue;
                neighbourIndices.emplace_back(x, y, z);
                cellNbrs.push_back(nbr);
            }
        }
    }
    Messenger::print("Added {} Cells to representative neighbour list.\n", neighbourIndices.size());

    // Construct neighbour arrays for individual Cells
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
void CellArray::scale(Vec3<double> scaleFactors)
{
    realCellSize_.multiply(scaleFactors);
    axes_.columnMultiply(scaleFactors);
}
