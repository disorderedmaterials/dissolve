// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/changeData.h"
#include "base/messenger.h"
#include "classes/atom.h"
#include "classes/cell.h"
#include <cassert>

ChangeData::ChangeData() {}

/*
 * Target Data
 */

// Set target atom
void ChangeData::setAtom(AtomRef i)
{
    atom_ = i;
    moved_ = false;
    r_ = atom_.r();
    cell_ = i.cell();
}

// Return target Atom
AtomRef ChangeData::atom() { return atom_; }

// Return array index of stored Atom
int ChangeData::atomArrayIndex() const { return atom_.globalAtomIndex(); }

// Update local position, and flag as moved
void ChangeData::updatePosition()
{
    r_ = atom_.r();
    cell_ = atom_.cell();
    moved_ = true;
}

// Revert atom to stored position
void ChangeData::revertPosition()
{
    // Set stored position
    atom_.setCoordinates(r_);

    // If the cell changed with the move, revert that too
    if (cell_ != atom_.cell())
    {
        atom_.cell()->removeAtom(atom_);
        cell_->addAtom(atom_);
    }
}

// Return whether atom has moved
bool ChangeData::hasMoved() { return moved_; }

// Return position vector
Vec3<double> ChangeData::r() const { return r_; }
