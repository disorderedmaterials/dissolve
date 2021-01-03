// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"

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
