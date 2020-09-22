// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "templates/list.h"
#include "templates/listitem.h"
#include "templates/orderedpointerdataarray.h"
#include "templates/reflist.h"
#include "templates/vector3.h"
#include <vector>

// Forward Declarations
class SpeciesAtom;

// SpeciesRing Definition
class SpeciesRing : public ListItem<SpeciesRing>
{
    public:
    SpeciesRing();
    ~SpeciesRing();

    /*
     * Atoms
     */
    private:
    // Array of atoms in the ring, in the order in which they appear
    std::vector<const SpeciesAtom *> atoms_;

    public:
    // Set atoms in ring
    void setAtoms(const std::vector<const SpeciesAtom *> &atoms);
    // Return nth atom in ring
    const SpeciesAtom *atom(int n) const;
    // Return array of atoms in ring
    const std::vector<const SpeciesAtom *> &atoms() const;
    // Return size of ring (number of atoms in array)
    int size() const;
    // Print ring information
    void print() const;

    /*
     * Comparison
     */
    public:
    bool operator==(const SpeciesRing &other);
};
