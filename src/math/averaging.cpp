// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/averaging.h"

// Return enum option info for AveragingScheme
EnumOptions<Averaging::AveragingScheme> &Averaging::averagingSchemes()
{
    static EnumOptionsList AveragingSchemeOptions = EnumOptionsList()
                                                    << EnumOption(Averaging::LinearAveraging, "Linear")
                                                    << EnumOption(Averaging::ExponentialAveraging, "Exponential");

    static EnumOptions<Averaging::AveragingScheme> options("AveragingScheme", AveragingSchemeOptions,
                                                           Averaging::LinearAveraging);

    return options;
}
