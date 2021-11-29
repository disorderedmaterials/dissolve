// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// Skeleton Module
class SkeletonModule : public Module
{
    public:
    SkeletonModule();
    ~SkeletonModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configurations
    std::vector<Configuration *> targetConfigurations_;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
