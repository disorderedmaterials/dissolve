// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/siteFilter.h"
#include "analyser/siteSelector.h"
#include "io/export/data1D.h"
#include "math/range.h"
#include "module/module.h"
#include "procedure/procedure.h"

// Calculate RDF Module
class SiteRDFModule : public Module
{
    public:
    SiteRDFModule();
    ~SiteRDFModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Target SpeciesSite definitions
    std::vector<const SpeciesSite *> a_, b_;
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{false};
    // Range (min, max, delta) of distance axis
    Vec3<double> distanceRange_{0.0, 10.0, 0.05};
    // Ranges for sums
    Range range_[3] = {{0.0, 3.0}, {3.0, 6.0}, {6.0, 9.0}};
    // Flags for ranges
    bool rangeEnabled_[3] = {true, false, false};
    // Whether to calculate the instantaneous coordination numbers rather than forming an average
    bool instantaneous_{false};
    // Whether to export instantaneous coordination numbers to disk
    bool exportInstantaneous_{false};
    // Export target
    Data1DExportFileFormat exportFileAndFormat_;
    bool export_{false};

    public:
    // Return whether specified coordination number range is enabled
    bool isRangeEnabled(int id) const;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
