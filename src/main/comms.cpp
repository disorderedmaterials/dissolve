// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/processpool.h"
#include "main/dissolve.h"
#include <numeric>

// Return a world process pool
ProcessPool &Dissolve::worldPool()
{
    static bool firstRun = true;
    static ProcessPool world;

    // If this is the first time we've been called, construct the pool
    if (firstRun)
    {
        // Assemble list of (world) process ranks for the pool
        std::vector<int> ranks(ProcessPool::nWorldProcesses());
        std::iota(ranks.begin(), ranks.end(), 0);
        world.setUp("World", ranks);
        firstRun = false;
    }

    return world;
}

// Set up communications
bool Dissolve::setUpMPIPools()
{
    Messenger::print("*** Setting up MPI pools...\n");

    // Default pool - all world ranks
    std::vector<int> allProcesses(ProcessPool::nWorldProcesses());
    std::iota(allProcesses.begin(), allProcesses.end(), 0);

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
