// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "modules/temperatureSchedule/temperatureSchedule.h"

// Run main processing
Module::ExecutionResult TemperatureScheduleModule::process(ModuleContext &moduleContext)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
    {
        Messenger::error("No configuration target set for module '{}'.\n", name());
        return ExecutionResult::Failed;
    }

    // Get the current configuration temperature
    auto currentT = targetConfiguration_->temperature();

    // Get difference with tergat temperature
    auto diffT = targetTemperature_ - currentT;

    // Get move amount
    auto deltaT = DissolveMath::sgn(diffT) * std::min(fabs(diffT), rate_);

    Messenger::print("Difference between current and target temperatures is {} K.\n", fabs(diffT));
    Messenger::print("New temperature will be {} K (rate max = {} K / iteration).\n", currentT + deltaT, rate_);

    targetConfiguration_->setTemperature(currentT + deltaT);

    return ExecutionResult::Success;
}
