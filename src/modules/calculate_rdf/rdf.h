// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class Collect1DProcedureNode;
class Process1DProcedureNode;
class SelectProcedureNode;
class SpeciesSite;

// Calculate RDF Module
class CalculateRDFModule : public Module
{
    public:
    CalculateRDFModule();
    ~CalculateRDFModule() override = default;

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

    /*
     * Control
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_;
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{false};
    // Range (min, max, delta) of distance axis
    Vec3<double> distanceRange_{0.0, 10.0, 0.05};

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
    std::shared_ptr<SelectProcedureNode> selectA_;
    // SelectNode for site B
    std::shared_ptr<SelectProcedureNode> selectB_;
    // Collect1DNode for A-B RDF
    std::shared_ptr<Collect1DProcedureNode> collectDistance_;
    // Process1DNode for A-B RDF
    std::shared_ptr<Process1DProcedureNode> processDistance_;

    public:
    // Return Collect1DNode for A-B RDF
    std::shared_ptr<Collect1DProcedureNode> collectDistanceNode() const;
    // Return SelectNode for site A
    std::shared_ptr<SelectProcedureNode> selectANode() const;
    // Return Process1DNode result (i.e. RDF)
    std::shared_ptr<Process1DProcedureNode> rdfResult() const;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
