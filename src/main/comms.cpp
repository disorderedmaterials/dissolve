// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/processpool.h"
#include "main/dissolve.h"

// Return a world process pool
ProcessPool &Dissolve::worldPool()
{
    static bool firstRun = true;
    static ProcessPool world;

    // If this is the first time we've been called, construct the pool
    if (firstRun)
    {
        // Assemble list of (world) process ranks for the pool
        Array<int> ranks;
        for (auto n = 0; n < ProcessPool::nWorldProcesses(); ++n)
            ranks.add(n);
        world.setUp("World", ranks);
        firstRun = false;
    }

    return world;
}

// Set up communications
bool Dissolve::setUpMPIPools()
{
    Messenger::print("*** Setting up MPI pools...\n");

    // Get relative atom counts between each configuration
    Array<int> configSizes;
    for (auto *cfg = coreData_.configurations().first(); cfg != nullptr; cfg = cfg->next())
        configSizes.add(cfg->nAtoms());

    // Default pool - all world ranks
    Array<int> allProcesses;
    for (auto n = 0; n < ProcessPool::nWorldProcesses(); ++n)
        allProcesses.add(n);

    // Set up pool based on selected strategy
    auto cfgIndex = 0;
    for (auto *cfg = coreData_.configurations().first(); cfg != nullptr; cfg = cfg->next())
    {
        Messenger::print("Configuration '{}':\n", cfg->name());

        // Simple, sequential strategy - all processes assigned to all Configurations
        if (!cfg->setUpProcessPool(allProcesses))
            return false;

        // Increase Configuration index
        ++cfgIndex;
    }

    return true;
}
