// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/temperatureSchedule/temperatureSchedule.h"
#include "keywords/configuration.h"
#include "keywords/double.h"

TemperatureScheduleModule::TemperatureScheduleModule() : Module(ModuleTypes::TemperatureSchedule)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Schedule", "Control rate of temperature change on the target configuration.");
    keywords_.add<DoubleKeyword>("TargetTemperature", "Target temperature, in Kelvin, to move towards", targetTemperature_,
                                 0.0);
    keywords_.add<DoubleKeyword>(
        "Rate", "Rate of temperature change, in Kelvin per iteration, to move towards target temperature", rate_, 0.0);
}
