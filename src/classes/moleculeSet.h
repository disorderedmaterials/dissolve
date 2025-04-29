// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/localMolecule.h"

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
    // Return nth local molecule
    LocalMolecule &localMolecule(int n);
    const LocalMolecule &localMolecule(int n) const;
    // Return local molecules
    std::vector<LocalMolecule> &localMolecules();
    const std::vector<LocalMolecule> &localMolecules() const;
};
