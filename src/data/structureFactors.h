// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"

namespace StructureFactors
{
// Normalisation Type enum
enum NormalisationType
{
    NoNormalisation,
    AverageOfSquaresNormalisation,
    SquareOfAverageNormalisation,
    nNormalisationTypes
};
// Return enum option info for NormalisationType
EnumOptions<NormalisationType> normalisationTypes();
} // namespace StructureFactors
