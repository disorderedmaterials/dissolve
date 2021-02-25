// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/structurefactors.h"

namespace StructureFactors
{

// Return enum option info for NormalisationType
EnumOptions<NormalisationType> normalisationTypes()
{
    static EnumOptions<NormalisationType> options("NormalisationType",
                                                  {{NoNormalisation, "None"},
                                                   {AverageOfSquaresNormalisation, "AverageOfSquares"},
                                                   {SquareOfAverageNormalisation, "SquareOfAverage"}},
                                                  NoNormalisation);

    return options;
}

} // namespace StructureFactors
