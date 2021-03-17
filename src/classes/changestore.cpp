// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/changestore.h"
#include "base/processpool.h"
#include "classes/atom.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include <memory>

ChangeStore::ChangeStore(ProcessPool &procPool) : processPool_(procPool) {}

/*
 * Watch Targets
 */

// Add atom to watch
void ChangeStore::add(std::shared_ptr<Atom> i)
{
    targetAtoms_.emplace_back();
    targetAtoms_.back().setAtom(i);
}

// Add Molecule to watch
void ChangeStore::add(std::shared_ptr<Molecule> mol)
{
    for (auto n = 0; n < mol->nAtoms(); ++n)
        add(mol->atom(n));
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
            targetAtoms_.erase(item);
        }
    }

    // Clear target Atom data
    targetAtoms_.clear();
}

// Distribute and apply changes
bool ChangeStore::distributeAndApply(Configuration *cfg)
{
#ifdef PARALLEL
    // First, get total number of changes across all processes
    int nTotalChanges = changes_.size();
    if (!processPool_.allSum(&nTotalChanges, 1))
        return false;

    Messenger::printVerbose("We think there are {} changes in total to distribute.\n", nTotalChanges);

    // All processes now resize their arrays so they are large enough to hold the total number of changes
    if (nTotalChanges == 0)
        return true;
    x_.clear();
    x_.resize(nTotalChanges);
    y_.clear();
    y_.resize(nTotalChanges);
    z_.clear();
    z_.resize(nTotalChanges);
    indices_.clear();
    indices_.resize(nTotalChanges);

    // Copy local change data into arrays
    std::transform(changes_.begin(), changes_.end(), indices_.begin(), [](auto &change) { return change.atomArrayIndex(); });
    std::transform(changes_.begin(), changes_.end(), x_.begin(), [](auto &change) { return change.r().x; });
    std::transform(changes_.begin(), changes_.end(), y_.begin(), [](auto &change) { return change.r().y; });
    std::transform(changes_.begin(), changes_.end(), z_.begin(), [](auto &change) { return change.r().z; });

    // Now, assemble full array of the change data on the master...
    if (!processPool_.assemble(indices_.data(), changes_.size(), indices_.data(), nTotalChanges))
        return false;
    if (!processPool_.assemble(x_.data(), changes_.size(), x_.data(), nTotalChanges))
        return false;
    if (!processPool_.assemble(y_.data(), changes_.size(), y_.data(), nTotalChanges))
        return false;
    if (!processPool_.assemble(z_.data(), changes_.size(), z_.data(), nTotalChanges))
        return false;

    // ... then broadcast it to the slaves
    if (!processPool_.broadcast(indices_))
        return false;
    if (!processPool_.broadcast(x_))
        return false;
    if (!processPool_.broadcast(y_))
        return false;
    if (!processPool_.broadcast(z_))
        return false;

    // Apply atom changes
    std::vector<std::shared_ptr<Atom>> &atoms = cfg->atoms();
    for (auto n = 0; n < nTotalChanges; ++n)
    {
        assert(indices_[n] >= 0 && indices_[n] < cfg->nAtoms());

        // Set new coordinates and update cell position
        atoms[indices_[n]]->setCoordinates(x_[n], y_[n], z_[n]);
        cfg->updateCellLocation(atoms[indices_[n]]);
    }
#else
    // Apply atom changes
    for (auto &data : changes_)
    {
        // Set new coordinates and check cell position (Configuration::updateAtomInCell() will do all this)
        data.revertPosition();
        cfg->updateCellLocation(data.atom());
    }
#endif

    return true;
}
