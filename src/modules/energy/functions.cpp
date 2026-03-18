// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "modules/energy/energy.h"
#include "templates/algorithms.h"
#include "templates/parallelDefs.h"
#include <numeric>

// Check energy stability of specified Configuration
EnergyModule::EnergyStability EnergyModule::checkStability(GenericList &processingData, const Configuration *cfg)
{
    // First, check if the Configuration is targetted by an EnergyModule
    if (!processingData.valueOr<bool>("IsEnergyModuleTarget", cfg->name(), false))
    {
        Messenger::error("Configuration '{}' is not targeted by any EnergyModule, so stability cannot be assessed. "
                         "Check your setup!\n",
                         cfg->name());
        return EnergyModule::NotAssessable;
    }

    // Retrieve the EnergyStable flag from the Configuration's module data
    if (processingData.contains("EnergyStable", cfg->name()))
    {
        auto stable = processingData.value<bool>("EnergyStable", cfg->name());
        if (!stable)
        {
            Messenger::print("Energy for Configuration '{}' is not yet stable.\n", cfg->name());
            return EnergyModule::EnergyUnstable;
        }
    }
    else
    {
        Messenger::warn("No energy stability information is present for Configuration '{}' (yet) - check your setup.\n",
                        cfg->name());
        return EnergyModule::NotAssessable;
    }

    return EnergyModule::EnergyStable;
}

// Check energy stability of specified Configurations, returning the number that ExecutionResult::Failed
int EnergyModule::nUnstable(GenericList &processingData, const std::vector<Configuration *> &configurations)
{
    auto nFailed = 0;

    for (auto *cfg : configurations)
    {
        // Check the stability of this Configuration
        auto result = checkStability(processingData, cfg);

        if (result == EnergyModule::EnergyStable)
            ++nFailed;
        else if (result == EnergyModule::NotAssessable)
            return EnergyModule::NotAssessable;
    }

    return nFailed;
}
