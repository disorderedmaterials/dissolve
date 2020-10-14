// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

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
    ~CalculateCNModule();

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
    // Return specified coordination number (from Sum1D node)
    const SampledDouble &coordinationNumber(int index);
    // Return whether range B is enabled
    bool isRangeBEnabled() const;
    // Return whether range C is enabled
    bool isRangeCEnabled() const;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
