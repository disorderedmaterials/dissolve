// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/partialSet.h"
#include "math/windowFunction.h"
#include "module/module.h"

// Forward Declarations
class Dissolve;
class PartialSet;
class NeutronSQModule;

// TR Module
class TRModule : public Module
{
    public:
    TRModule();
    ~TRModule() override = default;

    /*
     * Definition
     */
    public:
    private:
    // Step size in Q for S(Q) calculation
    double qDelta_{0.05};
    // Maximum Q for calculating reference G(r)
    std::optional<double> refQMax_{30.0};
    // Minimum Q for calculating reference G(r)
    std::optional<double> refQMin_{0.3};
    // Window function to use when Fourier-transforming reference S(Q) to G(r))
    WindowFunction::Form refWindowFunction_{WindowFunction::Form::None};
    // Broadening function to apply to reference S(Q)
    Function1DWrapper refQBroadening_;

    // Maximum Q for calculated representative G(r)
    std::optional<double> repQMax_{30.0};
    // Minimum Q for calculated representative G(r)
    std::optional<double> repQMin_{0.3};
    // Window function to use when Fourier-transforming weighted S(Q) to G(r)
    WindowFunction::Form repWindowFunction_{WindowFunction::Form::None};
    // Broadening function to apply to Weighted S(Q)
    Function1DWrapper repQBroadening_;

    // Source module for calculation
    const NeutronSQModule *sourceNeutronSQ_{nullptr};
    bool saveTR_{false};
    bool saveRepTR_{false};

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
