// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "modules/datatest/datatest.h"

// Run main processing
bool DataTestModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * This is a serial routine.
     */

    // Get options and target Module
    const auto testThreshold = keywords_.asDouble("Threshold");
    auto errorType = keywords_.enumeration<Error::ErrorType>("ErrorType");
    const auto &targets = keywords_.retrieve<std::vector<Module *>>("Target");
    auto *targetModule = targets.size() == 1 ? targets.front() : nullptr;

    // Print summary
    if (!targetModule)
        Messenger::print("DataTest: No target Module specified for data location - only tags will be searched.\n");
    else
        Messenger::print("DataTest: Target Module '{}' will be used as search prefix for data.\n", targetModule->uniqueName());
    Messenger::print("DataTest: Error calculation is '{}', threshold is {:e}.", Error::errorTypes().keyword(errorType),
                     testThreshold);
    Messenger::print("\n");

    // Loop over reference one-dimensional data supplied
    ListIterator<Data1D> data1DIterator(test1DData_.data());
    while (Data1D *testData1D = data1DIterator.iterate())
    {
        // Locate the target reference data
        const auto optData = findReferenceData<const Data1D>(testData1D->name(), targetModule, dissolve.processingModuleData());
        if (!optData)
        {
            if (targetModule)
                return Messenger::error("No data named '{}_{}' or '{}', or tagged '{}', exists.\n", targetModule->uniqueName(),
                                        testData1D->name(), testData1D->name(), testData1D->name());
            else
                return Messenger::error("No data with name '{}' exists.\n", testData1D->name());
        }
        const Data1D &data = *optData;
        Messenger::print("Located reference data with name '{}'.\n", data.objectTag());

        // Generate the error estimate and compare against the threshold value
        double error = Error::error(errorType, data, *testData1D, true);
        Messenger::print("Target data '{}' has error of {:7.3e} with calculated data and is {} (threshold is {:6.3e})\n\n",
                         testData1D->name(), error, isnan(error) || error > testThreshold ? "NOT OK" : "OK", testThreshold);
        if (isnan(error) || error > testThreshold)
            return false;
    }

    // Loop over reference two-dimensional data supplied
    ListIterator<Data2D> data2DIterator(test2DData_.data());
    while (Data2D *testData2D = data2DIterator.iterate())
    {
        // Locate the target reference data
        const auto optData = findReferenceData<const Data2D>(testData2D->name(), targetModule, dissolve.processingModuleData());
        if (!optData)
        {
            if (targetModule)
                return Messenger::error("No data named '{}_{}' or '{}', or tagged '{}', exists.\n", targetModule->uniqueName(),
                                        testData2D->name(), testData2D->name(), testData2D->name());
            else
                return Messenger::error("No data with tag '{}' exists.\n", testData2D->name());
        }
        const Data2D &data = *optData;
        Messenger::print("Located reference data with name '{}'.\n", data.objectTag());

        // Generate the error estimate and compare against the threshold value
        // 		double error = Error::error(errorType, data, *testData2D, true);
        // 		Messenger::print("Target data '{}' has error of {:7.3f} with calculated data and is {} (threshold
        // is {:6.3e})\n\n", testData2D->name(), error, isnan(error) || error > testThreshold ? "NOT OK" : "OK", testThreshold);
        // if (isnan(error) || error > testThreshold) return false;

        return Messenger::error("Error calculation between 2D datasets is not yet implemented.\n");
    }

    return true;
}
