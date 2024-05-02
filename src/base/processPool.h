// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

// Random number buffer size
#define RANDBUFFERSIZE 16172

#include "base/processGroup.h"
#include "base/timer.h"
#include "templates/optionalRef.h"
#include "templates/vector3.h"
// Include <mpi.h> only if we are compiling in parallel
#ifdef PARALLEL
#include <mpi.h>
#endif

// ProcessPool
class ProcessPool
{
    /*
     * Class to define a 'pool' of processes that should work together in parallel tasks, providing limits per process,
     * and macros to simplify coding and aid readability.
     */
    public:
    ProcessPool();
    // Communicator Types (for subroutines)
    enum CommunicatorType
    {
        GroupProcessesCommunicator, /* Process groups (communicator == groupCommunicator_) */
        GroupLeadersCommunicator,   /* A group containing all process group leaders in the local pool (communicator ==
                           leaderCommunicator_) */
        PoolProcessesCommunicator,  /* A group containing all processes in the local pool (communicator ==
                           poolCommunicator_) */
        NoCommunicator              /* No communicator */
    };

#ifdef PARALLEL
    // Return communicator for group specified
    MPI_Comm communicator(ProcessPool::CommunicatorType commType) const;
#endif

    /*
     * Global Information
     */
    private:
    // Number of processes in total
    static int nWorldProcesses_;
    // World rank of this process
    static int worldRank_;

    public:
    // Initialise parallel communications, setting rank within the specified communicators / nProcesses
    static bool initialiseMPI(int *argn, char ***argv);
    // End parallel communications
    static void finalise();
    // Return number of world processes
    static int nWorldProcesses();
    // Return rank within the specified communicator of this process
    static int worldRank();
    // Return if this is the world master process
    static bool isWorldMaster();

    /*
     * Process Identification
     */
    private:
    // Local rank of this process in the pool (corresponding to array index in worldRanks_)
    int poolRank_;
    // Index of local group in which this process exists
    int groupIndex_;
    // Rank of this process in its local group
    int groupRank_;

    public:
    // Return rank of this process in the pool
    int poolRank() const;
    // Return whether this process is the master for the specified communicator
    bool isMaster(ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator) const;
    // Return whether this process is a local slave for the specified communicator
    bool isSlave(ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator) const;
    // Return whether this process is the pool index specified
    bool isMe(int poolIndex) const;
    // Return whether this pool involves this process
    bool involvesMe() const;
    // Return group index in which this process exists
    int groupIndex() const;
    // Return local group in which this process exists
    ProcessGroup &myGroup();
    // Return rank of this process in its local group
    int groupRank() const;
    // Return whether this process is a group leader
    bool groupLeader() const;

    /*
     * Pool Data
     */
    public:
    // Group Population Enum
    enum GroupPopulation
    {
        HalfMaximumGroupPopulation = -1,
        MaximumGroupPopulation = 0,
        MinimumGroupPopulation = 1
    };

    private:
    // Name of this pool
    std::string name_;
    // Array of world ranks in this pool
    std::vector<int> worldRanks_;
    // Vector of process groups within the pool, referencing pool ranks of processes
    std::vector<ProcessGroup> processGroups_;
    // Maximum number of simultaneous process groups
    int maxProcessGroups_;
    // Pool ranks of process group leaders
    std::vector<int> groupLeaders_;
#ifdef PARALLEL
    // Group of all processes in this pool
    MPI_Group poolGroup_;
    // Communicator for all processes in this pool
    MPI_Comm poolCommunicator_;
    // Group group for this process
    MPI_Group groupGroup_;
    // Group communicator for this process
    MPI_Comm groupCommunicator_;
    // Process leaders group
    MPI_Group leaderGroup_;
    // Process leaders communicator
    MPI_Comm leaderCommunicator_;
#endif
    // Whether group data is modifiable
    bool groupsModifiable_;

    public:
    // Set up pool with processes specified
    bool setUp(std::string_view name, const std::vector<int> &worldRanks);
    // Return name of pool
    std::string_view name() const;
    // Return total number of processes in pool
    int nProcesses() const;
    // Return root (first) world rank of this pool
    int rootWorldRank() const;
    // Assign processes to groups
    bool assignProcessesToGroups();
    // Return number of process groups
    int nProcessGroups() const;
    // Return nth process group
    ProcessGroup &processGroup(int n);
    // Return number of processes in specified group
    int nProcessesInGroup(int groupId) const;
    // Return array of pool ranks in the specified group
    const std::vector<int> &poolRanksInGroup(int groupId) const;
    // Return whether group data is modifiable
    bool groupsModifiable() const;
    // Prevent group data from being modified
    void setGroupsFixed();
    // Return maximum number of simultaneous, single-Cell-modifying process groups
    int maxProcessGroups() const;

    /*
     * Strategy / Limits
     */
    public:
    // Division of Labour Style
    enum DivisionStrategy
    {
        GroupsStrategy = 16,        /* Divide loop work over process groups within pool */
        GroupProcessesStrategy = 8, /* Divide loop work over processes within a group */
        PoolStrategy = 4,           /* Divide loop work over individual processes in pool */
        PoolProcessesStrategy = 2   /* Single process handles entire loop */
    };
    // Return sub-strategy for specified dividion of labour strategy
    static DivisionStrategy subDivisionStrategy(DivisionStrategy strategy);
    // Return starting index for loop using specified strategy
    int interleavedLoopStart(ProcessPool::DivisionStrategy strategy) const;
    // Return stride for interleaved loop using specified strategy
    int interleavedLoopStride(ProcessPool::DivisionStrategy strategy) const;
    // Return 'size' for specified strategy
    int strategyNDivisions(DivisionStrategy strategy) const;
    // Return index of this process within the specified strategy
    int strategyProcessIndex(DivisionStrategy strategy) const;
    // Return best strategy (by process or by pool) for this process pool
    ProcessPool::DivisionStrategy bestStrategy() const;
    // Return starting outer loop index for a two-body interaction calculation where only the upper half (i >= j) is
    // required
    int twoBodyLoopStart(int nItems) const;
    // Return ending outer loop index for a two-body interaction calculation where only the upper half (i >= j) is required
    int twoBodyLoopEnd(int nItems) const;

    /*
     * Send/Receive Functions
     */
    public:
    // Wait for all processes
    bool wait(ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator) const;
    // Send single integer value to target rank within the specified communicator
    bool send(int value, int targetRank, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
              OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Receive single integer from source rank within the specified communicator
    bool receive(int &value, int sourceRank, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                 OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Send single long integer value to target rank within the specified communicator
    bool send(long int value, int targetRank, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
              OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Receive single long integer from source rank within the specified communicator
    bool receive(long int &value, int sourceRank,
                 ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                 OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Send single double value to target rank within the specified communicator
    bool send(double value, int targetRank, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
              OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Receive single double value from source rank within the specified communicator
    bool receive(double &value, int sourceRank, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                 OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Send single bool value to target rank within the specified communicator
    bool send(bool value, int targetRank, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
              OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Receive single bool value from source rank within the specified communicator
    bool receive(bool &value, int sourceRank, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                 OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Send integer array data to target rank within the specified communicator
    bool send(int *source, int nData, int targetRank,
              ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
              OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Receive integer array data from target rank within the specified communicator
    bool receive(int *source, int nData, int sourceRank,
                 ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                 OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Send double array data to target rank within the specified communicator
    bool send(double *source, int nData, int targetRank,
              ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
              OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Receive double array data from target rank within the specified communicator
    bool receive(double *source, int nData, int sourceRank,
                 ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                 OptionalReferenceWrapper<Timer> timer = std::nullopt) const;

    /*
     * Broadcast Functions
     */
    public:
    // Broadcast std::string
    bool broadcast(std::string &source, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast char data
    bool broadcast(char *source, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast single integer
    bool broadcast(int &source, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast integers
    bool broadcast(int *source, int count, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast chars
    bool broadcast(char *source, int count, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast single long integer
    bool broadcast(long int &source, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast long integer to all Processes
    bool broadcast(long int *source, int count, int rootRank,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast single double
    bool broadcast(double &source, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast double(s)
    bool broadcast(double *source, int count, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast float(s)
    bool broadcast(float *source, int count, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast bool
    bool broadcast(bool &source, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast std::vector<int>
    bool broadcast(std::vector<int> &array, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Broadcast std::vector<double>
    bool broadcast(std::vector<double> &array, int rootRank = 0,
                   ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                   OptionalReferenceWrapper<Timer> timer = std::nullopt) const;

    /*
     * Special Array Functions
     */
    public:
    // Reduce (sum) double data to root process
    bool sum(double *source, int count, int rootRank = 0,
             ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
             OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Reduce (sum) int data to root process
    bool sum(int *source, int count, int rootRank = 0,
             ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
             OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Reduce (sum) double data to all processes
    bool allSum(double *source, int count, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Reduce (sum) vector of Vec3<double> data to all processes
    bool allSum(std::vector<Vec3<double>> &source,
                ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Reduce (sum) int data to all processes
    bool allSum(int *source, int count, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Reduce (sum) int data to all processes
    bool allSum(long int *source, int count, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Reduce (sum) double data over processes relevant to specified strategy
    bool allSum(double *source, int count, ProcessPool::DivisionStrategy strategy,
                OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Reduce (sum) int data over processes relevant to specified strategy
    bool allSum(int *source, int count, ProcessPool::DivisionStrategy strategy,
                OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Assemble integer array on target rank within the specified communicator
    bool assemble(int *array, int nData, int *rootDest, int rootMaxData, int rootRank = 0,
                  ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                  OptionalReferenceWrapper<Timer> timer = std::nullopt) const;
    // Assemble double array on target rank within the specified communicator
    bool assemble(double *array, int nLocalData, double *rootDest, int rootMaxData, int rootRank = 0,
                  ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator,
                  OptionalReferenceWrapper<Timer> timer = std::nullopt) const;

    /*
     * Decisions
     */
    public:
    // Broadcast logical 'true' decision to processes (Master only)
    bool decideTrue(int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator) const;
    // Broadcast logical 'false' decision to processes (Master only)
    bool decideFalse(int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator) const;
    // Receive logical decision from master (Slaves only)
    bool decision(int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator) const;
    // Test the supplied condition over all processes, returning true only if they all report truth
    bool allTrue(bool isOK, ProcessPool::CommunicatorType commType = ProcessPool::PoolProcessesCommunicator) const;

    /*
     * Macro Variables
     */
    public:
    // Failure return result buffer
    static int FAILED;
    // Sccess return result buffer
    static int SUCCEEDED;
    // Accessible return result buffer for slave processes
    static int RESULT;
};

/*
 * Macro Definitions
 */

/*
 * MPIRunMaster(x)
 * Effectively performs the following code:
 * if (ProcessPool::isWorldMaster())
 * {
 *	// Test supplied code/condition 'x', and broadcast result to all processes
 *	if (x)
 * 	{
 * 		MPI_Bcast(&ProcessPool::SUCCEEDED,1,MPI_INTEGER,0,MPI_COMM_WORLD);
 * 		return true;
 * 	}
 * 	else
 * 	{
 * 		MPI_Bcast(&ProcessPool::FAILED,1,MPI_INTEGER,0,MPI_COMM_WORLD);
 *		return false;
 * 	}
 * }
 * else
 * {
 *	// Slaves don't evaluate 'x' - they just receive the result
 * 	MPI_Bcast(&ProcessPool::RESULT,1,MPI_INTEGER,0,MPI_COMM_WORLD);
 * 	return ProcessPool::RESULT;
 * }
 */
#ifdef PARALLEL
#define MPIRunMaster(pool, x)                                                                                                  \
    (pool.isMaster() ? (x ? (pool.decideTrue(), true) : (pool.decideFalse(), false)) : pool.decision())
#else
#define MPIRunMaster(pool, x) x
#endif
