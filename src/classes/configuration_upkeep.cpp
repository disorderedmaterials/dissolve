// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/changestore.h"
#include "main/dissolve.h"

// Rationalise object relationships between atoms, molecules, and cells
void Configuration::updateObjectRelationships()
{
    for (auto &m : molecules_)
        m->updateAtoms(atoms_);

    updateAtomLocations(true);
}

// Update Cell location of all Atoms
void Configuration::updateAtomLocations(bool clearExistingLocations)
{
    if (clearExistingLocations)
    {
        for (auto &i : atoms_)
            i.setCell(nullptr);
        cells_.clearAtoms();
    }

    for (auto &i : atoms_)
        updateAtomLocation(&i);
}

// Update Cell location of specified Atom
void Configuration::updateAtomLocation(Atom *i)
{
    // Fold Atom coordinates into Box
    i->setCoordinates(box_->fold(i->r()));

    // Determine new Cell position
    auto *cell = cells_.cell(i->r());

    // Need to move?
    if (cell != i->cell())
    {
        if (i->cell())
            i->cell()->removeAtom(i);
        cell->addAtom(i);
    }
}

// Update Cell locations of atoms within the specified Molecule
void Configuration::updateAtomLocations(const std::shared_ptr<Molecule> &mol)
{
    for (auto n = 0; n < mol->nAtoms(); ++n)
        updateAtomLocation(mol->atom(n));
}

// Update Cell location of specified Atom indices
void Configuration::updateAtomLocations(const std::vector<int> &targetAtoms, int indexOffset)
{
    for (const auto i : targetAtoms)
        updateAtomLocation(&atoms_[i + indexOffset]);
}
