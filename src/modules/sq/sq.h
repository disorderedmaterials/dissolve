// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/partialSet.h"
#include "math/averaging.h"
#include "math/function1D.h"
#include "math/windowFunction.h"
#include "module/module.h"

// Forward Declarations
class BraggModule;
class PartialSet;
class GRModule;

// SQ Module
class SQModule : public Module
{
    public:
    SQModule();
    ~SQModule() override = default;

    /*
     * Definition
     */
    private:
    // Number of historical partial sets to combine into final partials
    std::optional<int> averagingLength_;
    // Weighting scheme to use when averaging partials
    Averaging::AveragingScheme averagingScheme_{Averaging::LinearAveraging};
    // Broadening function to apply to Bragg S(Q)
    Functions::Function1DWrapper braggQBroadening_{Functions::Function1D::GaussianC2, {0.0, 0.02}};
    // Broadening function to apply to S(Q)
    Functions::Function1DWrapper qBroadening_;
    // Step size in Q for S(Q) calculation
    double qDelta_{0.05};
    // Maximum Q for calculated S(Q)
    double qMax_{30.0};
    // Minimum Q for calculated S(Q)
    double qMin_{0.01};
    // Whether to save partials to disk after calculation
    bool save_{false};
    // Source module for Bragg calculation
    const BraggModule *sourceBragg_{nullptr};
    // Source G(r) for main calculation
    const GRModule *sourceGR_{nullptr};
    // Window function to use when Fourier-transforming reference S(Q) to g(r))
    WindowFunction::Form windowFunction_{WindowFunction::Form::None};

    public:
    // Return source G(r) for main calculation
    const GRModule *sourceGR() const;

    /*
     * Functions
     */
    public:
    // Calculate unweighted S(Q) from unweighted g(r)
    static bool calculateUnweightedSQ(const ProcessPool &procPool, const PartialSet &unweightedgr, PartialSet &unweightedsq,
                                      double qMin, double qDelta, double qMax, double rho, const WindowFunction &windowFunction,
                                      Functions::Function1DWrapper broadening);

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;

    public:
    // Set target data
    void setTargets(const std::vector<std::unique_ptr<Configuration>> &configurations,
                    const std::map<ModuleTypes::ModuleType, std::vector<const Module *>> &moduleMap) override;
};
