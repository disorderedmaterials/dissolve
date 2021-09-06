// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/cell.h"
#include "classes/atom.h"
#include "classes/box.h"
#include <algorithm>

Cell::Cell(std::vector<Atom> *source, int index, Vec3<int> gridReference, Vec3<double> centre)
    : gridReference_(gridReference), index_(index), centre_(centre), source_(source)
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
const PackedRange<Atom> Cell::atoms() { return PackedRange<Atom>(*source_, atoms_); }
const ConstPackedRange<Atom> Cell::atoms() const { return ConstPackedRange<Atom>(*source_, atoms_); }

// Return number of Atoms in list
int Cell::nAtoms() const { return atoms_.size(); }

// Add atom to Cell
void Cell::addAtom(Atom &atom)
{
    atoms_.push_back(atom.arrayIndex() - 1);

    if (atom.cell())
        Messenger::warn("About to set Cell pointer in Atom {}, but this will overwrite an existing value.\n",
                        atom.arrayIndex());
    atom.setCell(this);
}

// Remove Atom from Cell
void Cell::removeAtom(Atom &atom)
{
    auto it = std::find(atoms_.begin(), atoms_.end(), atom.arrayIndex() - 1);
    assert(it != atoms_.end());
    (*source_)[*it].setCell(nullptr);
    atoms_.erase(it);
}
