// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
};
