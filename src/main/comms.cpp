// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
