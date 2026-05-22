// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/localMolecule.h"
#include "classes/speciesBond.h"

// Forward Declarations
class Species;

// Molecule Set Definition
class MoleculeSet
{
    public:
    MoleculeSet() = default;
    ~MoleculeSet() = default;

    private:
    // Local vector of molecules
    std::vector<LocalMolecule> localMolecules_;

    public:
    // Add a new molecule from the specified Species
    LocalMolecule &addMolecule(const Species *species);
    // Return nth local molecule, wrapping index if necessary
    LocalMolecule &localMolecule(int n);
    const LocalMolecule &localMolecule(int n) const;
    // Return local molecules
    std::vector<LocalMolecule> &localMolecules();
    const std::vector<LocalMolecule> &localMolecules() const;
};
