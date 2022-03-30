// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "procedure/procedure.h"

// Analyse Module
class AnalyseModule : public Module
{
    public:
    AnalyseModule();
    ~AnalyseModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Analysis procedure to be run
    Procedure analyser_;

    public:
    // Return the analyser
    Procedure &analyser();

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, const ProcessPool &procPool) override;
};
