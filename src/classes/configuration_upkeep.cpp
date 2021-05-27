// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/changestore.h"
#include "main/dissolve.h"

// Update Cell contents
void Configuration::updateCellContents()
{
    for (auto &i : atoms_)
        updateCellLocation(i.get());
}

// Update Cell location of specified Atom
void Configuration::updateCellLocation(Atom *i)
{
    // Fold Atom coordinates into Box
    i->setCoordinates(box_->fold(i->r()));

    // Determine new Cell position
    Cell *cell = cells_.cell(i->r());

    // Need to move?
    if (cell != i->cell())
    {
        if (i->cell())
            i->cell()->removeAtom(i);
        cell->addAtom(i);
    }
}

// Update Cell location of specified Molecule
void Configuration::updateCellLocation(const std::shared_ptr<Molecule> &mol)
{
    for (auto n = 0; n < mol->nAtoms(); ++n)
        updateCellLocation(mol->atom(n).get());
}

// Update Cell location of specified Atom indices
void Configuration::updateCellLocation(const std::vector<int> &targetAtoms, int indexOffset)
{
    for (const auto i : targetAtoms)
        updateCellLocation(atoms_[i + indexOffset].get());
}
