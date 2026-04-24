// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/configurationAtom.h"
#include "classes/molecule.h"
#include <vector>

// Forward Declarations
class Species;

// Local Molecule Definition
class LocalMolecule : public Molecule
{
    public:
    LocalMolecule() = default;
    virtual ~LocalMolecule() = default;
    LocalMolecule(const Species *copyFrom);
    LocalMolecule(const LocalMolecule &copyFrom);
    LocalMolecule(LocalMolecule &&moveFrom);
    LocalMolecule &operator=(const LocalMolecule &copyFrom);
    LocalMolecule &operator=(LocalMolecule &&moveFrom);

    private:
    // Local vector of Atoms that belong to this Molecule and their original unit cell indices
    std::vector<ConfigurationAtom> localAtoms_;

    private:
    // Copy data from specified object
    void copyData(const LocalMolecule &object);
    // Add Atom to Molecule
    void addAtom(ConfigurationAtom *i) override;
    // Update local atom pointers from main vector
    void updateAtoms(std::vector<ConfigurationAtom> &mainAtoms, int offset) override;

    public:
    // Set Species that this Molecule represents
    void setSpecies(const Species *sp) override;
    // Return nth local atom
    ConfigurationAtom &localAtom(int n);
    const ConfigurationAtom &localAtom(int n) const;
    // Return local atoms
    std::vector<ConfigurationAtom> &localAtoms();
    const std::vector<ConfigurationAtom> &localAtoms() const;
};
