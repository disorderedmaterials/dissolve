// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/export/data1D.h"
#include "module/module.h"

// Calculate RDF Module
class IntraDistanceModule : public Module
{
    public:
    IntraDistanceModule();
    ~IntraDistanceModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Target SpeciesSite definitions
    std::vector<const SpeciesSite *> a_, b_;
    // Range (min, max, delta) of distance axis
    Vec3<double> distanceRange_{0.0, 10.0, 0.05};
    // Export target
    Data1DExportFileFormat exportFileAndFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
