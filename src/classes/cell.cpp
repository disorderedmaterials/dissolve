// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/cell.h"
#include "classes/atom.h"
#include "classes/box.h"
#include <algorithm>

Cell::Cell(int index, Vec3<int> gridReference, Vec3<double> centre)
    : gridReference_(gridReference), index_(index), centre_(centre)
{
}

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
    cellNeighbours_.clear();
    cellNeighbours_.reserve(nearNeighbours.size());
    std::copy(nearNeighbours.begin(), nearNeighbours.end(), std::back_inserter(cellNeighbours_));

    // Create array of neighbours that require minimum image calculation
    mimCellNeighbours_.clear();
    mimCellNeighbours_.reserve(mimNeighbours.size());
    std::copy(mimNeighbours.begin(), mimNeighbours.end(), std::back_inserter(mimCellNeighbours_));

    // Create list of all cell neighbours
    allCellNeighbours_.clear();
    allCellNeighbours_.reserve(nearNeighbours.size() + mimNeighbours.size());
    std::copy(nearNeighbours.begin(), nearNeighbours.end(), std::back_inserter(allCellNeighbours_));
    std::copy(mimNeighbours.begin(), mimNeighbours.end(), std::back_inserter(allCellNeighbours_));
}

// Return adjacent Cell neighbour list
const std::vector<Cell *> &Cell::cellNeighbours() const { return cellNeighbours_; }

// Return list of Cell neighbours requiring minimum image calculation
const std::vector<Cell *> &Cell::mimCellNeighbours() const { return mimCellNeighbours_; }

// Return if the specified Cell requires minimum image calculation
bool Cell::mimRequired(const Cell *otherCell) const
{
    return (std::find(mimCellNeighbours_.begin(), mimCellNeighbours_.end(), otherCell) != mimCellNeighbours_.end());
}

// Return list of all Cell neighbours
const std::vector<Cell *> &Cell::allCellNeighbours() const { return allCellNeighbours_; }
