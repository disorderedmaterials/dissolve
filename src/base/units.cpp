// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/units.h"

// Return enum option info for DensityUnits
EnumOptions<Units::DensityUnits> Units::densityUnits()
{
    return EnumOptions<Units::DensityUnits>(
        "DensityUnits", {{Units::AtomsPerAngstromUnits, "atoms/A3"}, {Units::GramsPerCentimetreCubedUnits, "g/cm3"}});
}
