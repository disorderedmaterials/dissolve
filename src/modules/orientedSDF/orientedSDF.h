// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/siteSelector.h"
#include "io/export/data3D.h"
#include "module/module.h"

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
    // Target SpeciesSite definitions
    std::vector<const SpeciesSite *> a_, b_;
    // Axes to use for sites
    OrientedSite::SiteAxis axisA_{OrientedSite::SiteAxis::XAxis}, axisB_{OrientedSite::SiteAxis::XAxis};
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
    // Export file and format for SDF
    Data3DExportFileFormat sdfFileAndFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
