// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class CalculateAxisAngleProcedureNode;
class Collect1DProcedureNode;
class Collect2DProcedureNode;
class OperateExpressionProcedureNode;
class Process1DProcedureNode;
class Process2DProcedureNode;
class SelectProcedureNode;
class SpeciesSite;

// Calculate Axis Angle Module
class AxisAngleModule : public Module
{
    public:
    AxisAngleModule();
    ~AxisAngleModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Whether to exclude correlations between B and C sites on the same molecule
    bool excludeSameMolecule_{false};
    // Range (min, max, binwidth) of distance axis
    Vec3<double> distanceRange_{0.0, 10.0, 0.1};
    // Range (min, max, binwidth) of angle axis
    Vec3<double> angleRange_{0.0, 180.0, 10.0};
    // Whether the angular range should be considered symmetric about 90
    bool symmetric_{false};
    // Analysis procedure to be run
    Procedure analyser_;
    // SelectNode for site A
    std::shared_ptr<SelectProcedureNode> selectA_;
    // SelectNode for site B
    std::shared_ptr<SelectProcedureNode> selectB_;
    // CalculateAxisAngle node
    std::shared_ptr<CalculateAxisAngleProcedureNode> calculateAxisAngle_;
    // Collect1DNode for A-B RDF
    std::shared_ptr<Collect1DProcedureNode> collectDistance_;
    // Collect1DNode for A-B angle histogram
    std::shared_ptr<Collect1DProcedureNode> collectAngle_;
    // Collect2DNode for distance-angle data
    std::shared_ptr<Collect2DProcedureNode> collectDAngle_;
    // Process1DNode for A-B RDF
    std::shared_ptr<Process1DProcedureNode> processDistance_;
    // Process1DNode for A-B-C angle histogram
    std::shared_ptr<Process1DProcedureNode> processAngle_;
    // Process2DNode for distance-angle data
    std::shared_ptr<Process2DProcedureNode> processDAngle_;
    // Normalisation expression for distance-angle map
    std::shared_ptr<OperateExpressionProcedureNode> dAngleNormalisationExpression_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(const ModuleContext& moduleContext) override;
};
