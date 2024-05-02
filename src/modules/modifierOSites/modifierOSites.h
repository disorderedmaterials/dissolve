// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "analyser/siteSelector.h"
#include "io/export/data1D.h"
#include "math/range.h"
#include "module/module.h"

// ModifierOSites Module
class ModifierOSitesModule : public Module
{
    public:
    ModifierOSitesModule();
    ~ModifierOSitesModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Target SpeciesSite definitions
    std::vector<const SpeciesSite *> bridgingOxygenSpeciesSites_, networkFormerSpeciesSites_, modifierSpeciesSites_;
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{false};
    // Distance range for calculation
    Range distanceRange_{0.0, 2.5}, modifierDistanceRange_{0.0, 2.0};
    // Export target
    Data1DExportFileFormat exportFileAndFormatOType_;
    Data1DExportFileFormat exportFileAndFormatTotalOSites_;
    Data1DExportFileFormat exportFileAndFormatFOLength_;
    Data1DExportFileFormat exportFileAndFormatNBOLength_;
    Data1DExportFileFormat exportFileAndFormatBOLength_;
    Data1DExportFileFormat exportFileAndFormatOtherOLength_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
