// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// Accumulate Module
class AccumulateModule : public Module
{
    public:
    AccumulateModule();
    ~AccumulateModule() override = default;

    /*
     * Definition
     */
    public:
    // Target PartialSet Enum
    enum TargetPartialSet
    {
        GR,
        SQ,
        OriginalGR
    };
    // Return EnumOptions for TargetPartialSet
    static EnumOptions<TargetPartialSet> targetPartialSet();

    private:
    // Type of target PartialSet
    AccumulateModule::TargetPartialSet targetPartialSet_{TargetPartialSet::GR};
    // Modules containing the target partial set data to accumulate (individually)
    std::vector<Module *> targetModules_;
    // Whether to save the accumulated partial sets to disk
    bool save_{false};

    /*
     * Processing
     */
    public:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
