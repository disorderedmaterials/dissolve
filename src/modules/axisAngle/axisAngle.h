// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations

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

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
