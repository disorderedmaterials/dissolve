// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/range.h"
#include "module/module.h"
#include "io/export/data1D.h"

// Calculate Q-Species Module
class QSpeciesModule : public Module
{
    public:
    QSpeciesModule();
    ~QSpeciesModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Site definitions
    std::vector<const SpeciesSite *> bridgingOxygenSites_, networkFormerSites_;
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{false};
    // Distance range for calculation
    Range distanceRange_{0.0, 2.5};
    // Export target
    Data1DExportFileFormat exportFileFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
