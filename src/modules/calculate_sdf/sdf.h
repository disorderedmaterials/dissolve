// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/export/data3d.h"
#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class Collect3DProcedureNode;
class Process3DProcedureNode;
class SelectProcedureNode;
class SpeciesSite;

// CalculateSDF Module
class CalculateSDFModule : public Module
{
    public:
    CalculateSDFModule();
    ~CalculateSDFModule() override = default;

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
     * Functions / Data
     */
    private:
    // Analysis procedure to be run
    Procedure analyser_;
    // SelectNode for site A (origin)
    SelectProcedureNode *selectA_;
    // SelectNode for site B (surrounding)
    SelectProcedureNode *selectB_;
    // Collect3DNode for position of B around A SDF
    Collect3DProcedureNode *collectVector_;
    // Process3DNode for B around A SDF
    Process3DProcedureNode *processPosition_;
    // Export file and format for SDF
    Data3DExportFileFormat sdfFileAndFormat_;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
