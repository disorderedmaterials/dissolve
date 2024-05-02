// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/molecule.h"
#include <vector>

// Forward Declarations
class Species;

// Local Molecule Definition
class LocalMolecule : public Molecule
{
    public:
    LocalMolecule() = default;
    ~LocalMolecule() = default;
    LocalMolecule(const LocalMolecule &copyFrom);
    LocalMolecule(LocalMolecule &&moveFrom);
    LocalMolecule &operator=(const LocalMolecule &copyFrom);
    LocalMolecule &operator=(LocalMolecule &&moveFrom);

    private:
    // Local vector of Atoms that belong to this Molecule and their original unit cell indices
    std::vector<Atom> localAtoms_;

    private:
    // Copy data from specified object
    void copyData(const LocalMolecule &object);
    // Add Atom to Molecule
    void addAtom(Atom *i) override;
    // Update local atom pointers from main vector
    void updateAtoms(std::vector<Atom> &mainAtoms, int offset) override;

    public:
    // Set Species that this Molecule represents
    void setSpecies(const Species *sp) override;
    // Return nth local atom
    Atom &localAtom(int n);
    const Atom &localAtom(int n) const;
    // Return local atoms
    std::vector<Atom> &localAtoms();
    const std::vector<Atom> &localAtoms() const;
};
