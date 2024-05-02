// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/braggReflection.h"
#include "math/averaging.h"
#include "module/module.h"

// Bragg Module
class BraggModule : public Module
{
    public:
    BraggModule();
    ~BraggModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Number of historical data sets to combine into final reflection data
    std::optional<int> averagingLength_{5};
    // Weighting scheme to use when averaging reflection data
    Averaging::AveragingScheme averagingScheme_{Averaging::LinearAveraging};
    // Bragg intensity scaling factor accounting for number of repeat units in Configuration
    Vec3<int> multiplicity_{1, 1, 1};
    // Resolution (bin width) in Q space to use when calculating Bragg reflections
    double qDelta_{0.001};
    // Maximum Q value for Bragg calculation
    double qMax_{1.0};
    // Minimum Q value for Bragg calculation
    double qMin_{0.01};
    // Whether to save Bragg reflection data to disk
    bool saveReflections_{false};

    /*
     * Functions
     */
    public:
    // Calculate Bragg terms for specified Configuration
    bool calculateBraggTerms(GenericList &moduleData, const ProcessPool &procPool, Configuration *cfg, const double qMin,
                             const double qDelta, const double qMax, Vec3<int> multiplicity, bool &alreadyUpToDate);
    // Form partial and total reflection functions from calculated reflection data
    bool formReflectionFunctions(GenericList &moduleData, const ProcessPool &procPool, Configuration *cfg, const double qMin,
                                 const double qDelta, const double qMax);
    // Re-bin reflection data into supplied arrays
    bool reBinReflections(GenericList &moduleData, const ProcessPool &procPool, Configuration *cfg,
                          Array2D<Data1D> &braggPartials);

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
