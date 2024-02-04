// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/export/data1D.h"
#include "math/range.h"
#include "module/module.h"
#include "procedure/procedure.h"

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
    // Target SpeciesSite definitions
    std::vector<const SpeciesSite *> a_, b_, c_;
    // Range of A-B distance axis
    Range rangeAB_{0.0, 10.0};
    // Range of B-C distance axis
    Range rangeBC_{0.0, 10.0};
    // Range (min, max, binwidth) of angle axis
    Vec3<double> angleRange_{0.0, 180.0, 1.0};
    // Whether the angular range should be considered symmetric about 90
    bool symmetric_{false};
    // Export target
    Data1DExportFileFormat exportFileAndFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
