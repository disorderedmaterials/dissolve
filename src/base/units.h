// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"

// Units
class Units
{
    public:
    // Density Units
    enum DensityUnits
    {
        AtomsPerAngstromUnits,
        GramsPerCentimetreCubedUnits
    };
    // Return enum options for DensityUnits
    static EnumOptions<Units::DensityUnits> densityUnits();
};
