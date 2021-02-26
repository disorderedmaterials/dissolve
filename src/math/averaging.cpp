// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/averaging.h"

namespace Averaging
{

// Return enum option info for AveragingScheme
EnumOptions<Averaging::AveragingScheme> averagingSchemes()
{
    static EnumOptions<Averaging::AveragingScheme> options(
        "AveragingScheme", {{Averaging::LinearAveraging, "Linear"}, {Averaging::ExponentialAveraging, "Exponential"}},
        Averaging::LinearAveraging);

    return options;
}

// Establish the number of stored datasets, shift indices down, and lose oldest dataset if necessary
int pruneOldData(GenericList &moduleData, std::string_view name, std::string_view prefix, int nSetsInAverage)
{
    // Establish how many stored datasets we have
    int nStored = 0;
    for (nStored = 0; nStored < nSetsInAverage; ++nStored)
        if (!moduleData.contains(fmt::format("{}_{}", name, nStored + 1), prefix))
            break;
    Messenger::print("Average requested over {} datasets - {} available in module data ({} max).\n", nSetsInAverage, nStored,
                     nSetsInAverage - 1);

    // Remove the oldest dataset if it exists, and shuffle the others down
    if (nStored == nSetsInAverage)
    {
        moduleData.remove(fmt::format("{}_{}", name, nStored), prefix);
        --nStored;
    }
    for (auto n = nStored; n > 0; --n)
        moduleData.rename(fmt::format("{}_{}", name, n), prefix, fmt::format("{}_{}", name, n + 1), prefix);

    return nStored;
}

// Return exponential decay factor
double expDecay() { return 0.7; }

// Return the normalisation factor for the supplied scheme and number of data
double normalisationFactor(Averaging::AveragingScheme scheme, int nData)
{
    if (scheme == Averaging::LinearAveraging)
        return nData;
    else if (scheme == Averaging::ExponentialAveraging)
        return (1.0 - pow(expDecay(), nData)) / (1.0 - expDecay());

    return 1.0;
}

} // namespace Averaging
