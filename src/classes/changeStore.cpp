// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/changeStore.h"
#include "classes/atom.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include <memory>
#include <utility>

ChangeStore::ChangeStore() {}

/*
 * Watch Targets
 */

// Add atom to watch
void ChangeStore::add(Atom *i)
{
    targetAtoms_.emplace_back();
    targetAtoms_.back().setAtom(std::move(i));
}

// Add Molecule to watch
void ChangeStore::add(const std::shared_ptr<Molecule> &mol)
{
    for (auto *atom : mol->atoms())
        add(atom);
}

// Add Cell to watch
void ChangeStore::add(Cell *cell)
{
    for (auto &atom : cell->atoms())
        add(atom);
}

/*
 * Change Array
 */

// Reset ChangeStore
void ChangeStore::reset()
{
    targetAtoms_.clear();
    changes_.clear();
}

// Update all Atom positions
void ChangeStore::updateAll()
{
    std::for_each(targetAtoms_.begin(), targetAtoms_.end(), [](auto &item) { item.updatePosition(); });
}

// Update single atom position
void ChangeStore::updateAtom(int id)
{
    assert(id >= 0 && id < targetAtoms_.size());
    targetAtoms_[id].updatePosition();
}

// Revert all atoms to their previous positions
void ChangeStore::revertAll()
{
    for (auto &item : targetAtoms_)
        // revertPosition can make alterations to the cell that
        // contains the item, so it cannot be safely run in parallel.
        item.revertPosition();
}

// Revert specified index to stored position
void ChangeStore::revert(int id)
{
    assert(id >= 0 && id < targetAtoms_.size());
    targetAtoms_[id].revertPosition();
}

// Save Atom changes for broadcast, and reset arrays for new data
void ChangeStore::storeAndReset()
{
    for (auto item = targetAtoms_.begin(); item < targetAtoms_.end(); ++item)
    {
        // Has the position of this Atom been changed (i.e. updated)?
        if (item->hasMoved())
        {
            changes_.push_back(*item);
        }
    }

    // Clear target Atom data
    targetAtoms_.clear();
}

// Apply changes
bool ChangeStore::apply(Configuration *cfg)
{
    for (auto &data : changes_)
    {
        // Set new coordinates and check cell position (Configuration::updateAtomInCell() will do all this)
        data.revertPosition();
        cfg->updateAtomLocation(data.atom());
    }

    return true;
}
