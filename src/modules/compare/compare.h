// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"
#include "math/data1D.h"
#include "math/data1DBase.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/sampledData1D.h"
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
    // Data pairs to compare
    std::vector<std::pair<Data1D, Data1D>> compareData1D_;
    std::vector<std::pair<Data2D, Data2D>> compareData2D_;
    std::vector<std::pair<Data3D, Data3D>> compareData3D_;
    // Threshold for error metric above which test fails
    double threshold_{5.0e-3};

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
    // Add data from provided arguements
    template <typename T> bool addData(std::vector<T> &data)
    {

        auto dataVectors = std::make_tuple(compareData1D_, compareData2D_, compareData3D_);

        // Ensure provided data is consistent with required structure
        if (data.size() != 2)
        {
            return false;
        }

        // Set the data, getting the correct vector based on the type
        &[ data1, data2 ] = std::get<std::vector<std::pair<T, T>>>(dataVectors).emplace_back(std::pair<T, T>());
        data1 = data[0];
        data2 = data[1];
    }
    // Fetch internal data
    template <typename T> bool fetchData(const T &data, const ModuleContext &ModuleContext)
    {
        // Locate target internal data
        auto optData = moduleContext.dissolve().processingModuleData().search<const T>(data.tag);
        if (!optData)
        {
            return Messenger::error("No data with tag '{}' exists.\n", tag);
        }
        // Fill object with located data
        data.initialise(optData->get());
        return true;
    }
    // Fetch internal 1D data
    template <> bool fetchData(const std::string_view &tag, const ModuleContext &ModuleContext)
    {
        // Locate target internal 1D data
        auto optData = moduleContext.dissolve().processingModuleData().searchBase<Data1DBase, Data1D, SampledData1D>(tag);
        if (!optData)
        {
            return Messenger::error("No data with tag '{}' exists.\n", tag);
        }
        // Fill object with located 1D data
        data.initialise(optData->get());
        return true;
    }
};