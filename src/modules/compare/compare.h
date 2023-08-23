// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"
#include "main/dissolve.h"
#include "math/data1D.h"
#include "math/data1DBase.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/error.h"
#include "math/sampledData1D.h"
#include "module/context.h"
#include "module/module.h"

// DataCompare Module
class CompareModule : public Module
{
    public:
    CompareModule();
    ~CompareModule() override = default;

    /*
     * Definition
     */
    private:
    // Preallocated vectors of data to compare
    std::vector<std::pair<std::unique_ptr<Data1D>, std::unique_ptr<Data1D>>> compareData1D_;
    std::vector<std::pair<std::unique_ptr<Data2D>, std::unique_ptr<Data2D>>> compareData2D_;
    std::vector<std::pair<std::unique_ptr<Data3D>, std::unique_ptr<Data3D>>> compareData3D_;
    // Method of error calculation to use
    Error::ErrorType errorType_{Error::EuclideanError};
    // Threshold for error metric above which test fails
    double threshold_{5.0e-3};

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
    // Add data to data vectors
    template <typename T> bool addData(std::vector<T> &data)
    {

        auto dataVectors = std::forward_as_tuple(compareData1D_, compareData2D_, compareData3D_);

        // Ensure provided data is consistent with required structure
        if (data.size() != 2)
        {
            return false;
        }

        // Set the data, getting the correct vector from the tuple based on the type
        std::get<std::vector<std::pair<std::unique_ptr<T>, std::unique_ptr<T>>> &>(dataVectors)
            .emplace_back(std::make_pair(std::make_unique<T>(data[0]), std::make_unique<T>(data[1])));

        return true;
    }
    // Return data to be read
    template <typename T> std::vector<T> getData()
    {
        std::vector<T> linearData;

        auto dataVector =
            std::get<std::vector<std::pair<T, T>> &>(std::forward_as_tuple(compareData1D_, compareData2D_, compareData3D_));

        for (auto &[data1, data2] : dataVector)
        {
            linearData.push_back(data1);
            linearData.push_back(data2);
        }

        return linearData;
    }

    // Fetch internal data
    template <typename T> bool fetchData(T &data, ModuleContext &moduleContext)
    {
        // If data is a child of Data1DBase
        if (std::is_convertible<T *, Data1DBase *>::value)
        {
            // Locate target data from tag and cast to base
            auto optData =
                moduleContext.dissolve().processingModuleData().searchBase<Data1DBase, Data1D, SampledData1D>(data.tag);
            if (!optData)
            {
                return Messenger::error("No data with tag '{}' exists.\n", data.tag);
            }
            // Fill object with located data
            data = optData->get();
            return true;
        }
        // Data2D, Data3D
        else
        {
            // Locate target data from tag and cast to base
            auto optData = moduleContext.dissolve().processingModuleData().search<const T>(data.tag);
            if (!optData)
            {
                return Messenger::error("No data with tag '{}' exists.\n", data.tag);
            }
            // Fill object with located data
            data = optData->get();
            return true;
        }
    }
};