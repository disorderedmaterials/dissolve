// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

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
    ~CalibrationModule();

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const;
    // Return category for module
    std::string_view category() const;
    // Return brief description of module
    std::string_view brief() const;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const;

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
    void initialise();

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool);

    /*
     * Members / Functions
     */
    private:
    // RDF Module targets for IntraBroadening calibration
    RefList<Module> intraBroadeningModules_;
    // NeutronSQ Module targets for IntraBroadening calibration (S(Q) data)
    RefList<Module> intraBroadeningNeutronSQReferences_;
    // NeutronSQ Module targets for IntraBroadening calibration (G(r) data)
    RefList<Module> intraBroadeningNeutronGRReferences_;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};

// Interface Class for Complex Cost Functions
class CalibrationModuleCostFunctions
{
    public:
    CalibrationModuleCostFunctions(Dissolve &dissolve, ProcessPool &procPool, RefList<Module> &intraBroadeningModules,
                                   RefDataList<Module, CalibrationModule::IntraBroadeningFitTarget> &intraBroadeningReferences);

    private:
    // Dissolve Main Object
    Dissolve &dissolve_;
    // Target ProcessPool
    ProcessPool &processPool_;
    // RDFModule targets for IntraBroadening fitting
    RefList<Module> &intraBroadeningModules_;
    // NeutronSQModule targets for IntraBroadening fitting
    RefDataList<Module, CalibrationModule::IntraBroadeningFitTarget> &intraBroadeningReferences_;

    public:
    // Cost function for intraBroadening minimisation
    double intraBroadeningCost(const Array<double> &alpha);
};
