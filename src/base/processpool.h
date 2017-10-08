/*
	*** Process Pool
	*** src/base/processpool.h
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_PROCESSPOOL_H
#define DUQ_PROCESSPOOL_H

// Random number buffer size
#define RANDBUFFERSIZE 16172

#include "base/processgroup.h"
#include "base/charstring.h"
#include "base/timer.h"
#include "templates/vector3.h"
#include "templates/array.h"
#include "templates/array2d.h"
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
	// Constructor
	ProcessPool();
	// Copy Constructor
	ProcessPool(const ProcessPool& source);
	// Assignment Operator
	void operator=(const ProcessPool& source);
	// Communicator Types (for subroutines)
	enum CommunicatorType
	{
		Group,		/* Process groups (communicator == groupCommunicator_) */
		Leaders,	/* A group containing all process group leaders in the local pool (communicator == leaderCommunicator_) */
		Pool		/* A group containing all processes in the local pool (communicator == poolCommunicator_) */
	};
	// Interleaved Loop Contexts
	enum LoopContext
	{
		Individual,
		OverGroups,
		OverGroupProcesses,
		OverPoolProcesses
	};
	
#ifdef PARALLEL
	// Return communicator for group specified
	MPI_Comm communicator(ProcessPool::CommunicatorType commType);
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
	static bool initialiseMPI(int* argn, char ***argv);
	// End parallel communications
	static bool finalise();
	// Return number of world processes
	static int nWorldProcesses();
	// Return rank within the specified communicator of this process
	static int worldRank();
	// Return if this is the world master process
	static bool isWorldMaster();


	/*
	 * Timing
	 */
	private:
	// Communication Timer
	Timer timer_;

	public:
	// Reset accumulated Comm time
	void resetAccumulatedTime();
	// Return accumulated time string
	const char* accumulatedTimeString();


	/*
	 * Process Identification
	 */
	private:
	// Local rank of this process in the pool (corrsponding to array index in worldRanks_)
	int poolRank_;
	// Index of local group in which this process exists
	int groupIndex_;
	// Rank of this process in its local group
	int groupRank_;

	public:
	// Return rank of this process in the pool
	int poolRank();
	// Return whether this process is the master for the specified communicator
	bool isMaster(ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Return whether this process is a local slave for the specified communicator
	bool isSlave(ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Return whether this process is the pool index specified
	bool isMe(int poolIndex);
	// Return whether this pool involves this process
	bool involvesMe();
	// Return group index in which this process exists
	int groupIndex();
	// Return local group in which this process exists
	ProcessGroup* myGroup();
	// Return rank of this process in its local group
	int groupRank();
	// Return whether this process is a group leader
	bool groupLeader();
	// Return process info string
	const char* processInfo();


	/*
	 * Pool Data
	 */
	private:
	// Name of this pool
	CharString name_;
	// List of world ranks in this pool
	Array<int> worldRanks_;
	// List of process groups within the pool, referencing pool ranks of processes
	List<ProcessGroup> processGroups_;
	// Maximum number of simultaneous process groups
	int maxProcessGroups_;
	// Pool ranks of process group leaders
	Array<int> groupLeaders_;
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
	// Setup pool with processes specified
	bool setup(const char* name, Array<int> worldRanks);
	// Return name of pool
	const char* name();
	// Return total number of processes in pool
	int nProcesses();
	// Return root (first) world rank of this pool
	int rootWorldRank();
	// Determine how many simultaneous processes (groups) we can have at once, based on the Cell divisions
	void determineMaxProcessGroups(const Vec3<int>& divisions, const Vec3<int>& cellExtents, const List< ListVec3<int> >& neighbours);
	// Assign processes to groups
	bool assignProcessesToGroups();
	// Assign processes to groups taken from supplied ProcessPool
	bool assignProcessesToGroups(ProcessPool& groupsSource);
	// Return number of process groups
	int nProcessGroups() const;
	// Return nth process group
	ProcessGroup* processGroup(int n);
	// Return number of processes in specified group
	int nProcessesInGroup(int groupId);
	// Return array of pool ranks in the specified group
	int* poolRanksInGroup(int groupId);
	// Return whether group data is modifiable
	bool groupsModifiable();
	// Prevent group data from being modified
	void setGroupsFixed();


	/*
	 * Local Process Limits
	 */
	public:
	// Return starting index for general interleaved loop
	int interleavedLoopStart(ProcessPool::LoopContext loopContext);
	// Return stride for general interleaved loop
	int interleavedLoopStride(ProcessPool::LoopContext loopContext);
	// Return starting outer loop index for a two-body interaction calculation where only the upper half (i >= j) is required
	int twoBodyLoopStart(int nItems);
	// Return ending outer loop index for a two-body interaction calculation where only the upper half (i >= j) is required
	int twoBodyLoopEnd(int nItems);


	/*
	 * Send/Receive Functions
	 */
	public:
	// Wait for all processes
	bool wait(ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Send single integer value to target rank within the specified communicator 
	bool send(int value, int targetRank, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Receive single integer from source rank within the specified communicator
	bool receive(int& value, int sourceRank, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Send single double value to target rank within the specified communicator
	bool send(double value, int targetRank, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Receive single double value from source rank within the specified communicator
	bool receive(double& value, int sourceRank, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Send single bool value to target rank within the specified communicator
	bool send(bool value, int targetRank, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Receive single bool value from source rank within the specified communicator
	bool receive(bool& value, int sourceRank, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Send integer array data to target rank within the specified communicator
	bool send(int* source, int nData, int targetRank, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Receive integer array data from target rank within the specified communicator
	bool receive(int* source, int nData, int sourceRank, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Send double array data to target rank within the specified communicator
	bool send(double* source, int nData, int targetRank, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Receive double array data from target rank within the specified communicator
	bool receive(double* source, int nData, int sourceRank, ProcessPool::CommunicatorType commType = ProcessPool::Pool);


	/*
	 * Broadcast Functions
	 */
	public:
	// Broadcast CharString
	bool broadcast(CharString& source, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast char data
	bool broadcast(char* source, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast Vec3<int>
	bool broadcast(Vec3<int>& source, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast Vec3<double>
	bool broadcast(Vec3<double>& source, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast single integer
	bool broadcast(int& source, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast integers
	bool broadcast(int* source, int count, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast single long integer
	bool broadcast(long int& source, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast single double
	bool broadcast(double& source, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast double(s)
	bool broadcast(double* source, int count, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast float(s)
	bool broadcast(float* source, int count, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast bool
	bool broadcast(bool& source, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast Array<int>
	bool broadcast(Array<int>& array, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast Array<double>
	bool broadcast(Array<double>& array, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast Array< Vec3<int> >
	bool broadcast(Array< Vec3<int> >& array, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast Array< Vec3<double> >
	bool broadcast(Array< Vec3<double> >& array, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast Array2D<double>
	bool broadcast(Array2D<double>& array, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);


	/*
	 * Special Array Functions
	 */
	public:
	// Reduce (sum) double data to root process
	bool sum(double* source, int count, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Reduce (sum) int data to root process
	bool sum(int* source, int count, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Reduce (sum) double data to all processes
	bool allSum(double* source, int count, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Reduce (sum) int data to all processes
	bool allSum(int* source, int count, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Assemble integer array on target rank within the specified communicator
	bool assemble(int* array, int nData, int* rootDest, int rootMaxData, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Assemble double array on target rank within the specified communicator
	bool assemble(double* array, int nLocalData, double* rootDest, int rootMaxData, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Assemble Array<double> on target rank within the specified communicator
	bool assemble(Array<double>& array, int nData, Array<double>& rootDest, int rootMaxData, int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);


	/*
	 * Decisions
	 */
	public:
	// Broadcast logical decision to proceed to processes (Master only)
	void proceed(int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast logical decision to stop to processes (Master only)
	void stop(int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Receive logical decision from master (Slaves only)
	bool decision(int rootRank = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Test the supplied condition over all processes, returning true only if they all report truth
	bool allTrue(bool isOK, ProcessPool::CommunicatorType commType = ProcessPool::Pool);


	/*
	 * Buffered Random Numbers
	 */
	private:
	// Random number buffer
	double randomBuffer_[RANDBUFFERSIZE];
	// Index of next buffered number
	int randomBufferIndex_;
	// Communicator for random number buffer
	ProcessPool::CommunicatorType randomBufferCommGroup_;

	private:
	// Refill random number buffer
	void refillRandomBuffer();

	public:
	// Initialise random number buffer for processes
	void initialiseRandomBuffer(ProcessPool::CommunicatorType commType);
	// Get next buffered random number (0-1 inclusive)
	double random();
	// Get next buffered random number (-1 to +1 inclusive)
	double randomPlusMinusOne();


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
#define MPIRunMaster(pool,x) (pool.isMaster() ? (x ? (pool.proceed(),true) : (pool.stop(),false)) : pool.decision())
#else
#define MPIRunMaster(pool,x) x
#endif

#endif
