// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "templates/refdatalist.h"

// Forward Declarations
/* none */

// Calibration Module
class CalibrationModule : public Module
{
    /*
     * Module for testing various functions
     */

    public:
    CalibrationModule();
    ~CalibrationModule() override = default;

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
    public:
    // IntraBroadening Fitting Targets
    enum IntraBroadeningFitTarget
    {
        IntraBroadeningTargetSQ,
        IntraBroadeningTargetGR,
        IntraBroadeningTargetBoth,
        nIntraBroadeningFitTargets
    };

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

// Interface Class for Complex Cost Functions
class CalibrationModuleCostFunctions
{
    public:
    CalibrationModuleCostFunctions(Dissolve &dissolve, ProcessPool &procPool, std::vector<Module *> &intraBroadeningModules,
                                   RefDataList<Module, CalibrationModule::IntraBroadeningFitTarget> &intraBroadeningReferences);

    private:
    // Dissolve Main Object
    Dissolve &dissolve_;
    // Target ProcessPool
    ProcessPool &processPool_;
    // RDFModule targets for IntraBroadening fitting
    std::vector<Module *> &intraBroadeningModules_;
    // NeutronSQModule targets for IntraBroadening fitting
    RefDataList<Module, CalibrationModule::IntraBroadeningFitTarget> &intraBroadeningReferences_;

    public:
    // Cost function for intraBroadening minimisation
    double intraBroadeningCost(const std::vector<double> &alpha);
};
