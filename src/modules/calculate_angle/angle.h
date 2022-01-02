// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Whether to exclude correlations between A and B sites on the same molecule
    bool excludeSameMoleculeAB_{false};
    // Whether to exclude correlations between B and C sites on the same molecule
    bool excludeSameMoleculeBC_{false};
    // Whether to exclude correlations between A and C sites on the same molecule
    bool excludeSameSiteAC_{false};
    // Range (min, max, binwidth) of A-B distance axis
    Vec3<double> rangeAB_{0.0, 10.0, 0.05};
    // Range (min, max, binwidth) of B-C distance axis
    Vec3<double> rangeBC_{0.0, 10.0, 0.05};
    // Range (min, max, binwidth) of angle axis
    Vec3<double> angleRange_{0.0, 180.0, 1.0};
    // Analysis procedure to be run
    Procedure analyser_;
    // SelectNode for site A
    std::shared_ptr<SelectProcedureNode> selectA_;
    // SelectNode for site B
    std::shared_ptr<SelectProcedureNode> selectB_;
    // SelectNode for site C
    std::shared_ptr<SelectProcedureNode> selectC_;
    // Collect1DNode for A-B RDF
    std::shared_ptr<Collect1DProcedureNode> collectAB_;
    // Collect1DNode for B-C RDF
    std::shared_ptr<Collect1DProcedureNode> collectBC_;
    // Collect1DNode for A-B-C angle histogram
    std::shared_ptr<Collect1DProcedureNode> collectABC_;
    // Collect2DNode for (A-B)-C distance-angle data
    std::shared_ptr<Collect2DProcedureNode> collectDAngleAB_;
    // Collect2DNode for A-(B-C) distance-angle data
    std::shared_ptr<Collect2DProcedureNode> collectDAngleBC_;
    // Collect3DNode for A-B vs B-C vs A-B-C distance-distance-angle data
    std::shared_ptr<Collect3DProcedureNode> collectDDA_;
    // Process1DNode for A-B RDF
    std::shared_ptr<Process1DProcedureNode> processAB_;
    // Process1DNode for B-c RDF
    std::shared_ptr<Process1DProcedureNode> processBC_;
    // Process1DNode for A-B-C angle histogram
    std::shared_ptr<Process1DProcedureNode> processAngle_;
    // Process2DNode for (A-B)-C distance-angle data
    std::shared_ptr<Process2DProcedureNode> processDAngleAB_;
    // Process2DNode for A-(B-C) distance-angle data
    std::shared_ptr<Process2DProcedureNode> processDAngleBC_;
    // Process3DNode for A-B vs B-C vs A-B-C distance-distance-angle data
    std::shared_ptr<Process3DProcedureNode> processDDA_;

    /*
     * Processing
     */
    private:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool, KeywordSignals actionSignals) override;
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
