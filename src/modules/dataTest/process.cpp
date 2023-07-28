// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "keywords/vectorStringDouble.h"
#include "keywords/vectorStringPair.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "math/sampledData1D.h"
#include "modules/dataTest/dataTest.h"

// Run main processing
Module::ExecutionResult DataTestModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    /*
     * This is a serial routine.
     */

    // Print summary
    Messenger::print("DataTest: Error calculation is '{}', threshold is {:e}.", Error::errorTypes().keyword(errorType_),
                     threshold_);
    Messenger::print("\n");

    // Loop over reference one-dimensional data supplied
    for (auto sharedDataPointer : test1DData_.data())
    {
        auto &[referenceData, format] = *sharedDataPointer.get();
        // Locate the target reference data
        auto optData = dissolve.processingModuleData().searchBase<Data1DBase, Data1D, SampledData1D>(referenceData.tag());
        if (!optData)
        {
            Messenger::error("No data with tag '{}' exists.\n", referenceData.tag());
            return ExecutionResult::Failed;
        }
        const Data1D &data = optData->get();
        Messenger::print("Located reference data '{}'.\n", referenceData.tag());

        // Generate the error estimate and compare against the threshold value
        auto error = Error::error(errorType_, data, referenceData, true);
        auto notOK = isnan(error) || error > threshold_;
        Messenger::print("Target data '{}' has error of {:7.3e} with reference data and is {} (threshold is {:6.3e})\n\n",
                         referenceData.tag(), error, notOK ? "NOT OK" : "OK", threshold_);
        if (notOK)
        {
            return ExecutionResult::Failed;
        }
    }

    // Loop over internal one-dimensional data tests
    for (auto &[tag1, tag2] : internal1DData_)
    {
        // Locate the target reference datasets
        auto optData1 = dissolve.processingModuleData().searchBase<Data1DBase, Data1D, SampledData1D>(tag1);
        if (!optData1)
        {
            Messenger::error("No data with tag '{}' exists.\n", tag1);
            return ExecutionResult::Failed;
        }
        const Data1D data1 = optData1->get();
        Messenger::print("Located reference data '{}'.\n", tag1);
        auto optData2 = dissolve.processingModuleData().searchBase<Data1DBase, Data1D, SampledData1D>(tag2);
        if (!optData2)
        {
            Messenger::error("No data with tag '{}' exists.\n", tag2);
            return ExecutionResult::Failed;
        }
        const Data1D data2 = optData2->get();
        Messenger::print("Located reference data '{}'.\n", tag2);

        // Generate the error estimate and compare against the threshold value
        auto error = Error::error(errorType_, data1, data2, true);
        auto notOK = isnan(error) || error > threshold_;
        Messenger::print("Internal data '{}' has error of {:7.3e} with data '{}' and is {} (threshold is {:6.3e})\n\n", tag1,
                         error, tag2, notOK ? "NOT OK" : "OK", threshold_);
        if (notOK)
        {
            return ExecutionResult::Failed;
        }
    }

    // Loop over reference two-dimensional data supplied
    for (auto &sharedDataPointer : test2DData_.data())
    {
        auto &[referenceData, format] = *sharedDataPointer.get();
        // Locate the target reference data
        auto optData = dissolve.processingModuleData().search<const Data2D>(referenceData.tag());
        if (!optData)
        {
            Messenger::error("No data with tag '{}' exists.\n", referenceData.tag());
            return ExecutionResult::Failed;
        }
        const Data2D &data = *optData;
        Messenger::print("Located reference data '{}'.\n", referenceData.tag());

        // Generate the error estimate and compare against the threshold value
        // 		double error = Error::error(errorType_, data, *testData2D, true);
        // 		Messenger::print("Target data '{}' has error of {:7.3f} with calculated data and is {} (threshold
        // is {:6.3e})\n\n", testData2D->name(), error, isnan(error) || error > threshold_ ? "NOT OK" : "OK", threshold_);
        // if (isnan(error) || error > threshold_) return false;

        Messenger::error("Error calculation between 2D datasets is not yet implemented.\n");
        return ExecutionResult::Failed;
    }

    // Loop over reference three-dimensional data supplied
    for (auto &sharedDataPointer : test3DData_.data())
    {
        auto &[referenceData, format] = *sharedDataPointer.get();
        // Locate the target reference data
        auto optData = dissolve.processingModuleData().search<const Data3D>(referenceData.tag());
        if (!optData)
        {
            Messenger::error("No data with tag '{}' exists.\n", referenceData.tag());
            return ExecutionResult::Failed;
        }
        const Data3D &data = *optData;
        Messenger::print("Located reference data '{}'.\n", referenceData.tag());

        // Generate the error estimate and compare against the threshold value
        auto error = Error::error(errorType_, data.values().linearArray(), referenceData.values().linearArray(), true);
        auto notOK = isnan(error) || error > threshold_;
        Messenger::print("Target data '{}' has error of {:7.3f} with calculated data and is {} (threshold is {:6.3e})\n\n",
                         referenceData.tag(), error, notOK ? "NOT OK" : "OK", threshold_);
        if (notOK)
        {
            return ExecutionResult::Failed;
        }
    }

    // Loop over reference values supplied for SampledDouble objects
    for (auto &[tag, value] : testSampledDoubleData_)
    {
        // Locate the target reference data
        auto optData = dissolve.processingModuleData().search<const SampledDouble>(tag);
        if (!optData)
        {
            Messenger::error("No data with tag '{}' exists.\n", tag);
            return ExecutionResult::Failed;
        }
        const SampledDouble &data = optData->get();
        Messenger::print("Located reference data '{}'.\n", tag);

        // Generate the error estimate and compare against the threshold value
        auto error = fabs(value - data.value());
        auto notOK = isnan(error) || error > threshold_;
        Messenger::print("Target data '{}' has error of {:7.3e} with reference data and is {} (threshold is {:6.3e})\n\n", tag,
                         error, notOK ? "NOT OK" : "OK", threshold_);
        if (notOK)
        {
            return ExecutionResult::Failed;
        }
    }

    // Loop over reference values supplied for SampledVector objects
    for (auto &[tag, referenceData, format] : testSampledVectorData_.data())
    {
        // Locate the target reference data
        auto optData = dissolve.processingModuleData().search<const SampledVector>(tag);
        if (!optData)
        {
            Messenger::error("No data with tag '{}' exists.\n", tag);
            return ExecutionResult::Failed;
        }
        const SampledVector &data = optData->get();
        Messenger::print("Located reference data '{}'.\n", tag);

        // Generate the error estimate and compare against the threshold value
        auto error = Error::error(errorType_, data.values(), referenceData, true);
        auto notOK = isnan(error) || error > threshold_;
        Messenger::print("Target data '{}' has error of {:7.3e} with reference data and is {} (threshold is {:6.3e})\n\n", tag,
                         error, notOK ? "NOT OK" : "OK", threshold_);
        if (notOK)
        {
            return ExecutionResult::Failed;
        }
    }

    return ExecutionResult::Success;
}