// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/geometry.h"
#include "module/module.h"

// Checks Module
class ChecksModule : public Module
{
    public:
    ChecksModule();
    ~ChecksModule() override = default;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;

    /*
     * Control
     */
    private:
    // Target configuration
    std::vector<Configuration *> targetConfigurations_;
    // Distances to check
    std::vector<Geometry> distances_;
    // Threshold at which distance checks will fail (Angstroms)
    double distanceThreshold_{0.001};
    // Angles to check
    std::vector<Geometry> angles_;
    // Threshold at which angle checks will fail (degrees)
    double angleThreshold_{0.05};

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
