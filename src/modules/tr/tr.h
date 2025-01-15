// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/partialSet.h"
#include "data/structureFactors.h"
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
    // Maximum Q for calculated S(Q)
    double qMax_{50.0};
    // Minimum Q for calculated S(Q)
    double qMin_{0.01};
    // Window function to use when Fourier-transforming reference S(Q) to g(r))
    WindowFunction::Form windowFunction_{WindowFunction::Form::Lorch0};
    // Normalisation to apply to calculated total F(Q)
    StructureFactors::NormalisationType normaliseTo_{StructureFactors::NoNormalisation};
    // Broadening function to apply to S(Q)
    Function1DWrapper qBroadening_;
    // Source module for calculation
    const NeutronSQModule *sourceNeutronSQ_{nullptr};
    bool saveTR_{false};
    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
