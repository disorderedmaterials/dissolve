// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class CalculateAngleProcedureNode;
class Collect1DProcedureNode;
class OperateExpressionProcedureNode;
class Process1DProcedureNode;
class SelectProcedureNode;
class SpeciesSite;

// Calculate Angle Module
class IntraAngleModule : public Module
{
    public:
    IntraAngleModule();
    ~IntraAngleModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Range (min, max, binwidth) of A-B distance axis
    Vec3<double> rangeAB_{0.0, 10.0, 0.05};
    // Range (min, max, binwidth) of B-C distance axis
    Vec3<double> rangeBC_{0.0, 10.0, 0.05};
    // Range (min, max, binwidth) of angle axis
    Vec3<double> angleRange_{0.0, 180.0, 1.0};
    // Whether the angular range should be considered symmetric about 90
    bool symmetric_{false};
    // Analysis procedure to be run
    Procedure analyser_;
    // SelectNode for site A
    std::shared_ptr<SelectProcedureNode> selectA_;
    // SelectNode for site B
    std::shared_ptr<SelectProcedureNode> selectB_;
    // SelectNode for site C
    std::shared_ptr<SelectProcedureNode> selectC_;
    // CalculateAngle node for A-B-C angle
    std::shared_ptr<CalculateAngleProcedureNode> calculateAngle_;
    // Collect1DNode for A-B-C angle histogram
    std::shared_ptr<Collect1DProcedureNode> collectABC_;
    // Process1DNode for A-B-C angle histogram
    std::shared_ptr<Process1DProcedureNode> processAngle_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
