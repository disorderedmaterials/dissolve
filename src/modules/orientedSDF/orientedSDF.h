// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/export/data3D.h"
#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class CalculateAxisAngleProcedureNode;
class Collect3DProcedureNode;
class IfValueInRangeProcedureNode;
class Process3DProcedureNode;
class SelectProcedureNode;

// CalculateSDF Module
class OrientedSDFModule : public Module
{
    public:
    OrientedSDFModule();
    ~OrientedSDFModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{true};
    // Range along X axis
    Vec3<double> rangeX_{-10.0, 10.0, 0.5};
    // Range along Y axis
    Vec3<double> rangeY_{-10.0, 10.0, 0.5};
    // Range along Z axis
    Vec3<double> rangeZ_{-10.0, 10.0, 0.5};
    // Axis angle range requirement to permit binning into SDF
    Range axisAngleRange_{-15.0, 15.0};
    // Whether the angular range should be considered symmetric about 90
    bool symmetric_{false};
    // Analysis procedure to be run
    Procedure analyser_;
    // SelectNode for site A (origin)
    std::shared_ptr<SelectProcedureNode> selectA_;
    // SelectNode for site B (surrounding)
    std::shared_ptr<SelectProcedureNode> selectB_;
    // CalculateAxisAngle node
    std::shared_ptr<CalculateAxisAngleProcedureNode> calculateAxisAngle_;
    // Axis value range check
    std::shared_ptr<IfValueInRangeProcedureNode> checkAxisValue_;
    // Collect3DNode for position of B around A SDF
    std::shared_ptr<Collect3DProcedureNode> collectVector_;
    // Process3DNode for B around A SDF
    std::shared_ptr<Process3DProcedureNode> processPosition_;
    // Export file and format for SDF
    Data3DExportFileFormat sdfFileAndFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
