// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "classes/data1dstore.h"
#include "classes/data2dstore.h"
#include "classes/data3dstore.h"
#include "classes/valuestore.h"
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
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const override;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;
    // Return category for module
    std::string_view category() const override;
    // Return brief description of module
    std::string_view brief() const override;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const override;

    /*
     * Control
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

    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
