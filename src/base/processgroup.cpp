// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/processgroup.h"
#include "base/messenger.h"

ProcessGroup::ProcessGroup() : ListItem<ProcessGroup>() {}

/*
 * Group Data
 */

// Add process to group
void ProcessGroup::addProcess(int poolRank, int worldRank)
{
    poolRanks_.add(poolRank);
    worldRanks_.add(worldRank);
}

// Return total number of processes in group
int ProcessGroup::nProcesses() { return poolRanks_.nItems(); }

// Return world ranks of group processes
Array<int> &ProcessGroup::worldRanks() { return worldRanks_; }

// Return nth world rank of group processes
int ProcessGroup::worldRank(int n) const { return worldRanks_.constAt(n); }

// Return pool ranks of group processes
Array<int> &ProcessGroup::poolRanks() { return poolRanks_; }

// Return nth pool rank of group processes
int ProcessGroup::poolRank(int n) const { return poolRanks_.constAt(n); }
