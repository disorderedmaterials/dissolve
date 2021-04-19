// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "math/sampleddata1d.h"
#include "modules/datatest/datatest.h"

// Run main processing
bool DataTestModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * This is a serial routine.
     */

    // Get options
    const auto testThreshold = keywords_.asDouble("Threshold");
    auto errorType = keywords_.enumeration<Error::ErrorType>("ErrorType");

    // Print summary
    Messenger::print("DataTest: Error calculation is '{}', threshold is {:e}.", Error::errorTypes().keyword(errorType),
                     testThreshold);
    Messenger::print("\n");

    // Loop over reference one-dimensional data supplied
    for (auto &[referenceData, format] : test1DData_.data())
    {
        // Locate the target reference data
        auto optData = dissolve.processingModuleData().searchBase<Data1DBase, Data1D, SampledData1D>(referenceData.tag());
        if (!optData)
            return Messenger::error("No data with tag '{}' exists.\n", referenceData.tag());
        const Data1D &data = optData->get();
        Messenger::print("Located reference data '{}'.\n", referenceData.tag());

        // Generate the error estimate and compare against the threshold value
        double error = Error::error(errorType, data, referenceData, true);
        Messenger::print("Target data '{}' has error of {:7.3e} with reference data and is {} (threshold is {:6.3e})\n\n",
                         referenceData.tag(), error, isnan(error) || error > testThreshold ? "NOT OK" : "OK", testThreshold);
        if (isnan(error) || error > testThreshold)
            return false;
    }

    // Loop over internal one-dimensional data tests
    for (auto &[tag1, tag2] : internal1DData_)
    {
        // Locate the target reference datasets
        auto optData1 = dissolve.processingModuleData().searchBase<Data1DBase, Data1D, SampledData1D>(tag1);
        if (!optData1)
            return Messenger::error("No data with tag '{}' exists.\n", tag1);
        const Data1D data1 = optData1->get();
        Messenger::print("Located reference data '{}'.\n", tag1);
        auto optData2 = dissolve.processingModuleData().searchBase<Data1DBase, Data1D, SampledData1D>(tag2);
        if (!optData2)
            return Messenger::error("No data with tag '{}' exists.\n", tag2);
        const Data1D data2 = optData2->get();
        Messenger::print("Located reference data '{}'.\n", tag2);

        // Generate the error estimate and compare against the threshold value
        double error = Error::error(errorType, data1, data2, true);
        Messenger::print("Internal data '{}' has error of {:7.3e} with data '{}' and is {} (threshold is {:6.3e})\n\n", tag1,
                         error, tag2, isnan(error) || error > testThreshold ? "NOT OK" : "OK", testThreshold);
        if (isnan(error) || error > testThreshold)
            return false;
    }

    // Loop over reference two-dimensional data supplied
    for (auto &[referenceData, format] : test2DData_.data())
    {
        // Locate the target reference data
        auto optData = dissolve.processingModuleData().search<const Data2D>(referenceData.tag());
        if (!optData)
            return Messenger::error("No data with tag '{}' exists.\n", referenceData.tag());
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
