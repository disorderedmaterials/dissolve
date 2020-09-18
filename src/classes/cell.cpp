/*
    *** Cell Definition
    *** src/classes/cell.cpp
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

#include "classes/cell.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/cellneighbour.h"
#include "templates/orderedvector.h"
#include <algorithm>

Cell::Cell()
{
    index_ = -1;
    nCellNeighbours_ = 0;
    nMimCellNeighbours_ = 0;
}

Cell::~Cell() {}

/*
 * Identity
 */

// Set grid reference
void Cell::setGridReference(int x, int y, int z) { gridReference_.set(x, y, z); }

// Return grid reference
const Vec3<int> &Cell::gridReference() const { return gridReference_; }

// Set unique index
void Cell::setIndex(int id)
{
    // To prevent misuse, the identity of a Cell can be changed from its starting value of -1 only once.
    if (id == -1)
        Messenger::print("BAD_USAGE - Refused to set the ID of a Cell more than once.\n");
    else
        index_ = id;
}

// Return unique index
int Cell::index() const { return index_; }

// Set real-space Cell centre
void Cell::setCentre(Vec3<double> r) { centre_ = r; }

// Return real-space Cell centre
const Vec3<double> &Cell::centre() const { return centre_; }

/*
 * Contents
 */

// Return array of contained Atoms
OrderedVector<Atom *> &Cell::atoms() { return atoms_; }
const OrderedVector<Atom *> &Cell::atoms() const { return atoms_; }

// Return array of contained Atoms, ordered by their array indices
const OrderedVector<Atom *> &Cell::indexOrderedAtoms() const { return indexOrderedAtoms_; }

// Return number of Atoms in list
int Cell::nAtoms() const { return atoms_.size(); }

// Add atom to Cell
bool Cell::addAtom(Atom *i)
{
#ifdef CHECKS
    if (i == nullptr)
    {
        Messenger::print("NULL_POINTER - nullptr given to Cell::addAtom().\n");
        return false;
    }
#endif
    // Add Atom to our pointer- and index-ordered arrays
    atoms_.insert(i);
    indexOrderedAtoms_.insert(i);

    if (i->cell())
        Messenger::warn("About to set Cell pointer in Atom {}, but this will overwrite an existing value.\n", i->arrayIndex());
    i->setCell(this);

    return true;
}

// Remove Atom from Cell
bool Cell::removeAtom(Atom *i)
{
#ifdef CHECKS
    if (i == nullptr)
    {
        Messenger::print("NULL_POINTER - nullptr given to Cell::removeAtom().\n");
        return false;
    }
#endif
    // Remove atom from this cell
    if (atoms_.erase(i))
    {
        indexOrderedAtoms_.erase(i);
        i->setCell(nullptr);
    }
    else
    {
        Messenger::error("Tried to remove Atom {} from Cell {}, but it was not present.\n", i->arrayIndex(), index_);
        return false;
    }

    return true;
}

/*
 * Neighbours
 */

// Add Cell neighbours
void Cell::addCellNeighbours(OrderedVector<Cell *> &nearNeighbours, OrderedVector<Cell *> &mimNeighbours)
{
    int n;

    // Create near-neighbour array of Cells not requiring minimum image to be applied
    nCellNeighbours_ = nearNeighbours.size();
    cellNeighbours_.resize(nCellNeighbours_);
    std::copy(nearNeighbours.begin(), nearNeighbours.end(), cellNeighbours_.begin());

    // Create array of neighbours that require minimum image calculation
    nMimCellNeighbours_ = mimNeighbours.size();
    mimCellNeighbours_.clear();
    mimCellNeighbours_.resize(nMimCellNeighbours_);
    std::copy(mimNeighbours.begin(), mimNeighbours.end(), mimCellNeighbours_.begin());

    // Create ordered list of CellNeighbours (including cells from both lists)
    OrderedVector<std::pair<Cell *, bool>> allCells;
    for (auto *nearNbr : nearNeighbours)
        allCells.emplace(nearNbr, false);
    for (auto *mimNbr : mimNeighbours)
        allCells.emplace(mimNbr, true);

    if (allCells.size() != (nCellNeighbours_ + nMimCellNeighbours_))
        Messenger::error("Cell neighbour lists are corrupt - same cell found in both near and mim lists.\n");
    allCellNeighbours_.resize(allCells.size());
    auto destination = allCellNeighbours_.begin();
    for (auto source : allCells)
    {
        (*destination).set(source.first, source.second);
        ++destination;
    }
}

// Return number of Cell near-neighbours, not requiring minimum image calculation
int Cell::nCellNeighbours() const { return nCellNeighbours_; }

// Return total number of Cell neighbours requiring minimum image calculation
int Cell::nMimCellNeighbours() const { return nMimCellNeighbours_; }

// Return total number of Cell neighbours
int Cell::nTotalCellNeighbours() const { return nCellNeighbours_ + nMimCellNeighbours_; }

// Return adjacent Cell neighbour list
const std::vector<Cell *> &Cell::cellNeighbours() const { return cellNeighbours_; }

// Return specified adjacent Cell neighbour
Cell *Cell::cellNeighbour(int id) const { return cellNeighbours_[id]; }

// Return list of Cell neighbours requiring minimum image calculation
const std::vector<Cell *> &Cell::mimCellNeighbours() const { return mimCellNeighbours_; }

// Return specified Cell neighbour, requiring minimum image calculation
Cell *Cell::mimCellNeighbour(int id) const { return mimCellNeighbours_[id]; }

// Return if the specified Cell requires minimum image calculation
bool Cell::mimRequired(const Cell *otherCell) const
{
    for (int n = 0; n < nMimCellNeighbours_; ++n)
        if (mimCellNeighbours_[n] == otherCell)
            return true;
    return false;
}

// Return list of all Cell neighbours
const std::vector<CellNeighbour> &Cell::allCellNeighbours() const { return allCellNeighbours_; }
