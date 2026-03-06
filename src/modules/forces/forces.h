// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "io/export/forces.h"
#include "io/import/forces.h"
#include "kernels/common.h"
#include "module/module.h"

// Forward Declarations
class Molecule;
class PotentialMap;

// Forces Module
class ForcesModule : public Module
{
    public:
    ForcesModule();
    ~ForcesModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configurations
    Configuration *targetConfiguration_{nullptr};
    // Test analytic forces against production forces force routines against basic serial versions and supplied reference values
    // (if provided)
    bool test_{false};
    // Threshold of force (%) at which test comparison will fail
    double testThreshold_{1.0e-6};
    // Reference forces for test
    ForceImportFileFormat referenceForces_;
    // Force export file and format
    ForceExportFileFormat exportedForces_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(Dissolve &dissolve) override;

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, Flags<KeywordBase::KeywordSignal> actionSignals) override;
};
