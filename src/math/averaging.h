/*
    *** Averaging
    *** src/math/averaging.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "base/enumoptions.h"
#include "genericitems/listhelper.h"

// Forward Declarations
/* none */

// Data Averaging
class Averaging
{
    public:
    // Averaging Schemes
    enum AveragingScheme
    {
        LinearAveraging,
        ExponentialAveraging,
        nAveragingSchemes
    };
    // Return enum options for AveragingScheme
    static EnumOptions<Averaging::AveragingScheme> &averagingSchemes();

    private:
    // Establish the number of stored datasets, shift indices down, and lose oldest dataset if necessary
    static int pruneOldData(GenericList &moduleData, std::string_view name, std::string_view prefix, int nSetsInAverage)
    {
        // Establish how many stored datasets we have
        int nStored = 0;
        for (nStored = 0; nStored < nSetsInAverage; ++nStored)
            if (!moduleData.contains(fmt::format("{}_{}", name, nStored + 1), prefix))
                break;
        Messenger::print("Average requested over {} datsets - {} available in module data ({} max).\n", nSetsInAverage, nStored,
                         nSetsInAverage - 1);

        // Remove the oldest dataset if it exists, and shuffle the others down
        if (nStored == nSetsInAverage)
        {
            moduleData.remove(fmt::format("{}_{}", name, nStored), prefix);
            --nStored;
        }
        for (int n = nStored; n > 0; --n)
            moduleData.rename(fmt::format("{}_{}", name, n), prefix, fmt::format("{}_{}", name, n + 1), prefix);

        return nStored;
    }
    // Return exponential decay factor
    static double expDecay() { return 0.7; }
    // Return the normalisation factor for the supplied scheme and number of data
    static double normalisationFactor(Averaging::AveragingScheme scheme, int nData)
    {
        if (scheme == Averaging::LinearAveraging)
            return nData;
        else if (scheme == Averaging::ExponentialAveraging)
            return (1.0 - pow(expDecay(), nData)) / (1.0 - expDecay());

        return 1.0;
    }

    public:
    // Perform averaging of named data
    template <class T>
    static bool average(GenericList &moduleData, std::string_view name, std::string_view prefix, int nSetsInAverage,
                        AveragingScheme averagingScheme)
    {
        // Find the 'root' data of type T, which should currently contain the most recently-calculated data
        if (!moduleData.contains(name, prefix))
        {
            Messenger::error("Couldn't find root data '{}' (prefix = '{}') in order to perform averaging.\n", name, prefix);
            return false;
        }
        T &currentData = GenericListHelper<T>::retrieve(moduleData, name, prefix);

        // Establish the number of existing datasets, and perform management on them
        int nData = pruneOldData(moduleData, name, prefix, nSetsInAverage);

        // Store the current T as the earliest data (index == 1)
        T &recentData =
            GenericListHelper<T>::realise(moduleData, fmt::format("{}_1", name), prefix, GenericItem::InRestartFileFlag);
        recentData = currentData;
        ++nData;

        // Calculate normalisation factor
        double normalisation = normalisationFactor(averagingScheme, nData);

        // Perform averaging of the datsets that we have
        currentData.reset();
        double weight = 1.0;
        for (int n = 0; n < nData; ++n)
        {
            // Get a copy of the (n+1)'th dataset
            T data = GenericListHelper<T>::value(moduleData, fmt::format("{}_{}", name, n + 1), prefix);

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
    static bool arrayAverage(GenericList &moduleData, std::string_view name, std::string_view prefix, int nSetsInAverage,
                             AveragingScheme averagingScheme)
    {
        // Find the 'root' data of type T, which should currently contain the most recently-calculated data
        if (!moduleData.contains(name, prefix))
        {
            Messenger::error("Couldn't find root data '{}' (prefix = '{}') in order to perform averaging.\n", name, prefix);
            return false;
        }
        T &currentData = GenericListHelper<T>::retrieve(moduleData, name, prefix);

        // Establish the number of existing datasets, and perform management on them
        int nData = pruneOldData(moduleData, name, prefix, nSetsInAverage);

        // Store the current T as the earliest data (index == 1)
        T &recentData =
            GenericListHelper<T>::realise(moduleData, fmt::format("{}_1", name), prefix, GenericItem::InRestartFileFlag);
        recentData = currentData;
        ++nData;

        // Calculate normalisation factor
        double normalisation = normalisationFactor(averagingScheme, nData);

        // Reset array elements
        for (int i = 0; i < currentData.nItems(); ++i)
            currentData.at(i).reset();

        // Perform averaging of the datsets that we have
        double weight = 1.0;
        for (int n = 0; n < nData; ++n)
        {
            // Get a copy of the (n+1)'th dataset
            T data = GenericListHelper<T>::value(moduleData, fmt::format("{}_{}", name, n + 1), prefix);

            // Determine the weighting factor
            if (averagingScheme == Averaging::LinearAveraging)
                weight = 1.0 / normalisation;
            else if (averagingScheme == Averaging::ExponentialAveraging)
                weight = pow(expDecay(), n) / normalisation;

            // Loop over individual elements of the array
            for (int i = 0; i < currentData.nItems(); ++i)
            {
                // Weight the data
                data.at(i) *= weight;

                // Sum in to the average
                currentData.at(i) += data.at(i);
            }
        }

        return true;
    };
};
