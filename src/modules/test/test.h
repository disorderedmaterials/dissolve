// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// Test Module
class TestModule : public Module
{
    public:
    TestModule();
    ~TestModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
