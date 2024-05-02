// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <vector>

// Include <mpi.h> only if we are compiling in parallel
#ifdef PARALLEL
#include <mpi.h>
#endif

// ProcessGroup
class ProcessGroup
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
    // Vector of world ranks of processes in this group
    std::vector<int> worldRanks_;
    // Vector of corresponding pool ranks of world processes in this group
    std::vector<int> poolRanks_;
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
    std::vector<int> &worldRanks();
    // Return nth world rank of group processes
    int worldRank(int n) const;
    // Return pool ranks of group processes
    const std::vector<int> &poolRanks() const;
    // Return nth pool rank of group processes
    int poolRank(int n) const;
};
