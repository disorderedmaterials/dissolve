// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/array.h"
// Include <mpi.h> only if we are compiling in parallel
#ifdef PARALLEL
#include <mpi.h>
#endif

// ProcessGroup
class ProcessGroup : public ListItem<ProcessGroup>
{
    /*
     * Class to define a group of processes within a pool, keeping reference of their pool and world ranks.
     */
    public:
    ProcessGroup();

    /*
     * Group Data
     */
    private:
    // List of world ranks of processes in this group
    Array<int> worldRanks_;
    // List of corresponding pool ranks of world processes in this group
    Array<int> poolRanks_;
    // World rank of the group leader process
    int leaderWorldRank_;
    // Pool rank of the group leader process
    int leaderPoolRank_;

    public:
    // Add process to group
    void addProcess(int poolRank, int worldRank);
    // Return total number of processes in group
    int nProcesses() const;
    // Return world ranks of group processes
    Array<int> &worldRanks();
    // Return nth world rank of group processes
    int worldRank(int n) const;
    // Return pool ranks of group processes
    const Array<int> &poolRanks() const;
    // Return nth pool rank of group processes
    int poolRank(int n) const;
};
