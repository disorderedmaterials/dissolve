// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "genericitems/list.h"

// Data Averaging
namespace Averaging
{

// Averaging Schemes
enum AveragingScheme
{
    LinearAveraging,
    ExponentialAveraging,
    nAveragingSchemes
};
// Return enum options for AveragingScheme
EnumOptions<Averaging::AveragingScheme> averagingSchemes();

// Establish the number of stored datasets, shift indices down, and lose oldest dataset if necessary
int pruneOldData(GenericList &processingData, std::string_view name, std::string_view prefix, int nSetsInAverage);
// Return exponential decay factor
double expDecay();

// Return the normalisation factor for the supplied scheme and number of data
double normalisationFactor(Averaging::AveragingScheme scheme, int nData);

// Perform averaging of named data
template <class T>
bool average(GenericList &processingData, std::string_view name, std::string_view prefix, int nSetsInAverage,
             AveragingScheme averagingScheme)
{
    // Find the 'root' data of type T, which should currently contain the most recently-calculated data
    if (!processingData.contains(name, prefix))
        return Messenger::error("Couldn't find root data '{}' (prefix = '{}') in order to perform averaging.\n", name, prefix);
    T &currentData = processingData.retrieve<T>(name, prefix);

    // Establish the number of existing datasets, and perform management on them
    int nData = pruneOldData(processingData, name, prefix, nSetsInAverage);

    // Store the current T as the earliest data (index == 1)
    T &recentData = processingData.realise<T>(fmt::format("{}_1", name), prefix, GenericList::InRestartFileFlag);
    recentData = currentData;
    ++nData;

    // Calculate normalisation factor
    double normalisation = normalisationFactor(averagingScheme, nData);

    // Perform averaging of the datsets that we have
    currentData.reset();
    double weight = 1.0;
    for (auto n = 0; n < nData; ++n)
    {
        // Get a copy of the (n+1)'th dataset
        T data = processingData.value<T>(fmt::format("{}_{}", name, n + 1), prefix);

        // Determine the weighting factor
        if (averagingScheme == Averaging::LinearAveraging)
            weight = 1.0 / normalisation;
        else if (averagingScheme == Averaging::ExponentialAveraging)
            weight = pow(expDecay(), n) / normalisation;

        // Weight the data
        data *= weight;

        // Sum in to the average
        currentData += data;
    }

    return true;
};

// Perform averaging of named array data
template <class T>
static bool arrayAverage(GenericList &processingData, std::string_view name, std::string_view prefix, int nSetsInAverage,
                         AveragingScheme averagingScheme)
{
    // Find the 'root' data of type T, which should currently contain the most recently-calculated data
    if (!processingData.contains(name, prefix))
    {
        Messenger::error("Couldn't find root data '{}' (prefix = '{}') in order to perform averaging.\n", name, prefix);
        return false;
    }
    T &currentData = processingData.retrieve<T>(name, prefix);

    // Establish the number of existing datasets, and perform management on them
    int nData = pruneOldData(processingData, name, prefix, nSetsInAverage);

    // Store the current T as the earliest data (index == 1)
    T &recentData = processingData.realise<T>(fmt::format("{}_1", name), prefix, GenericList::InRestartFileFlag);
    recentData = currentData;
    ++nData;

    // Calculate normalisation factor
    double normalisation = normalisationFactor(averagingScheme, nData);

    // Reset array elements
    for (auto i = 0; i < currentData.nItems(); ++i)
        currentData.at(i).reset();

    // Perform averaging of the datsets that we have
    double weight = 1.0;
    for (auto n = 0; n < nData; ++n)
    {
        // Get a copy of the (n+1)'th dataset
        T data = processingData.value<T>(fmt::format("{}_{}", name, n + 1), prefix);

        // Determine the weighting factor
        if (averagingScheme == Averaging::LinearAveraging)
            weight = 1.0 / normalisation;
        else if (averagingScheme == Averaging::ExponentialAveraging)
            weight = pow(expDecay(), n) / normalisation;

        // Loop over individual elements of the array
        for (auto i = 0; i < currentData.nItems(); ++i)
        {
            // Weight the data
            data.at(i) *= weight;

            // Sum in to the average
            currentData.at(i) += data.at(i);
        }
    }

    return true;
};

}; // namespace Averaging
