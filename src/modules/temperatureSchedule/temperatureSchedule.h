// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// TemperatureSchedule Module
class TemperatureScheduleModule : public Module
{
    public:
    TemperatureScheduleModule();
    ~TemperatureScheduleModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Target temperature
    double targetTemperature_{300.0};
    // Rate of change, Kelvin per iteration
    double rate_{1.0};

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
