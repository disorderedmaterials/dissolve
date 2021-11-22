// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class CalculateRDFModule;
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
     * Definition
     */
    private:
    // Source module for calculation
    const CalculateRDFModule *sourceRDF_{nullptr};
    // Reference coordination number for range A against which calculated value should be tested
    double testRangeA_{0.0};
    // Reference coordination number for range B against which calculated value should be tested
    double testRangeB_{0.0};
    // Reference coordination number for range C against which calculated value should be tested
    double testRangeC_{0.0};
    // Threshold difference at which test comparisons will fail
    double testThreshold_{0.1};
    // Analysis procedure to be run
    Procedure analyser_;
    // Sum1D node
    std::shared_ptr<Sum1DProcedureNode> sum1D_;
    // Process1D node
    std::shared_ptr<Process1DProcedureNode> process1D_;
    // Site normalisation node
    std::shared_ptr<OperateSitePopulationNormaliseProcedureNode> siteNormaliser_;

    public:
    // Return whether range B is enabled
    bool isRangeBEnabled() const;
    // Return whether range C is enabled
    bool isRangeCEnabled() const;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
