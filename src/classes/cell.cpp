// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/cell.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/cellneighbour.h"
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
    assert(index_ == -1);

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
std::vector<std::shared_ptr<Atom>> &Cell::atoms() { return atoms_; }
const std::vector<std::shared_ptr<Atom>> &Cell::atoms() const { return atoms_; }

// Return number of Atoms in list
int Cell::nAtoms() const { return atoms_.size(); }

// Add atom to Cell
void Cell::addAtom(const std::shared_ptr<Atom> &atom)
{
    assert(atom);
    atoms_.push_back(atom);

    if (atom->cell())
        Messenger::warn("About to set Cell pointer in Atom {}, but this will overwrite an existing value.\n",
                        atom->arrayIndex());
    atom->setCell(this);
}

// Remove Atom from Cell
void Cell::removeAtom(const std::shared_ptr<Atom> &atom)
{
    auto it = std::find(atoms_.begin(), atoms_.end(), atom);
    assert(it != atoms_.end());
    (*it)->setCell(nullptr);
    atoms_.erase(it);
}

/*
 * Neighbours
 */

// Add Cell neighbours
void Cell::addCellNeighbours(std::vector<Cell *> &nearNeighbours, std::vector<Cell *> &mimNeighbours)
{
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
    std::vector<std::pair<Cell *, bool>> allCells;
    for (auto *nearNbr : nearNeighbours)
        allCells.emplace_back(nearNbr, false);
    for (auto *mimNbr : mimNeighbours)
        allCells.emplace_back(mimNbr, true);

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
