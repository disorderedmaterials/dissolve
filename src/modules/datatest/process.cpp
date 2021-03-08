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
    for (auto &[referenceData, format] : test1DData_.data())
    {
        // Locate the target reference data
        const auto optData =
            findReferenceData<const Data1D>(referenceData.tag(), targetModule, dissolve.processingModuleData());
        if (!optData)
        {
            if (targetModule)
                return Messenger::error("No data named '{}_{}' or '{}', or tagged '{}', exists.\n", targetModule->uniqueName(),
                                        referenceData.tag(), referenceData.tag(), referenceData.tag());
            else
                return Messenger::error("No data with name '{}' exists.\n", referenceData.tag());
        }
        const Data1D &data = *optData;
        Messenger::print("Located reference data '{}'.\n", referenceData.tag());

        // Generate the error estimate and compare against the threshold value
        double error = Error::error(errorType, data, referenceData, true);
        Messenger::print("Target data '{}' has error of {:7.3e} with reference data and is {} (threshold is {:6.3e})\n\n",
                         referenceData.tag(), error, isnan(error) || error > testThreshold ? "NOT OK" : "OK", testThreshold);
        if (isnan(error) || error > testThreshold)
            return false;
    }

    // Loop over reference two-dimensional data supplied
    for (auto &[referenceData, format] : test2DData_.data())
    {
        // Locate the target reference data
        const auto optData =
            findReferenceData<const Data2D>(referenceData.tag(), targetModule, dissolve.processingModuleData());
        if (!optData)
        {
            if (targetModule)
                return Messenger::error("No data named '{}_{}' or '{}', or tagged '{}', exists.\n", targetModule->uniqueName(),
                                        referenceData.tag(), referenceData.tag(), referenceData.tag());
            else
                return Messenger::error("No data with tag '{}' exists.\n", referenceData.tag());
        }
        const Data2D &data = *optData;
        Messenger::print("Located reference data '{}'.\n", referenceData.tag());

        // Generate the error estimate and compare against the threshold value
        // 		double error = Error::error(errorType, data, *testData2D, true);
        // 		Messenger::print("Target data '{}' has error of {:7.3f} with calculated data and is {} (threshold
        // is {:6.3e})\n\n", testData2D->name(), error, isnan(error) || error > testThreshold ? "NOT OK" : "OK", testThreshold);
        // if (isnan(error) || error > testThreshold) return false;

        return Messenger::error("Error calculation between 2D datasets is not yet implemented.\n");
    }

    return true;
}
