// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/oplsaa2005/base.h"

OPLSAA2005BaseForcefield::OPLSAA2005BaseForcefield() = default;

OPLSAA2005BaseForcefield::~OPLSAA2005BaseForcefield() = default;

/*
 * Definition
 */

// Return short-range interaction style for AtomTypes
ShortRangeFunctions::Form OPLSAA2005BaseForcefield::shortRangeForm() const
{
    return ShortRangeFunctions::Form::LennardJonesGeometric;
}
