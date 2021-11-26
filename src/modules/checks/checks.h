// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/geometry.h"
#include "module/module.h"

// Checks Module
class ChecksModule : public Module
{
    /*
     * Module for making simple checks
     */

    public:
    ChecksModule();
    ~ChecksModule() override = default;

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const override;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;
    // Return category for module
    std::string_view category() const override;
    // Return brief description of module
    std::string_view brief() const override;

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

    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
