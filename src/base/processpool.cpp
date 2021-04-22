// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/processpool.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "templates/algorithms.h"

// Static Members
int ProcessPool::nWorldProcesses_ = 1;
int ProcessPool::worldRank_ = 0;

// External Declarations
int ProcessPool::FAILED = 0;
int ProcessPool::SUCCEEDED = 1;
int ProcessPool::RESULT;

ProcessPool::ProcessPool() { clear(); }

ProcessPool::ProcessPool(const ProcessPool &source)
{
    (*this) = source;
    poolRank_ = -99;
    groupIndex_ = -99;
    groupRank_ = -99;
}

void ProcessPool::operator=(const ProcessPool &source)
{
    // Process Identification
    name_ = source.name_;
    poolRank_ = source.poolRank_;
    worldRanks_ = source.worldRanks_;

    // Local groups
    processGroups_ = source.processGroups_;
    groupIndex_ = source.groupIndex_;
    groupRank_ = source.groupRank_;
    groupLeaders_ = source.groupLeaders_;
    maxProcessGroups_ = source.maxProcessGroups_;
#ifdef PARALLEL
    groupGroup_ = source.groupGroup_;
    groupCommunicator_ = source.groupCommunicator_;
    leaderGroup_ = source.leaderGroup_;
    leaderCommunicator_ = source.leaderCommunicator_;
    poolGroup_ = source.poolGroup_;
    poolCommunicator_ = source.poolCommunicator_;
#endif
    groupsModifiable_ = source.groupsModifiable_;

    // Random number buffer
    // ???
}

// Clear all data
void ProcessPool::clear()
{
    poolRank_ = -1;
    groupIndex_ = -1;
    groupRank_ = -1;
    worldRanks_.clear();
    processGroups_.clear();
    maxProcessGroups_ = 1;
    groupLeaders_.clear();
    groupsModifiable_ = true;
#ifdef PARALLEL
    groupGroup_ = 0;
    groupCommunicator_ = 0;
    leaderGroup_ = 0;
    leaderCommunicator_ = 0;
    poolGroup_ = 0;
    poolCommunicator_ = 0;
#endif
}

#ifdef PARALLEL
// Return communicator specified
MPI_Comm ProcessPool::communicator(ProcessPool::CommunicatorType commType)
{
    if (commType == ProcessPool::GroupProcessesCommunicator)
        return groupCommunicator_;
    else if (commType == ProcessPool::GroupLeadersCommunicator)
        return leaderCommunicator_;
    else if (commType == ProcessPool::PoolProcessesCommunicator)
        return poolCommunicator_;
    Messenger::print("BAD_USAGE - CommunicatorType {} is not valid in ProcessPool::communicator().\n", commType);

    return MPI_COMM_WORLD;
}
#endif

/*
 * Global Information
 */

// Initialise parallel communications, setting world ranks / nProcesses
bool ProcessPool::initialiseMPI(int *argn, char ***argv)
{
    // Initialise MPI
#ifdef PARALLEL
    Messenger::printVerbose("Initialising MPI...\n");
    if (MPI_Init(argn, argv) == MPI_SUCCESS)
    {
        Messenger::print("Initialised MPI.\n");
        if (MPI_Comm_size(MPI_COMM_WORLD, &nWorldProcesses_))
        {
            Messenger::error("Failed to get world size.\n");
            return false;
        }
        Messenger::print("Number of processes = {}\n", nWorldProcesses_);

        if (MPI_Comm_rank(MPI_COMM_WORLD, &worldRank_))
        {
            Messenger::error("Failed to get process rank.\n");
            return false;
        }
    }
    else
    {
        Messenger::error("Failed to initialise MPI.\n");
        return false;
    }
#endif
    return true;
}

// End parallel communications
void ProcessPool::finalise()
{
#ifdef PARALLEL
    MPI_Finalize();
#endif
}

// Return number of world processes
int ProcessPool::nWorldProcesses() { return nWorldProcesses_; }

// Return world rank of this process
int ProcessPool::worldRank() { return worldRank_; }

// Return if this is the world master process
bool ProcessPool::isWorldMaster() { return (worldRank_ == 0); }

/*
 * Timing
 */

// Reset accumulated Comm time
void ProcessPool::resetAccumulatedTime() { timer_.zero(); }

// Return accumulated time string
std::string ProcessPool::accumulatedTimeString() { return timer_.totalTimeString(); }

/*
 * Process Identification
 */

// Return local rank of this process in the pool
int ProcessPool::poolRank() const { return poolRank_; }

// Return whether this process is the master for the specified communicator
bool ProcessPool::isMaster(ProcessPool::CommunicatorType commType) const
{
    if (commType == ProcessPool::PoolProcessesCommunicator)
        return (poolRank_ == 0);
    else if (commType == ProcessPool::GroupProcessesCommunicator)
        return (groupLeaders_.at(groupIndex_) == poolRank_);
    else if (commType == ProcessPool::GroupLeadersCommunicator)
        return (groupLeaders_.at(0) == poolRank_);

    return false;
}

// Return whether this process is a local slave for the specified communicatyr
bool ProcessPool::isSlave(ProcessPool::CommunicatorType commType) const
{
    if (commType == ProcessPool::PoolProcessesCommunicator)
        return (poolRank_ != 0);
    else if (commType == ProcessPool::GroupProcessesCommunicator)
        return (groupLeaders_.at(groupIndex_) != poolRank_);
    else if (commType == ProcessPool::GroupLeadersCommunicator)
        return (groupLeaders_.at(0) != poolRank_);

    return true;
}

// Return whether this process is the pool index specified
bool ProcessPool::isMe(int poolIndex) const { return (poolRank_ == poolIndex); }

// Return whether this pool involves this process
bool ProcessPool::involvesMe() const
{
    for (auto n = 0; n < worldRanks_.nItems(); ++n)
        if (worldRanks_.at(n) == worldRank_)
            return true;
    return false;
}

// Return group index in which this process exists
int ProcessPool::groupIndex() const { return groupIndex_; }

// Return local group in which this process exists
ProcessGroup &ProcessPool::myGroup()
{
    assert(groupIndex_ >= 0 && groupIndex_ < processGroups_.nItems());
    return processGroups_[groupIndex_];
}

// Return rank of this process in its local group
int ProcessPool::groupRank() const { return groupRank_; }

// Return whether this process is a group leader
bool ProcessPool::groupLeader() const { return (groupRank_ == 0); }

/*
 * Pool Contents
 */

// Set up pool with world ranks specified
bool ProcessPool::setUp(std::string_view name, Array<int> worldRanks)
{
    clear();

    name_ = name;

    Messenger::print("Setting up process pool '{}'...\n", name_);

    // Set rank list
    worldRanks_ = worldRanks;

    // See if our rank is in the list
    for (auto n = 0; n < worldRanks_.nItems(); ++n)
    {
        if (worldRank_ == worldRanks_[n])
        {
            poolRank_ = n;
            Messenger::print("Process with world rank {} added to pool '{}' with local rank {}.\n", worldRanks_[n], name_, n);
            break;
        }
    }

    // Set default maximum number of groups
    maxProcessGroups_ = worldRanks_.nItems();

    Messenger::print("There are {} processes in pool '{}' (max groups = {}).\n", worldRanks_.nItems(), name_,
                     maxProcessGroups_);

#ifdef PARALLEL
    // Create pool group and communicator
    MPI_Group origGroup;
    MPI_Comm_group(MPI_COMM_WORLD, &origGroup);
    if (MPI_Group_incl(origGroup, worldRanks_.nItems(), worldRanks_.array(), &poolGroup_) != MPI_SUCCESS)
        return false;
    if (MPI_Comm_create(MPI_COMM_WORLD, poolGroup_, &poolCommunicator_) != MPI_SUCCESS)
        return false;
#endif

    return true;
}

// Return name of pool
std::string_view ProcessPool::name() const { return name_; }

// Return total number of processes in pool
int ProcessPool::nProcesses() const { return worldRanks_.nItems(); }

// Return root (first) world rank of this pool
int ProcessPool::rootWorldRank() const { return worldRanks_.at(0); }

// Assign processes to groups
bool ProcessPool::assignProcessesToGroups()
{
    /*
     * Create process groups and set group membership for each process
     * The process in the pool (list in worldRanks_) will be assigned to one or more ProcessGroups.
     * Each process in the pool will construct a list of the groups and the world ranks in each.
     * Afterwards, an MPI communicator is constructed for each group.
     */
#ifdef PARALLEL
    int baseAlloc = worldRanks_.nItems() / maxProcessGroups_;
    int remainder = worldRanks_.nItems() % maxProcessGroups_;
    std::string rankString;
    for (auto n = 0; n < maxProcessGroups_; ++n)
    {
        // Create nth process group and add a (currently null) entry to the groupLeaders_ array
        ProcessGroup group;
        groupLeaders_.add(-1);
        Messenger::printVerbose("Created process group {}\n", n);

        // Create array of ranks to put in new group
        auto firstRank = baseAlloc * n + (n < remainder ? n : remainder);
        auto lastRank = firstRank + baseAlloc - (n >= remainder ? 1 : 0);
        rankString.clear();
        for (auto localRank = firstRank; localRank <= lastRank; ++localRank)
        {
            auto wr = worldRanks_[localRank];
            group.addProcess(localRank, wr);
            rankString += fmt::format(" {}", wr);

            // If this process is the current worldRank_ we are considering, set its group membership
            if (wr == worldRank_)
                groupIndex_ = n;
        }
        Messenger::printVerbose("Group will contain {} processes (world ranks:{}).\n", group.nProcesses(), rankString);
        processGroups_.add(group);
    }

    // Create local group and communicator - each process will only create and be involved in one group communicator
    // (groupGroup_)
    MPI_Group origGroup;
    MPI_Comm_group(MPI_COMM_WORLD, &origGroup);
    if (MPI_Group_incl(origGroup, myGroup().nProcesses(), myGroup().worldRanks().array(), &groupGroup_) != MPI_SUCCESS)
        return false;
    if (MPI_Comm_create(MPI_COMM_WORLD, groupGroup_, &groupCommunicator_) != MPI_SUCCESS)
        return false;
    MPI_Group_rank(groupGroup_, &groupRank_);
    Messenger::printVerbose("... Process with pool rank {} (world rank {}) has local group {}, group rank {}, and is "
                            "a process group {}\n",
                            poolRank_, worldRank_, groupIndex_, groupRank_, groupLeader() ? "leader" : "slave");

    // Master now assembles list of group leaders
    bool leader;
    if (isMaster())
    {
        Messenger::printVerbose("Process world rank {} is the master of pool '{}', and will assemble group leaders...\n",
                                worldRank_, name_);
        // Loop over process groups
        for (auto group = 0; group < processGroups_.nItems(); ++group)
        {
            // Query each process in the group to see if it is the leader...
            for (auto n = 0; n < nProcessesInGroup(group); ++n)
            {
                auto prank = processGroups_[group].poolRank(n);

                // Is this us, the master process?
                if (prank == poolRank_)
                    leader = groupLeader();
                else if (!receive(leader, prank))
                    return false;

                // Is this process the leader?
                if (leader)
                {
                    // Check for existing value
                    if (groupLeaders_[group] == -1)
                        groupLeaders_[group] = prank;
                    else
                    {
                        Messenger::error("MPI Error: More than one process group leader for group {}.\n", group);
                        return false;
                    }

                    Messenger::printVerbose("Process with pool rank {} (world rank {}) added to group "
                                            "leaders list for pool '{}'.\n",
                                            prank, worldRanks_[prank], name_);
                }
            }
        }
    }
    else if (!send(groupLeader(), 0))
        return false;

    // Broadcast group leader list
    if (!broadcast(groupLeaders_))
        return false;
    Messenger::printVerbose("Group leader processes are :\n");
    for (auto group = 0; group < processGroups_.nItems(); ++group)
        Messenger::printVerbose("   Group {:3d} : process rank {}\n", group, groupLeaders_[group]);

    // Create group leader communicator
    // Must first convert local pool ranks of the group leaders into world ranks, before passing this to MPI_Group_incl
    Array<int> groupLeadersW;
    for (auto n = 0; n < groupLeaders_.nItems(); ++n)
        groupLeadersW.add(worldRanks_[groupLeaders_[n]]);
    MPI_Comm_group(MPI_COMM_WORLD, &origGroup);
    if (MPI_Group_incl(origGroup, groupLeadersW.nItems(), groupLeadersW.array(), &leaderGroup_) != MPI_SUCCESS)
        return false;
    if (MPI_Comm_create(MPI_COMM_WORLD, leaderGroup_, &leaderCommunicator_) != MPI_SUCCESS)
        return false;
#else
    // No MPI, but must still set up a dummy process group
    ProcessGroup group;
    group.addProcess(0, 0);
    processGroups_.add(group);
    groupIndex_ = 0;
    groupRank_ = 0;
#endif

    return true;
}

// Assign processes to groups taken from supplied ProcessPool
bool ProcessPool::assignProcessesToGroups(ProcessPool &groupsSource)
{
    /*
     * Since we have the ability to run Modules with any ProcessPool and at any point, we must occasionally
     * re-assign the processes in the pool (typically the Dissolve::worldPool_) to a different set of groups in
     * order to utilise all available processing power (e.g. when a Module is run as, or is performing, a
     * post-processing step).
     */

    // If we have been supplied with ourself as the reference ProcessPool, we can exit gracefully now
    if (this == &groupsSource)
        return true;

    // First check that we are allowed to modify the groups within this pool
    if (!groupsModifiable_)
    {
        Messenger::error("Tried to modify the group contents of a ProcessPool in which they have explicitly been fixed.\n");
        return false;
    }

#ifdef PARALLEL
    // All processes in this pool first abandon their current groupGroup_ and leaderGroup_ communicators and groups
    if (groupGroup_ != 0)
        MPI_Group_free(&groupGroup_);
    if (groupCommunicator_ != 0)
        MPI_Comm_free(&groupCommunicator_);
    if (leaderGroup_ != 0)
        MPI_Group_free(&leaderGroup_);
    // -- NOTE If a group slave tries to free the leaderCommunicator_, we get an 'invalid communicator' error
    if (groupLeader() && (leaderCommunicator_ != 0))
        MPI_Comm_free(&leaderCommunicator_);
    groupGroup_ = 0;
    groupCommunicator_ = 0;
    leaderGroup_ = 0;
    leaderCommunicator_ = 0;
    groupRank_ = -1;
    groupIndex_ = -1;
#endif

    // Reset the groupLeaders and processGroups arrays
    groupLeaders_.clear();
    processGroups_.clear();

    // Copy over the number of allowable groups from the source ProcessPool
    maxProcessGroups_ = 1;
    if (!assignProcessesToGroups())
    {
        Messenger::error("Failed to re-assign processes to groups in this ProcessPool.\n");
        return false;
    }

    return true;
}

// Return number of process groups
int ProcessPool::nProcessGroups() const { return processGroups_.nItems(); }

// Return nth process group
ProcessGroup &ProcessPool::processGroup(int n)
{
    assert(n >= 0 && n < processGroups_.nItems());
    return processGroups_[n];
}

// Return number of processes in specified group
int ProcessPool::nProcessesInGroup(int groupId) const
{
    assert(groupId >= 0 && groupId < processGroups_.nItems());
    return processGroups_.at(groupId).nProcesses();
}

// Return array of pool ranks in specified group
const Array<int> &ProcessPool::poolRanksInGroup(int groupId) const
{
    assert(groupId >= 0 && groupId < processGroups_.nItems());
    return processGroups_.at(groupId).poolRanks();
}

// Return whether group data is modifiable
bool ProcessPool::groupsModifiable() const { return groupsModifiable_; }

// Prevent group data from being modified
void ProcessPool::setGroupsFixed() { groupsModifiable_ = false; }

// Return maximum number of simultaneous, single-Cell-modifying process groups
int ProcessPool::maxProcessGroups() const { return maxProcessGroups_; }

/*
 * Strategy / Limits
 */

// Return sub-strategy for specified dividion of labour strategy
ProcessPool::DivisionStrategy ProcessPool::subDivisionStrategy(ProcessPool::DivisionStrategy strategy)
{
    switch (strategy)
    {
        case (GroupsStrategy):
            return GroupProcessesStrategy;
        case (GroupProcessesStrategy):
            Messenger::error("Strategy is GroupProcessesStrategy, and so can't subdivide any further. Results may "
                             "be incorrect!\n");
            return GroupProcessesStrategy;
        case (PoolStrategy):
            return PoolProcessesStrategy;
        case (PoolProcessesStrategy):
            Messenger::error("Strategy is PoolProcessesStrategy, and so can't subdivide any further. Results may "
                             "be incorrect!\n");
            return PoolProcessesStrategy;
    }

    // Default
    return PoolProcessesStrategy;
}

// Return starting index for loop using specified strategy
int ProcessPool::interleavedLoopStart(ProcessPool::DivisionStrategy strategy) const
{
    if (strategy == ProcessPool::GroupsStrategy)
        return groupIndex_;
    else if (strategy == ProcessPool::GroupProcessesStrategy)
        return groupRank_;
    else if (strategy == ProcessPool::PoolStrategy)
        return poolRank_;

    // PoolProcessesStrategy
    return 0;
}

// Return stride for general outer loop using specified strategy
int ProcessPool::interleavedLoopStride(ProcessPool::DivisionStrategy strategy) const
{
    if (strategy == ProcessPool::GroupsStrategy)
        return processGroups_.nItems();
    else if (strategy == ProcessPool::GroupProcessesStrategy)
        return nProcessesInGroup(groupIndex_);
    else if (strategy == ProcessPool::PoolStrategy)
        return worldRanks_.nItems();

    // PoolProcessesStrategy
    return 1;
}

// Return 'size' for specified strategy
int ProcessPool::strategyNDivisions(ProcessPool::DivisionStrategy strategy) const
{
    switch (strategy)
    {
        case (GroupsStrategy):
            return nProcessGroups();
        case (GroupProcessesStrategy):
            return nProcessesInGroup(groupIndex());
        case (PoolStrategy):
            return nProcesses();
        case (PoolProcessesStrategy):
            return 1;
    }

    // Default
    return 1;
}

// Return index of this process within the specified strategy
int ProcessPool::strategyProcessIndex(ProcessPool::DivisionStrategy strategy) const
{
    switch (strategy)
    {
        case (GroupsStrategy):
            return groupIndex_;
        case (GroupProcessesStrategy):
            return groupRank_;
        case (PoolStrategy):
            return poolRank_;
        case (PoolProcessesStrategy):
            return 0;
    }

    // Default
    return 0;
}

// Return best strategy (by process or by pool) for this process pool
ProcessPool::DivisionStrategy ProcessPool::bestStrategy() const
{
    return (worldRanks_.nItems() > maxProcessGroups_ ? ProcessPool::GroupsStrategy : ProcessPool::PoolStrategy);
}

// Return starting outer loop index for a two-body interaction calculation where only the upper half (i >= j) is required
int ProcessPool::twoBodyLoopStart(int nItems) const
{
    // TODO Optimise calculation of limit
    // Don't do anything if this pool doesn't involve us
#ifdef PARALLEL
    if (!involvesMe())
        return -1;
#endif

    // Diagonal Atoms - For calculation of upper-diagonal half of any two-body interaction matrix
    double rnproc = 1.0 / worldRanks_.nItems(), area = 1.0;
    int startAtom = 0, finishAtom = -1;

    // Loop over processes
    for (auto process = 0; process < worldRanks_.nItems(); ++process)
    {
        // If this is the last process, make sure we avoid doing sqrt of zero or delta-neg value
        if (process == (worldRanks_.nItems() - 1))
            finishAtom = nItems - 1;
        else
            finishAtom = (1.0 - sqrt(area - rnproc)) * nItems - 1;
        area -= rnproc;

        // Store limits for this process (if it is us)
        if (process == poolRank_)
        {
            Messenger::printVerbose("Assigned two-body outer loop start index of {} for process with pool rank {}.\n",
                                    startAtom, process);
            return startAtom;
        }

        // Update startAtom
        startAtom = finishAtom + 1;
    }

    return -1;
}

// Return ending outer loop index for a two-body interaction calculation where only the upper half (i >= j) is required
int ProcessPool::twoBodyLoopEnd(int nItems) const
{
    // TODO Optimise calculation of limit
    // Don't do anything if this pool doesn't involve us
#ifdef PARALLEL
    if (!involvesMe())
        return -1;
#endif

    // Diagonal Atoms - For calculation of upper-diagonal half of any two-body interaction matrix
    double rnproc = 1.0 / worldRanks_.nItems(), area = 1.0;
    int finishAtom;

    // Loop over processes
    for (auto process = 0; process < worldRanks_.nItems(); ++process)
    {
        // If this is the last process, make sure we avoid doing sqrt of zero or delta-neg value
        if (process == (worldRanks_.nItems() - 1))
            finishAtom = nItems - 1;
        else
            finishAtom = (1.0 - sqrt(area - rnproc)) * nItems - 1;
        area -= rnproc;

        // Store limits for this process (if it is us)
        if (process == poolRank_)
        {
            Messenger::printVerbose("Assigned two-body outer loop final index of {} for process with pool rank {}.\n",
                                    finishAtom, process);
            return finishAtom;
        }
    }

    return -1;
}

/*
 * Send/Receive Functions
 */

// Wait for all processes
bool ProcessPool::wait(ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    if (MPI_Barrier(communicator(commType)) != MPI_SUCCESS)
        return false;
#endif
    return true;
}

// Send single integer value to target process
bool ProcessPool::send(int value, int targetRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if (MPI_Send(&value, 1, MPI_INTEGER, targetRank, 0, communicator(commType)) != MPI_SUCCESS)
        return false;
    timer_.accumulate();
#endif
    return true;
}

// Receive single integer value from source process
bool ProcessPool::receive(int &value, int sourceRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    MPI_Status status;
    if (MPI_Recv(&value, 1, MPI_INTEGER, sourceRank, 0, communicator(commType), &status) != MPI_SUCCESS)
        return false;
    timer_.accumulate();
#endif
    return true;
}

// Send single long integer value to target process
bool ProcessPool::send(long int value, int targetRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if (MPI_Send(&value, 1, MPI_LONG, targetRank, 0, communicator(commType)) != MPI_SUCCESS)
        return false;
    timer_.accumulate();
#endif
    return true;
}

// Receive single long integer value from source process
bool ProcessPool::receive(long int &value, int sourceRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    MPI_Status status;
    if (MPI_Recv(&value, 1, MPI_LONG, sourceRank, 0, communicator(commType), &status) != MPI_SUCCESS)
        return false;
    timer_.accumulate();
#endif
    return true;
}

// Send single double value to target process
bool ProcessPool::send(double value, int targetRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if (MPI_Send(&value, 1, MPI_DOUBLE, targetRank, 0, communicator(commType)) != MPI_SUCCESS)
        return false;
    timer_.accumulate();
#endif
    return true;
}

// Receive single double value from source process
bool ProcessPool::receive(double &value, int sourceRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    MPI_Status status;
    if (MPI_Recv(&value, 1, MPI_DOUBLE, sourceRank, 0, communicator(commType), &status) != MPI_SUCCESS)
        return false;
    timer_.accumulate();
#endif
    return true;
}

// Send single bool value to target process
bool ProcessPool::send(bool value, int targetRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    int data = value;
    if (MPI_Send(&data, 1, MPI_INTEGER, targetRank, 0, communicator(commType)) != MPI_SUCCESS)
        return false;
    timer_.accumulate();
#endif
    return true;
}

// Receive single bool value from source process
bool ProcessPool::receive(bool &value, int sourceRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    MPI_Status status;
    int result;
    if (MPI_Recv(&result, 1, MPI_INTEGER, sourceRank, 0, communicator(commType), &status) != MPI_SUCCESS)
        return false;
    value = result;
    timer_.accumulate();
#endif
    return true;
}

// Send integer array data to target process
bool ProcessPool::send(int *source, int nData, int targetRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if (MPI_Send(source, nData, MPI_INTEGER, targetRank, 0, communicator(commType)) != MPI_SUCCESS)
        return false;
    timer_.accumulate();
#endif
    return true;
}

// Receive integer array data from target process
bool ProcessPool::receive(int *source, int nData, int sourceRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    MPI_Status status;
    if (MPI_Recv(source, nData, MPI_INTEGER, sourceRank, 0, communicator(commType), &status) != MPI_SUCCESS)
        return false;
    timer_.accumulate();
#endif
    return true;
}

// Send double array data to target process
bool ProcessPool::send(double *source, int nData, int targetRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if (MPI_Send(source, nData, MPI_DOUBLE, targetRank, 0, communicator(commType)) != MPI_SUCCESS)
        return false;
    timer_.accumulate();
#endif
    return true;
}

// Receive double array data from target process
bool ProcessPool::receive(double *source, int nData, int sourceRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    MPI_Status status;
    if (MPI_Recv(source, nData, MPI_DOUBLE, sourceRank, 0, communicator(commType), &status) != MPI_SUCCESS)
        return false;
    timer_.accumulate();
#endif
    return true;
}

/*
 * Broadcast Functions
 */

// Broadcast std::string
bool ProcessPool::broadcast(std::string &source, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    int length;
    if (poolRank_ == rootRank)
    {
        // Broadcast string length first...
        length = source.size();
        if (MPI_Bcast(&length, 1, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
        {
            Messenger::print("Failed to broadcast char length data from root rank {}.\n", rootRank);
            return false;
        }

        // Now broadcast character data
        if (MPI_Bcast(const_cast<char *>(source.data()), length, MPI_CHARACTER, rootRank, communicator(commType)) !=
            MPI_SUCCESS)
        {
            Messenger::print("Failed to broadcast char data from root rank {}.\n", rootRank);
            return false;
        }
    }
    else
    {
        // Slaves receive the data into the buffer, and then set the source string.
        // Length first...
        if (MPI_Bcast(&length, 1, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
        {
            Messenger::print("Slave {} (world rank {}) failed to receive char length data from root rank {}.\n", poolRank_,
                             worldRank_, rootRank);
            return false;
        }
        source.resize(length);
        if (MPI_Bcast(const_cast<char *>(source.data()), length, MPI_CHARACTER, rootRank, communicator(commType)) !=
            MPI_SUCCESS)
        {
            Messenger::print("Slave {} (world rank {}) failed to receive char data from root rank {}.\n", poolRank_, worldRank_,
                             rootRank);
            return false;
        }
    }
    timer_.accumulate();
#endif
    return true;
}

// Broadcast char data to all Processes
bool ProcessPool::broadcast(char *source, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    int length;
    if (poolRank_ == rootRank)
    {
        // Broadcast string length first...
        length = strlen(source) + 1;
        if (MPI_Bcast(&length, 1, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
        {
            Messenger::print("Failed to broadcast char length data from root rank {}.\n", rootRank);
            return false;
        }

        // Now broadcast character data
        if (MPI_Bcast(source, length, MPI_CHARACTER, rootRank, communicator(commType)) != MPI_SUCCESS)
        {
            Messenger::print("Failed to broadcast char data from root rank {}.\n", rootRank);
            return false;
        }
    }
    else
    {
        // Slaves receive the data into the buffer, and then set the source string.
        // Length first...
        if (MPI_Bcast(&length, 1, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
        {
            Messenger::print("Slave {} (world rank {}) failed to receive char length data from root rank {}.\n", poolRank_,
                             worldRank_, rootRank);
            return false;
        }

        if (MPI_Bcast(source, length, MPI_CHARACTER, rootRank, communicator(commType)) != MPI_SUCCESS)
        {
            Messenger::print("Slave {} (world rank {}) failed to receive char data from root rank {}.\n", poolRank_, worldRank_,
                             rootRank);
            return false;
        }
    }
    timer_.accumulate();
#endif
    return true;
}

// Broadcast single integer
bool ProcessPool::broadcast(int &source, int rootRank, ProcessPool::CommunicatorType commType)
{
    return broadcast(&source, 1, rootRank, commType);
}

// Broadcast integer(s) to all Processes
bool ProcessPool::broadcast(int *source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if (MPI_Bcast(source, count, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
    {
        Messenger::print("Failed to broadcast int data from root rank {}.\n", rootRank);
        return false;
    }
    timer_.accumulate();
#endif
    return true;
}

// Broadcast char(s) to all Processes
bool ProcessPool::broadcast(char *source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if (MPI_Bcast(source, count, MPI_CHAR, rootRank, communicator(commType)) != MPI_SUCCESS)
    {
        Messenger::print("Failed to broadcast char array data from root rank {}.\n", rootRank);
        return false;
    }
    timer_.accumulate();
#endif
    return true;
}

// Broadcast long integer to all Processes
bool ProcessPool::broadcast(long int &source, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if (MPI_Bcast(&source, 1, MPI_LONG, rootRank, communicator(commType)) != MPI_SUCCESS)
    {
        Messenger::print("Failed to broadcast long int data from root rank {}.\n", rootRank);
        return false;
    }
    timer_.accumulate();
#endif
    return true;
}

// Broadcast long integer to all Processes
bool ProcessPool::broadcast(long int *source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if (MPI_Bcast(source, count, MPI_LONG, rootRank, communicator(commType)) != MPI_SUCCESS)
    {
        Messenger::print("Failed to broadcast long int data from root rank {}.\n", rootRank);
        return false;
    }
    timer_.accumulate();
#endif
    return true;
}

// Broadcast single double
bool ProcessPool::broadcast(double &source, int rootRank, ProcessPool::CommunicatorType commType)
{
    return broadcast(&source, 1, rootRank, commType);
}

// Broadcast double(s) to all Processes
bool ProcessPool::broadcast(double *source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if (MPI_Bcast(source, count, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
    {
        Messenger::print("Failed to broadcast int data from root rank {}.\n", rootRank);
        return false;
    }
    timer_.accumulate();
#endif
    return true;
}

// Broadcast float(s) to all Processes
bool ProcessPool::broadcast(float *source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if (MPI_Bcast(source, count, MPI_FLOAT, rootRank, communicator(commType)) != MPI_SUCCESS)
    {
        Messenger::print("Failed to broadcast float data from root rank {}.\n", rootRank);
        return false;
    }
    timer_.accumulate();
#endif

    return true;
}

// Broadcast bool to all Processes
bool ProcessPool::broadcast(bool &source, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    int result = (source ? 1 : 0);
    if (MPI_Bcast(&result, 1, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
    {
        Messenger::print("Failed to broadcast int data from root rank {}.\n", rootRank);
        return false;
    }
    source = result;
    timer_.accumulate();
#endif
    return true;
}

// Broadcast Array<int>
bool ProcessPool::broadcast(Array<int> &array, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();

    int length;
    if (poolRank_ == rootRank)
    {
        // Broadcast array length first...
        length = array.nItems();
        if (MPI_Bcast(&length, 1, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
        {
            Messenger::print("Failed to broadcast Array<int> size from root rank {} (world rank {}).\n", rootRank,
                             worldRanks_[rootRank]);
            return false;
        }

        // Now broadcast Array data
        if (length > 0)
        {
            if (MPI_Bcast(array.array(), length, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
            {
                Messenger::print("Failed to broadcast Array<int> data from root rank {} (world rank {}).\n", rootRank,
                                 worldRanks_[rootRank]);
                return false;
            }
        }
    }
    else
    {
        // Slaves receive the length, and then create and receive the array
        // Length first...
        if (MPI_Bcast(&length, 1, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
        {
            Messenger::print("Slave {} (world rank {}) failed to receive Array<int> size from root rank {}.\n", poolRank_,
                             worldRank_, rootRank);
            return false;
        }

        if (length > 0)
        {
            // Create array of specified size
            array.initialise(length);

            if (MPI_Bcast(array.array(), length, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
            {
                Messenger::print("Slave {} (world rank {}) failed to receive Array<int> data from root rank {}.\n", poolRank_,
                                 worldRank_, rootRank);
                return false;
            }
        }
        else
            array.clear();
    }

    timer_.accumulate();
#endif
    return true;
}

// Broadcast std::vector<int>
bool ProcessPool::broadcast(std::vector<int> &array, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();

    if (!broadcast(array.data(), array.size(), rootRank, commType))
    {
        Messenger::print("Failed to broadcast std::vector<int> data from root rank {} (world rank {}).\n", rootRank,
                         worldRanks_[rootRank]);
        return false;
    }

    timer_.accumulate();
#endif
    return true;
}

// Broadcast std::vector<double>
bool ProcessPool::broadcast(std::vector<double> &array, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();

    if (!broadcast(array.data(), array.size(), rootRank, commType))
    {
        Messenger::print("Failed to broadcast std::vector<double> data from root rank {} (world rank {}).\n", rootRank,
                         worldRanks_[rootRank]);
        return false;
    }

    timer_.accumulate();
#endif
    return true;
}

/*
 * Special Array Functions
 */

// Reduce (sum) double data to root rank
bool ProcessPool::sum(double *source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    // If we are the target process then we need to construct a temporary buffer to store the received data in.
    if ((commType == ProcessPool::GroupLeadersCommunicator) && (!groupLeader()))
        return true;
    if (poolRank_ == rootRank)
    {
        double buffer[count];
        if (MPI_Reduce(source, buffer, count, MPI_DOUBLE, MPI_SUM, rootRank, communicator(commType)) != MPI_SUCCESS)
            return false;
        // Put reduced data back into original buffer
        std::copy(buffer, buffer + count, source);
    }
    else
    {
        // Not the target process, so just send data
        if (MPI_Reduce(source, nullptr, count, MPI_DOUBLE, MPI_SUM, rootRank, communicator(commType)) != MPI_SUCCESS)
            return false;
    }
    timer_.accumulate();
#endif
    return true;
}

// Reduce (sum) int data to root rank
bool ProcessPool::sum(int *source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    // If we are the target process then we need to construct a temporary buffer to store the received data in.
    if ((commType == ProcessPool::GroupLeadersCommunicator) && (!groupLeader()))
        return true;
    if (poolRank_ == rootRank)
    {
        int buffer[count];
        if (MPI_Reduce(source, buffer, count, MPI_INTEGER, MPI_SUM, rootRank, communicator(commType)) != MPI_SUCCESS)
            return false;
        // Put reduced data back into original buffer
        std::copy(buffer, buffer + count, source);
    }
    else
    {
        // Not the target process, so just send data
        if (MPI_Reduce(source, nullptr, count, MPI_INTEGER, MPI_SUM, rootRank, communicator(commType)) != MPI_SUCCESS)
            return false;
    }
    timer_.accumulate();
#endif
    return true;
}

// Reduce (sum) double data to all processes
bool ProcessPool::allSum(double *source, int count, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    double buffer[count];
    if ((commType == ProcessPool::GroupLeadersCommunicator) && (!groupLeader()))
        return true;
    if (MPI_Allreduce(source, &buffer, count, MPI_DOUBLE, MPI_SUM, communicator(commType)) != MPI_SUCCESS)
        return false;
    // Put reduced data back into original buffer
    std::copy(buffer, buffer + count, source);
    timer_.accumulate();
#endif
    return true;
}

// Reduce (sum) vector of Vec3<double> data to all processes
bool ProcessPool::allSum(std::vector<Vec3<double>> &source, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    if ((commType == ProcessPool::GroupLeadersCommunicator) && (!groupLeader()))
        return true;

    // Construct POD buffers that we can send via MPI
    std::vector<double> buffer(source.size()), received(source.size());
    for (auto n = 0; n < 3; ++n)
    {
        std::fill(received.begin(), received.end(), 0.0);

        // Copy elements (x, y, or z) to buffer
        std::transform(source.begin(), source.end(), buffer.begin(), [n](const auto &val) { return val.get(n); });

        // Reduce
        if (MPI_Allreduce(buffer.data(), received.data(), received.size(), MPI_DOUBLE, MPI_SUM, communicator(commType)) !=
            MPI_SUCCESS)
            return false;

        // Put reduced data back into source
        for (auto &&[rcvd, val] : zip(received, source))
            val[n] = rcvd;
    }

    timer_.accumulate();
#endif
    return true;
}

// Reduce (sum) int data to all processes
bool ProcessPool::allSum(int *source, int count, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    int buffer[count];
    std::fill(buffer, buffer + count, 0);

    if ((commType == ProcessPool::GroupLeadersCommunicator) && (!groupLeader()))
        return true;
    if (MPI_Allreduce(source, &buffer, count, MPI_INTEGER, MPI_SUM, communicator(commType)) != MPI_SUCCESS)
        return false;

    // Put reduced data back into original buffer
    std::copy(buffer, buffer + count, source);
    timer_.accumulate();
#endif
    return true;
}

// Reduce (sum) long int data to all processes
bool ProcessPool::allSum(long int *source, int count, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    timer_.start();
    long int buffer[count];
    if ((commType == ProcessPool::GroupLeadersCommunicator) && (!groupLeader()))
        return true;
    if (MPI_Allreduce(source, &buffer, count, MPI_LONG, MPI_SUM, communicator(commType)) != MPI_SUCCESS)
        return false;

    // Put reduced data back into original buffer
    std::copy(buffer, buffer + count, source);
    timer_.accumulate();
#endif
    return true;
}

// Reduce (sum) double data over processes relevant to specified strategy
bool ProcessPool::allSum(double *source, int count, ProcessPool::DivisionStrategy strategy)
{
#ifdef PARALLEL
    timer_.start();
    if (strategy == ProcessPool::GroupsStrategy)
    {
        // Sum values from group leaders, then broadcast to other processes in their respective groups
        if (!allSum(source, count, ProcessPool::GroupLeadersCommunicator))
            return false;
        return broadcast(source, count, 0, ProcessPool::GroupProcessesCommunicator);
    }
    else if (strategy == ProcessPool::GroupProcessesStrategy)
    {
        // Sum values over processes within group
        return allSum(source, count, ProcessPool::GroupProcessesCommunicator);
    }
    else if (strategy == ProcessPool::PoolStrategy)
    {
        return allSum(source, count, ProcessPool::PoolProcessesCommunicator);
    }
    else if (strategy == ProcessPool::PoolProcessesStrategy)
    {
        // No need to perform any summation
    }
    timer_.accumulate();
#endif
    return true;
}

// Reduce (sum) int data over processes relevant to specified strategy
bool ProcessPool::allSum(int *source, int count, ProcessPool::DivisionStrategy strategy)
{
#ifdef PARALLEL
    timer_.start();
    if (strategy == ProcessPool::GroupsStrategy)
    {
        // Sum values from group leaders, then broadcast to other processes in their respective groups
        if (!allSum(source, count, ProcessPool::GroupLeadersCommunicator))
            return false;
        return broadcast(source, count, 0, ProcessPool::GroupProcessesCommunicator);
    }
    else if (strategy == ProcessPool::GroupProcessesStrategy)
    {
        // Sum values over processes within group
        return allSum(source, count, ProcessPool::GroupProcessesCommunicator);
    }
    else if (strategy == ProcessPool::PoolStrategy)
    {
        return allSum(source, count, ProcessPool::PoolProcessesCommunicator);
    }
    else if (strategy == ProcessPool::PoolProcessesStrategy)
    {
        // No need to perform any summation
    }
    timer_.accumulate();
#endif
    return true;
}

// Assemble integer array for entire pool on target process
bool ProcessPool::assemble(int *array, int nData, int *rootDest, int rootMaxData, int rootRank,
                           ProcessPool::CommunicatorType commType)
{
    /*
     * Given that the integer 'array' exists on all processes, and each process has stored nData at the
     * beginning of this array, this function will assemble the data into a complete, continuous array (up to
     * maxData elements in the new array rootDest) on the rootRank.
     */
#ifdef PARALLEL
    timer_.start();
    if (poolRank_ == rootRank)
    {
        int n;
        // The root's data must be copied into the local array
        for (n = 0; n < nData; ++n)
            rootDest[n] = array[n];

        // Now get data from other processes, appending each chunk to the rootDest array
        int slaveNData;
        for (auto n = 0; n < worldRanks_.nItems(); ++n)
        {
            if (poolRank_ == n)
                continue;

            // Get length of data to receive
            if (!receive(slaveNData, n, commType))
                return false;

            // Check buffer length
            if ((nData + slaveNData) > rootMaxData)
            {
                Messenger::error("Tried to assemble more data on process '{}' than the array allows for "
                                 "(maxData = {}).\n",
                                 rootRank, rootMaxData);
                return false;
            }
            // Receive data
            if (!receive(&rootDest[nData], slaveNData, n, commType))
                return false;
            nData += slaveNData;
        }
    }
    else
    {
        // Slaves send array size and then the data to the master...
        if (!send(nData, rootRank, commType))
            return false;
        if (!send(array, nData, rootRank, commType))
            return false;
    }
    timer_.accumulate();
#endif
    return true;
}

// Assemble double array for entire pool on target process
bool ProcessPool::assemble(double *array, int nLocalData, double *rootDest, int rootMaxData, int rootRank,
                           ProcessPool::CommunicatorType commType)
{
    /*
     * Given that the double 'array' exists on all processes, and each process has stored nData at the
     * beginning of this array, this function will assemble the data into a complete, continuous array (up to
     * maxData elements in the new array rootDest) on the root.
     */
#ifdef PARALLEL
    timer_.start();
    if (poolRank_ == rootRank)
    {
        // The root's data must be copied into the local array
        for (auto n = 0; n < nLocalData; ++n)
            rootDest[n] = array[n];

        // Now get data from other processes, appending each chunk to the rootDest array
        int slaveNData;
        for (auto n = 0; n < worldRanks_.nItems(); ++n)
        {
            if (poolRank_ == n)
                continue;

            // Get length of data to receive
            if (!receive(slaveNData, n, commType))
                return false;

            // Check buffer length
            if ((nLocalData + slaveNData) > rootMaxData)
            {
                Messenger::error("Tried to assemble more data on process '{}' than the array allows for "
                                 "(maxData = {}).\n",
                                 rootRank, rootMaxData);
                return false;
            }
            // Receive data
            if (!receive(&rootDest[nLocalData], slaveNData, n, commType))
                return false;
            nLocalData += slaveNData;
        }
    }
    else
    {
        // Slaves send array size and then the data to the master...
        if (!send(nLocalData, rootRank, commType))
            return false;
        if (!send(array, nLocalData, rootRank, commType))
            return false;
    }
    timer_.accumulate();
#endif
    return true;
}

// Assemble Array<double> for entire pool on target process
bool ProcessPool::assemble(Array<double> &array, int nData, Array<double> &rootDest, int rootMaxData, int rootRank,
                           ProcessPool::CommunicatorType commType)
{
    if (poolRank_ == rootRank)
    {
        if (rootDest.size() < rootMaxData)
        {
            Messenger::error("Destination Array<double> in ProcessPool::assemble() is not large enough.");
            decideFalse(rootRank, commType);
            return false;
        }
        else
            decideTrue(rootRank, commType);
    }
    else if (!decision(rootRank, commType))
        return false;

    // Call double* version of routine...
    return assemble(array.array(), nData, rootDest.array(), rootMaxData, rootRank, commType);
}

/*
 * Decisions
 */

// Broadcast logical 'true' decision to all processes (Master only)
bool ProcessPool::decideTrue(int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    auto decision = true;
    if (!broadcast(decision, rootRank, commType))
        return Messenger::error("Error telling processes to proceed.\n");
#endif
    return true;
}

// Broadcast logical 'false' decision to all processes (Master only)
bool ProcessPool::decideFalse(int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    auto decision = false;
    if (!broadcast(decision, rootRank, commType))
        return Messenger::error("Error telling processes to stop.\n");
#endif
    return true;
}

// Receive logical decision from master (Slaves only)
bool ProcessPool::decision(int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    bool data;
    if (!broadcast(data, rootRank, commType))
        Messenger::error("Error receiving decision from root rank {}.\n", rootRank);
    return data;
#endif
    return true;
}

// Test the supplied condition over all processes, returning true only if they all report truth
bool ProcessPool::allTrue(bool value, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
    // First, sum all bool values of the processes in the pool
    auto summedResult = (value ? 1 : 0);
    if (!allSum(&summedResult, 1, commType))
        return false;
    if (commType == ProcessPool::GroupLeadersCommunicator)
        return (summedResult == groupLeaders_.nItems());
    else
        return (summedResult == nProcesses());
#endif
    return value;
}

/*
 * Buffered Random Numbers
 */

// Refill random number buffer
void ProcessPool::refillRandomBuffer()
{
#ifdef PARALLEL
    // Reset index
    randomBufferIndex_ = 0;

    // Generate new random numbers in array
    if (randomBufferCommGroup_ == ProcessPool::PoolProcessesCommunicator)
    {
        // Master creates buffer and sends to slaves
        Messenger::printVerbose("Random Buffer - Pool parallel, so master ({}) will create and send array.\n",
                                isMaster() ? "me" : "not me");
        if (isMaster())
        {
            for (auto n = 0; n < RANDBUFFERSIZE; ++n)
                randomBuffer_[n] = DissolveMath::random();
            broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
        }
        else
            broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
    }
    else if (randomBufferCommGroup_ == ProcessPool::GroupLeadersCommunicator)
    {
        // Master creates buffer and sends to slaves
        Messenger::printVerbose("Random Buffer - Group leaders parallel, so master ({}) will create and send array.\n",
                                isMaster() ? "me" : "not me");
        if (isMaster())
        {
            for (auto n = 0; n < RANDBUFFERSIZE; ++n)
                randomBuffer_[n] = DissolveMath::random();
            broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
        }
        else
            broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
    }
    else if (randomBufferCommGroup_ == ProcessPool::GroupProcessesCommunicator)
    {
        // Group leader creates buffer and sends to slaves
        Messenger::printVerbose("Random Buffer - Group parallel, so process leader ({}) will create and send array.\n",
                                groupLeader() ? "me" : "not me");
        if (groupLeader())
        {
            for (auto n = 0; n < RANDBUFFERSIZE; ++n)
                randomBuffer_[n] = DissolveMath::random();
            broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
        }
        else
            broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
    }
    else
    {
        // Create own random buffer
        Messenger::printVerbose("Random Buffer - Solo parallel, so will create own array.\n");
        for (auto n = 0; n < RANDBUFFERSIZE; ++n)
            randomBuffer_[n] = DissolveMath::random();
    }
#endif
}

// Initialise random number buffer
void ProcessPool::initialiseRandomBuffer(ProcessPool::CommunicatorType commType)
{
    randomBufferCommGroup_ = commType;
    refillRandomBuffer();
}

// Initialise random number buffer for processes
void ProcessPool::initialiseRandomBuffer(ProcessPool::DivisionStrategy strategy)
{
    switch (strategy)
    {
        case (GroupsStrategy):
            randomBufferCommGroup_ = ProcessPool::GroupLeadersCommunicator;
            break;
        case (GroupProcessesStrategy):
            randomBufferCommGroup_ = ProcessPool::GroupProcessesCommunicator;
            break;
        case (PoolStrategy):
            randomBufferCommGroup_ = ProcessPool::PoolProcessesCommunicator;
            break;
        case (PoolProcessesStrategy):
            randomBufferCommGroup_ = ProcessPool::NoCommunicator;
            break;
    }
    refillRandomBuffer();
}

// Get next buffered random number
double ProcessPool::random()
{
#ifdef PARALLEL
    // Have we exhausted the buffer?
    if (randomBufferIndex_ == RANDBUFFERSIZE)
        refillRandomBuffer();
    return randomBuffer_[randomBufferIndex_++];
#else
    return DissolveMath::random();
#endif
}

// Get next buffered random number (-1 to +1 inclusive)
double ProcessPool::randomPlusMinusOne()
{
#ifdef PARALLEL
    // Have we exhausted the buffer?
    if (randomBufferIndex_ == RANDBUFFERSIZE)
        refillRandomBuffer();
    return (randomBuffer_[randomBufferIndex_++] - 0.5) * 2.0;
#else
    return DissolveMath::randomPlusMinusOne();
#endif
}
