// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"
#include "keywords/dataSource.h"
#include "main/dissolve.h"
#include "math/data1D.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/error.h"
#include "math/range.h"
#include "math/sampledData1D.h"
#include "module/context.h"
#include "module/module.h"

using RangeErrorPair = std::pair<std::vector<Range>, std::vector<double>>;

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
    // Data stored in the keyword
    std::vector<DataSourceKeyword<Data1D>::DataPair> data1dSources_;
    // Method of error calculation to use
    Error::ErrorType errorType_{Error::EuclideanError};
    // Ranges to calculate error over
    std::vector<Range> ranges_;
    // Mapping from data pair to ranges and error
    std::map<const DataSourceKeyword<Data1D>::DataPair *, RangeErrorPair> data1dSourcesErrors_;

    public:
    // Return data sources
    const std::vector<DataSourceKeyword<Data1D>::DataPair> &data1dSources() const;
    // Return errors
    const std::map<const DataSourceKeyword<Data1D>::DataPair *, RangeErrorPair> &data1dSourcesErrors() const;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};