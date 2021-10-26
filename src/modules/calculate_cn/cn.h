// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class Process1DProcedureNode;
class Sum1DProcedureNode;
class OperateSitePopulationNormaliseProcedureNode;

// CalculateCN Module
class CalculateCNModule : public Module
{
    public:
    CalculateCNModule();
    ~CalculateCNModule() override = default;

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
    // Reference coordination number for range A against which calculated value should be tested
    double testRangeA_{0.0};
    // Reference coordination number for range B against which calculated value should be tested
    double testRangeB_{0.0};
    // Reference coordination number for range C against which calculated value should be tested
    double testRangeC_{0.0};
    // Threshold difference at which test comparisons will fail
    double testThreshold_{0.1};

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
     * Functions / Data
     */
    private:
    // Analysis procedure to be run
    Procedure analyser_;
    // Sum1D node
    Sum1DProcedureNode *sum1D_;
    // Process1D node
    Process1DProcedureNode *process1D_;
    // Site normalisation node
    OperateSitePopulationNormaliseProcedureNode *siteNormaliser_;

    public:
    // Return whether range B is enabled
    bool isRangeBEnabled() const;
    // Return whether range C is enabled
    bool isRangeCEnabled() const;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
