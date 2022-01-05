// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{false};
    // Range (min, max, delta) of distance axis
    Vec3<double> distanceRange_{0.0, 10.0, 0.05};
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
     * Processing
     */
    private:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool, KeywordSignals actionSignals) override;
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
