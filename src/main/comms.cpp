// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/processpool.h"
#include "main/dissolve.h"
#include <numeric>

// Return the world process pool
const ProcessPool &Dissolve::worldPool() const { return worldPool_; }

// Set up communications
bool Dissolve::setUpMPIPools()
{
    Messenger::print("*** Setting up MPI pools...\n");

    // Set up the world pool
    std::vector<int> allProcesses(ProcessPool::nWorldProcesses());
    std::iota(allProcesses.begin(), allProcesses.end(), 0);
    worldPool_.setUp("World", allProcesses);

    return true;
}
