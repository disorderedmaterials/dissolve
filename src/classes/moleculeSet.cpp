// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/moleculeSet.h"

// Add a new molecule from the specified Species
LocalMolecule &MoleculeSet::addMolecule(const Species *species) { return localMolecules_.emplace_back(species); }

// Return nth local molecule, wrapping index if necessary
LocalMolecule &MoleculeSet::localMolecule(int n) { return localMolecules_[n % localMolecules_.size()]; }
const LocalMolecule &MoleculeSet::localMolecule(int n) const { return localMolecules_[n % localMolecules_.size()]; }

// Return local molecules
std::vector<LocalMolecule> &MoleculeSet::localMolecules() { return localMolecules_; }
const std::vector<LocalMolecule> &MoleculeSet::localMolecules() const { return localMolecules_; };
