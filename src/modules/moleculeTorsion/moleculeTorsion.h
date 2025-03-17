// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "io/export/data1D.h"
#include "module/module.h"

// Calculate RDF Module
class MoleculeTorsionModule : public Module
{
    public:
    MoleculeTorsionModule();
    ~MoleculeTorsionModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Target Species
    const Species *species_{nullptr};
    // Target atom indices in user index space (1 -> N)
    int i_{1}, j_{2}, k_{3}, l_{4};
    // Export target
    Data1DExportFileFormat exportFileAndFormat_;

    /*
     * Processing
     */
    private:
    // Run set-up stage
    bool setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals);
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
