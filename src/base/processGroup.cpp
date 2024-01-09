// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/processGroup.h"
#include "base/messenger.h"

ProcessGroup::ProcessGroup() {}

/*
 * Group Data
 */

// Add process to group
void ProcessGroup::addProcess(int poolRank, int worldRank)
{
    poolRanks_.push_back(poolRank);
    worldRanks_.push_back(worldRank);
}

// Return total number of processes in group
int ProcessGroup::nProcesses() const { return poolRanks_.size(); }

// Return world ranks of group processes
std::vector<int> &ProcessGroup::worldRanks() { return worldRanks_; }

// Return nth world rank of group processes
int ProcessGroup::worldRank(int n) const { return worldRanks_.at(n); }

// Return pool ranks of group processes
const std::vector<int> &ProcessGroup::poolRanks() const { return poolRanks_; }

// Return nth pool rank of group processes
int ProcessGroup::poolRank(int n) const { return poolRanks_.at(n); }
