// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"

// Return list of master Bond parameters
const std::vector<MasterIntra> &Dissolve::masterBonds() const { return coreData_.masterBonds(); }

// Return list of master Angle parameters
const std::vector<MasterIntra> &Dissolve::masterAngles() const { return coreData_.masterAngles(); }

// Return list of master Torsion parameters
const std::vector<MasterIntra> &Dissolve::masterTorsions() const { return coreData_.masterTorsions(); }

// Return list of master Improper parameters
const std::vector<MasterIntra> &Dissolve::masterImpropers() const { return coreData_.masterImpropers(); }
