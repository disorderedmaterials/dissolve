// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/site.h"
#include "io/export/data1D.h"
#include "io/export/data2D.h"
#include "module/module.h"

// Forward Declarations
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
    // Target SpeciesSite definitions
    std::vector<const SpeciesSite *> a_, b_;
    // Axes to use for sites
    OrientedSite::SiteAxis axisA_{OrientedSite::SiteAxis::XAxis}, axisB_{OrientedSite::SiteAxis::XAxis};
    // Export file and format for RDF
    Data1DExportFileFormat exportFileAndFormatRDF_;
    // Export file and format for AxisAngle
    Data1DExportFileFormat exportFileAndFormatAxisAngle_;
    // Export file and format for DAxisAngle
    Data2DExportFileFormat exportFileAndFormatDAxisAngle_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
