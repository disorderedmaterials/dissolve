// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/structureFactors.h"

namespace StructureFactors
{

// Return enum option info for NormalisationType
EnumOptions<NormalisationType> normalisationTypes()
{
    return EnumOptions<NormalisationType>("NormalisationType", {{NoNormalisation, "None"},
                                                                {AverageOfSquaresNormalisation, "AverageOfSquares"},
                                                                {SquareOfAverageNormalisation, "SquareOfAverage"}});
}

} // namespace StructureFactors
