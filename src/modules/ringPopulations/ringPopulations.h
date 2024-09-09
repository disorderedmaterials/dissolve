// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/siteSelector.h"
#include "io/export/data1D.h"
#include "math/range.h"
#include "module/module.h"

// Calculate Q-Species Module
class RingPopulationsModule : public Module
{
    public:
    RingPopulationsModule();
    ~RingPopulationsModule() override = default;

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
    // Distance range for calculation
    Range distanceRange_{0.0, 2.5};
    // Export target
    Data1DExportFileFormat exportFileAndFormat_;
    bool export_{false};

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
