// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    // Return type of module
    std::string_view type() const override;

    /*
     * Control
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
    // Module containing the target partial set data to accumulate
    std::vector<Module *> targetModule_;
    // Whether to save the accumulated partials to disk
    bool save_{false};

    /*
     * Processing
     */
    public:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
