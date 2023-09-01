// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "math/interpolator.h"
#include "modules/compare/compare.h"

Module::ExecutionResult CompareModule::process(ModuleContext &moduleContext)
{
    for (auto &[dataA, dataB] : *dataSources_.get())
    {
        /*if (!fetchData<Data1D>(*dataA, moduleContext) || !fetchData<Data1D>(*dataB, moduleContext))
            return ExecutionResult::Failed;

        std::vector<double> errors;

        for (auto &range : ranges_)
        {
            errors.push_back(Error::error(errorType_, *dataA, *dataB, range).error);
        }

        rangeError1D_.emplace_back(std::make_pair(std::ref(ranges_), errors));

        auto &delta = moduleContext.dissolve().processingModuleData().realise<Data1D>(
            fmt::format("Delta{}_{}//Compare", dataA.get()->tag(), dataB.get()->tag()), name_, GenericItem::InRestartFileFlag);

        // Get the minimum and maximum x values that exist over both datasets
        auto rangeMin = std::min(dataB.get()->xAxis().front(), dataA.get()->xAxis().front());
        auto rangeMax = std::max(dataB.get()->xAxis().back(), dataA.get()->xAxis().back());

        // Generate interpolation of dataB
        Interpolator interpolatedB(*dataB);

        for (auto &&[x, y] : zip(dataA.get()->xAxis(), dataA.get()->values()))
        {
            // Is our x value above the minimum range of each dataset ?
            if (x < rangeMin)
                continue;

            // Is our x value below the maximum range of each dataset ?
            if (x > rangeMax)
                break;

            delta.addPoint(x, fabs(y - interpolatedB.y(x)));
        }*/
    }

    return ExecutionResult::Success;
}
