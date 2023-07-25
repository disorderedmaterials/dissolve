// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "classes/data1DStore.h"
#include "classes/data2DStore.h"
#include "classes/data3DStore.h"
#include "classes/valueStore.h"
#include "io/import/values.h"
#include "math/error.h"
#include "module/module.h"

// DataTest Module
class DataTestModule : public Module
{
    public:
    DataTestModule();
    ~DataTestModule() override = default;

    /*
     * Definition
     */
    private:
    // Method of error calculation to use
    Error::ErrorType errorType_{Error::EuclideanError};
    // Internal 1D data testing
    std::vector<std::pair<std::string, std::string>> internal1DData_;
    // Test 1D datasets
    Data1DStore test1DData_;
    // Test 2D datasets
    Data2DStore test2DData_;
    // Test 3D datasets
    Data3DStore test3DData_;
    // Test SampledDouble values
    std::vector<std::pair<std::string, double>> testSampledDoubleData_;
    // Test SampledValue datasets
    ValueStore testSampledVectorData_;
    // Threshold for error metric above which test fails
    double threshold_{5.0e-3};

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(Dissolve &dissolve, const ProcessPool &procPool) override;
};
