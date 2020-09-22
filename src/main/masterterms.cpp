// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "main/dissolve.h"

// Return list of master Bond parameters
const List<MasterIntra> &Dissolve::masterBonds() const { return coreData_.masterBonds(); }

// Return list of master Angle parameters
const List<MasterIntra> &Dissolve::masterAngles() const { return coreData_.masterAngles(); }

// Return list of master Torsion parameters
const List<MasterIntra> &Dissolve::masterTorsions() const { return coreData_.masterTorsions(); }
