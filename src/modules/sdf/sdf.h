// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/export/data3D.h"
#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class Collect3DProcedureNode;
class Process3DProcedureNode;
class SelectProcedureNode;
class SpeciesSite;

// CalculateSDF Module
class SDFModule : public Module
{
    public:
    SDFModule();
    ~SDFModule() override = default;

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
    // Analysis procedure to be run
    Procedure analyser_;
    // SelectNode for site A (origin)
    std::shared_ptr<SelectProcedureNode> selectA_;
    // SelectNode for site B (surrounding)
    std::shared_ptr<SelectProcedureNode> selectB_;
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
