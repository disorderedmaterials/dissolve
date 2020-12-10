// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "data/structurefactors.h"

namespace StructureFactors
{

// Return enum option info for NormalisationType
EnumOptions<NormalisationType> normalisationTypes()
{
    static EnumOptionsList NormalisationTypeOptions = EnumOptionsList()
                                                      << EnumOption(NoNormalisation, "None")
                                                      << EnumOption(AverageOfSquaresNormalisation, "AverageOfSquares")
                                                      << EnumOption(SquareOfAverageNormalisation, "SquareOfAverage");

    static EnumOptions<NormalisationType> options("NormalisationType", NormalisationTypeOptions, NoNormalisation);

    return options;
}

} // namespace StructureFactors
