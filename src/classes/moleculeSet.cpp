// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/moleculeSet.h"

// Return nth local molecule
LocalMolecule &MoleculeSet::localMolecule(int n) { return localMolecules_[n]; }
const LocalMolecule &MoleculeSet::localMolecule(int n) const { return localMolecules_[n]; }

// Return local molecules
std::vector<LocalMolecule> &MoleculeSet::localMolecules() { return localMolecules_; }
const std::vector<LocalMolecule> &MoleculeSet::localMolecules() const { return localMolecules_; };
