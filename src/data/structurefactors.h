// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"

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
