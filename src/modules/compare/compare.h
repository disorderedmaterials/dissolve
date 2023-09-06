// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "io/import/data1D.h"
#include "io/import/data2D.h"
#include "io/import/data3D.h"
#include "keywords/dataSource.h"
#include "main/dissolve.h"
#include "math/data1D.h"
#include "math/data1DBase.h"
#include "math/data2D.h"
#include "math/data3D.h"
#include "math/error.h"
#include "math/range.h"
#include "math/sampledData1D.h"
#include "module/context.h"
#include "module/module.h"

using rangeErrorPair = std::pair<std::vector<Range>, std::vector<double>>;

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
    std::vector<DataSourceKeywordBase::DataPair> dataSources_;
    // Method of error calculation to use
    Error::ErrorType errorType_{Error::EuclideanError};
    // Threshold for error metric above which test fails
    double threshold_{5.0e-3};
    // Ranges to calculate error over
    std::vector<Range> ranges_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};