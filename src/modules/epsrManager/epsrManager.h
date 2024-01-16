// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/scatteringMatrix.h"
#include "module/module.h"
#include "procedure/procedure.h"

// EPSR Manager Module
class EPSRManagerModule : public Module
{
    public:
    EPSRManagerModule();
    ~EPSRManagerModule() override = default;

    /*
     * Definition
     */
    public:
    enum ExpansionFunctionType
    {
        PoissonExpansionFunction,  /* Fit difference functions using Poisson (power exponential) functions */
        GaussianExpansionFunction, /* Fit difference functions using Gaussian functions */
        nExpansionFunctionTypes
    };

    private:
    // Target Module containing data to refine against
    std::vector<Module *> target_;
    // Scattering matrix
    ScatteringMatrix scatteringMatrix_;
    // Expansion function type to use for potential fits
    EPSRManagerModule::ExpansionFunctionType expansionFunction_{EPSRManagerModule::PoissonExpansionFunction};

    public:
    /*
     * Functions
     */
    bool generateEmpiricalPotentials(GenericList &moduleData, Dissolve &dissolve, double averagedRho,
                                     std::optional<int> ncoeffp, double rminpt, double rmaxpt, double sigma1, double sigma2);
    void truncate(Data1D &data, double rMin, double rMax);

    public:
    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
