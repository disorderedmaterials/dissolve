// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class Collect1DProcedureNode;
class Collect2DProcedureNode;
class Collect3DProcedureNode;
class Process1DProcedureNode;
class Process2DProcedureNode;
class Process3DProcedureNode;
class SelectProcedureNode;
class SpeciesSite;

// Calculate Angle Module
class CalculateAngleModule : public Module
{
    public:
    CalculateAngleModule();
    ~CalculateAngleModule() override = default;

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
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool) override;
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * Functions / Data
     */
    private:
    // Analysis procedure to be run
    Procedure analyser_;
    // SelectNode for site A
    SelectProcedureNode *selectA_;
    // SelectNode for site B
    SelectProcedureNode *selectB_;
    // SelectNode for site C
    SelectProcedureNode *selectC_;
    // Collect1DNode for A-B RDF
    Collect1DProcedureNode *collectAB_;
    // Collect1DNode for B-C RDF
    Collect1DProcedureNode *collectBC_;
    // Collect1DNode for A-B-C angle histogram
    Collect1DProcedureNode *collectABC_;
    // Collect2DNode for (A-B)-C distance-angle data
    Collect2DProcedureNode *collectDAngleAB_;
    // Collect2DNode for A-(B-C) distance-angle data
    Collect2DProcedureNode *collectDAngleBC_;
    // Collect3DNode for A-B vs B-C vs A-B-C distance-distance-angle data
    Collect3DProcedureNode *collectDDA_;
    // Process1DNode for A-B RDF
    Process1DProcedureNode *processAB_;
    // Process1DNode for B-c RDF
    Process1DProcedureNode *processBC_;
    // Process1DNode for A-B-C angle histogram
    Process1DProcedureNode *processAngle_;
    // Process2DNode for (A-B)-C distance-angle data
    Process2DProcedureNode *processDAngleAB_;
    // Process2DNode for A-(B-C) distance-angle data
    Process2DProcedureNode *processDAngleBC_;
    // Process3DNode for A-B vs B-C vs A-B-C distance-distance-angle data
    Process3DProcedureNode *processDDA_;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
