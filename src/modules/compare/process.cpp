// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/interpolator.h"
#include "modules/compare/compare.h"

Module::ExecutionResult CompareModule::process(ModuleContext &moduleContext)
{

    auto index = 1;
    for (auto &dataPair : data1dSources_)
    {
        /*
         * Preparing data
         */

        // Check to make sure the data exists
        if (!dataPair.first.dataExists() || !dataPair.second.dataExists())
        {
            continue;
        }

        // Set the ranges in the map to reference the ranges_ vector
        auto &ranges = data1dSourcesErrors_[&dataPair].first;
        ranges = ranges_;

        // Source the data
        if (!dataPair.first.sourceData(moduleContext.processPool(), moduleContext.dissolve().processingModuleData()))
        {
            continue;
        }
        if (!dataPair.second.sourceData(moduleContext.processPool(), moduleContext.dissolve().processingModuleData()))
        {
            continue;
        }

        auto dataA = dataPair.first.data();
        auto dataB = dataPair.second.data();

        /*
         * Save Data
         */

        auto &dataAStorage = moduleContext.dissolve().processingModuleData().realise<Data1D>(
            fmt::format("Pair{}_DataA", index), name_, GenericItem::InRestartFileFlag);
        auto &dataBStorage = moduleContext.dissolve().processingModuleData().realise<Data1D>(
            fmt::format("Pair{}_DataB", index), name_, GenericItem::InRestartFileFlag);

        dataAStorage = dataA;
        dataBStorage = dataB;

        /*
         * Calculating and reporting the error between datasets
         */

        // Getting the total data range and add it to the start of the range vector so it is always calculated as default
        Range totalRange{dataA.xAxis().front(), dataB.xAxis().back()};
        ranges.insert(ranges.begin(), totalRange);

        // Set the error vector to be the same size as the ranges
        data1dSourcesErrors_[&dataPair].second.resize(ranges.size());

        Messenger::print("Errors between {} and {}", dataPair.first.dataName(), dataPair.second.dataName());

        // Loop through the the range & error vectors, calculating and reporting the errors
        for (auto &&[range, error] : zip(ranges, data1dSourcesErrors_[&dataPair].second))
        {
            auto errorReport = Error::error(errorType_, dataA, dataB, range);
            error = errorReport.error;
            Messenger::print("     {}", Error::errorReportString(errorReport));
        }

        Messenger::print("\n");

        /*
         * Calculating the difference (delta) between datasets
         */

        auto &delta = moduleContext.dissolve().processingModuleData().realise<Data1D>(fmt::format("Pair{}_Delta", index), name_,
                                                                                      GenericItem::InRestartFileFlag);
        delta.clear();

        delta.clear();

        // Get the minimum and maximum x values that exist over both datasets
        auto rangeMin = std::min(dataA.xAxis().front(), dataB.xAxis().front());
        auto rangeMax = std::max(dataA.xAxis().back(), dataB.xAxis().back());

        // Generate interpolation of dataPair.second
        Interpolator interpolatedB(dataB);

        for (auto &&[x, y] : zip(dataA.xAxis(), dataA.values()))
        {
            // Is our x value above the minimum range of each dataset ?
            if (x < rangeMin)
                continue;

            // Is our x value below the maximum range of each dataset ?
            if (x > rangeMax)
                break;

            // Add difference as a point to delta Data object
            delta.addPoint(x, fabs(y - interpolatedB.y(x)));
        }

        ++index;
    }

    return ExecutionResult::Success;
}
