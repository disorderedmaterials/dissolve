// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "classes/changedata.h"
#include "base/messenger.h"
#include "classes/atom.h"
#include "classes/cell.h"

ChangeData::ChangeData() : ListItem<ChangeData>()
{
    atom_ = nullptr;
    cell_ = nullptr;
    moved_ = false;
}

ChangeData::~ChangeData() {}

/*
 * Target Data
 */

// Set target atom
void ChangeData::setAtom(std::shared_ptr<Atom> i)
{
#ifdef CHECKS
    if (i == nullptr)
    {
        Messenger::print("NULL_POINTER - nullptr passed to ChangeData::setAtom().\n");
        return;
    }
#endif
    atom_ = i;
    moved_ = false;
    r_ = atom_->r();
    cell_ = i->cell();
}

// Return target Atom
std::shared_ptr<Atom> ChangeData::atom() { return atom_; }

// Return array index of stored Atom
int ChangeData::atomArrayIndex() const { return atom_->arrayIndex(); }

// Update local position, and flag as moved
void ChangeData::updatePosition()
{
    r_ = atom_->r();
    cell_ = atom_->cell();
    moved_ = true;
}

// Revert atom to stored position
void ChangeData::revertPosition()
{
    // Set stored position
    atom_->setCoordinates(r_);

    // If the cell changed with the move, revert that too
    if (cell_ != atom_->cell())
    {
        atom_->cell()->removeAtom(atom_);
        cell_->addAtom(atom_);
    }
}

// Return whether atom has moved
bool ChangeData::hasMoved() { return moved_; }

// Return position vector
Vec3<double> ChangeData::r() const { return r_; }
