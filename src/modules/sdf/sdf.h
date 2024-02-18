// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/export/data3D.h"
#include "module/module.h"

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
    // Target SpeciesSite definitions
    std::vector<const SpeciesSite *> a_, b_;
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{true};
    // Range along X axis
    Vec3<double> rangeX_{-10.0, 10.0, 0.5};
    // Range along Y axis
    Vec3<double> rangeY_{-10.0, 10.0, 0.5};
    // Range along Z axis
    Vec3<double> rangeZ_{-10.0, 10.0, 0.5};
    // Export file and format for SDF
    Data3DExportFileFormat sdfFileAndFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
