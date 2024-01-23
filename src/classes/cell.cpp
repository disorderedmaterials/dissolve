// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/cell.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "templates/algorithms.h"
#include <algorithm>

Cell::Cell(int index, Vec3<int> gridReference, Vec3<double> centre)
    : gridReference_(gridReference), index_(index), centre_(centre)
{
}

/*
 * Identity
 */

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

// Return real-space Cell centre
const Vec3<double> &Cell::centre() const { return centre_; }

/*
 * Contents
 */

// Return vector of contained Atoms
std::vector<AtomRef> &Cell::atoms() { return atoms_; }
const std::vector<AtomRef> &Cell::atoms() const { return atoms_; }

// Return number of Atoms in list
int Cell::nAtoms() const { return atoms_.size(); }

// Add atom to Cell
void Cell::addAtom(AtomRef atom)
{
    assert(!atom->cell());

    atoms_.push_back(atom);
    atom->setCell(this);
}

// Remove Atom from Cell
void Cell::removeAtom(AtomRef atom)
{
    auto it = std::find(atoms_.begin(), atoms_.end(), atom);
    assert(it != atoms_.end());
    (*it)->setCell(nullptr);
    atoms_.erase(it);
}

// Clear all atoms from cell
void Cell::clearAtoms() { atoms_.clear(); }
