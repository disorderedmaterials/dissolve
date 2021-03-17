// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/oplsaa2005/base.h"

OPLSAA2005BaseForcefield::OPLSAA2005BaseForcefield() {}

OPLSAA2005BaseForcefield::~OPLSAA2005BaseForcefield() {}

/*
 * Definition
 */

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType OPLSAA2005BaseForcefield::shortRangeType() const { return Forcefield::LennardJonesGeometricType; }
