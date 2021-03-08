// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "classes/data1dstore.h"
#include "classes/data2dstore.h"
#include "classes/data3dstore.h"
#include "module/module.h"

// Forward Declarations
/* none */

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
     * Initialisation
     */
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
     * Functions
     */
    private:
    // Test 1D datasets
    Data1DStore test1DData_;
    // Test 2D datasets
    Data2DStore test2DData_;
    // Test 3D datasets
    Data3DStore test3DData_;

    private:
    // Find reference Data
    template <class T>
    const OptionalReferenceWrapper<const T> findReferenceData(std::string_view dataIdentifier, Module *targetModule,
                                                              GenericList &processingModuleData)
    {
        // The 'dataIdentifier' is the actual name of the data (possibly with module prefix) - does it exist in
        // the target list?
        if (targetModule && processingModuleData.contains(dataIdentifier, targetModule->uniqueName()))
            return processingModuleData.value<T>(dataIdentifier, targetModule->uniqueName());
        else if (processingModuleData.contains(dataIdentifier))
            return processingModuleData.value<T>(dataIdentifier);

        // Failed to find data
        return {};
    }

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
