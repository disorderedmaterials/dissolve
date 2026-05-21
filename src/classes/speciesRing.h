// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include <vector>

// Forward Declarations
class BaseAtom;

// SpeciesRing Definition
class SpeciesRing
{
    public:
    SpeciesRing(const std::vector<const BaseAtom *> &atoms = {});
    ~SpeciesRing() = default;
    bool operator==(const SpeciesRing &other) const;

    /*
     * Atoms
     */
    private:
    // Array of atoms in the ring, in the order in which they appear
    std::vector<const BaseAtom *> atoms_;

    public:
    // Set atoms in ring
    void setAtoms(const std::vector<const BaseAtom *> &atoms);
    // Return nth atom in ring
    const BaseAtom *atom(int n) const;
    // Return array of atoms in ring
    const std::vector<const BaseAtom *> &atoms() const;
    // Return size of ring (number of atoms in array)
    int size() const;
    // Print ring information
    void print() const;
};
