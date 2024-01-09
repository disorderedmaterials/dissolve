// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/processPool.h"
#include "main/dissolve.h"
#include <numeric>

// Set up the world pool
void Dissolve::setUpWorldPool()
{
    std::vector<int> allProcesses(ProcessPool::nWorldProcesses());
    std::iota(allProcesses.begin(), allProcesses.end(), 0);
    worldPool_.setUp("World", allProcesses);
    worldPool_.assignProcessesToGroups();
}

// Return the world process pool
const ProcessPool &Dissolve::worldPool() const { return worldPool_; }
