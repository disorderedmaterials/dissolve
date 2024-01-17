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
    // Number of coefficients used to define the empirical potential
    std::optional<int> nCoeffP_;
    // Radius at which potential truncation goes to zero (-1.0 to use pair potential maximum range)
    std::optional<double> rMaxPT_;
    // Radius at which potential truncation begins (-1.0 to set to 2.0 Angstroms under rmaxpt)
    std::optional<double> rMinPT_;
    // Width for Gaussian function in reciprocal space
    double gSigma1_{0.1};
    // Width for Gaussian function in real space
    double gSigma2_{0.2};

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
    bool generateEmpiricalPotentials(std::string_view modulename, GenericList &moduleData, Dissolve &dissolve,
                                     double averagedRho, std::optional<int> ncoeffp, double rminpt, double rmaxpt,
                                     double sigma1, double sigma2);
    void truncate(Data1D &data, double rMin, double rMax);

    public:
    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
