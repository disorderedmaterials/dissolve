/*
	*** Process Pool
	*** src/base/processpool.h
	Copyright T. Youngs 2012-2016

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
#include "base/dnchar.h"
#include "base/timer.h"
#include "templates/vector3.h"
#include "templates/reflist.h"
#include "templates/array.h"
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
	 * Timers
	 */
	private:
	// Communication Timers
	Timer accumTime_, totalTime_;

	public:
	// Reset accumulated Comm time
	void resetAccumulatedTime();
	// Return accumulated time string
	const char* accumulatedTimeString();
	// Return total time string
	const char* totalTimeString();


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
	Dnchar name_;
	// List of world ranks in this pool
	Array<int> worldRanks_;
	// List of process groups within the pool, referencing pool ranks of processes
	List<ProcessGroup> processGroups_;
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

	public:
	// Setup pool with processes specified
	bool setup(const char* name, Array<int> worldRanks);
	// Return name of pool
	const char* name();
	// Return total number of processes in pool
	int nProcesses();
	// Return root (first) world rank of this pool
	int rootWorldRank();
	// Setup strategy for Cells, based on local process pool size
	bool setupCellStrategy(const Vec3<int>& divisions, const Vec3<int>& cellExtents, const List< ListVec3<int> >& neighbours);
	// Return number of process groups
	int nProcessGroups() const;
	// Return nth process group
	ProcessGroup* processGroup(int n);
	// Return number of processes in specified group
	int nProcessesInGroup(int groupId);
	// Return array of pool ranks in the specified group
	int* poolRanksInGroup(int groupId);


	/*
	 * Local Process Limits
	 */
	private:
	// Linear Atom index limits for this process
	int linearFirstAtom_, linearLastAtom_;
	// Linear Grain index limits for this process
	int linearFirstGrain_, linearLastGrain_;
	// Diagonal Atom index limits for this process
	int diagonalFirstAtom_, diagonalLastAtom_;

	public:
	// Setup limits base on total nAtoms and nGrains
	bool calculateLimits(int nAtoms, int nGrains);
	// Return first linear Atom index
	int linearFirstAtom();
	// Return last linear Atom index
	int linearLastAtom();
	// Return first linear Grain index
	int linearFirstGrain();
	// Return last linear Grain index
	int linearLastGrain();
	// Return diagonal first Atom index
	int diagonalFirstAtom();
	// Return diagonal last Atom index
	int diagonalLastAtom();
	// Return starting index for general interleaved loop
	int interleavedLoopStart(ProcessPool::LoopContext loopContext);
	// Return stride for general interleaved loop
	int interleavedLoopStride(ProcessPool::LoopContext loopContext);


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
	// Broadcast Dnchar
	bool broadcast(Dnchar& source, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast char data
	bool broadcast(char* source, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast Vec3<double>
	bool broadcast(Vec3<double>& source, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast integer(s)
	bool broadcast(int* source, int count, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast double(s)
	bool broadcast(double* source, int count, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast float(s)
	bool broadcast(float* source, int count, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast bool
	bool broadcast(bool& source, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast Array<int>
	bool broadcast(Array<int>& array, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast Array<double>
	bool broadcast(Array<double>& array, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);


	/*
	 * Special Array Functions
	 */
	public:
	// Reduce (sum) double data to root process
	bool sum(double* source, int count, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Reduce (sum) int data to root process
	bool sum(int* source, int count, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Reduce (sum) double data to all processes
	bool allSum(double* source, int count, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Reduce (sum) int data to all processes
	bool allSum(int* source, int count, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Assemble integer array on target rank within the specified communicator
	bool assemble(int* array, int nData, int* rootDest, int rootMaxData, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Assemble double array on target rank within the specified communicator
	bool assemble(double* array, int nData, double* rootDest, int rootMaxData, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Assemble Array<double> on target rank within the specified communicator
	bool assemble(Array<double>& array, int nData, Array<double>& rootDest, int rootMaxData, int root = 0, ProcessPool::CommunicatorType commType = ProcessPool::Pool);


	/*
	 * Decisions
	 */
	public:
	// Broadcast logical decision to proceed to processes (Master only)
	void proceed(ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Broadcast logical decision to stop to processes (Master only)
	void stop(ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Receive logical decision from master (Slaves only)
	bool decision(ProcessPool::CommunicatorType commType = ProcessPool::Pool);
	// Return if one or more processes have failed (based on supplied bool)
	bool ok(bool isOK, ProcessPool::CommunicatorType commType = ProcessPool::Pool);


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

// BroadcastList
template <class T> class BroadcastList
{
	/*
	 * Constructor-only template class which iterates over a supplied list, broadcasting the object from master
	 * to slave processes. The List must contain items which subclass MPIListItem, in order to provide the 'broadcast()' virtual.
	 */
	public:
	// Constructor
	BroadcastList(ProcessPool& procPool, List<T>& items, bool& result)
	{
		result = false;
		int count;
		if (procPool.isMaster())
		{
			// Broadcast number of items in list, then list items...
			count = items.nItems();
			if (!procPool.broadcast(&count, 1)) return;
			for (MPIListItem<T>* item = items.first(); item != NULL; item = item->next) if (!item->broadcast(procPool)) return;
		}
		else
		{
			// Get number of list items to expect
			if (!procPool.broadcast(&count, 1)) return;

			// Clear list and reconstruct
			items.clear();
			for (int n=0; n<count; ++n)
			{
				// Slaves must create a suitable structure first, and then join the broadcast
				T* item = items.add();
				if (!item->broadcast(procPool)) return;
			}
		}

		// All OK - success!
		result = true;
	};
};

// BroadcastRefList
template <class T, class D> class BroadcastRefList
{
	/*
	 * Constructor-only template class which iterates over a supplied RefList, recreating the reference list of the master
	 * on the slave processes.
	 */
	public:
	// Constructor
	BroadcastRefList(ProcessPool& procPool, RefList<T,D>& items, const List<T>& itemSource, bool& result)
	{
		result = false;
		int nItems, index;
		T* item;
		D data;
		if (procPool.isMaster())
		{
			// Send number of items in list
			nItems = items.nItems();
			if (!procPool.broadcast(&nItems, 1)) return;
			// Loop over items - send index of referenced item in its original list, and then associated data
			for (RefListItem<T,D>* ri = items.first(); ri != NULL; ri = ri->next)
			{
				index = itemSource.indexOf(ri->item);
				if (!procPool.broadcast(&index, 1))
				{
					Messenger::error("Master failed to broadcast item index in BroadcastRefList.\n");
					return;
				}
				if (!procPool.broadcast(&ri->data, 1))
				{
					Messenger::error("Master failed to broadcast item data in BroadcastRefList.\n");
					return;
				}
			}
		}
		else
		{
			// Receive number of items to create
			if (!procPool.broadcast(&nItems, 1)) return;
			for (int n=0; n<nItems; ++n)
			{
				// Master will send the index of the item in the itemSource list to reference
				if (!procPool.broadcast(&index, 1))
				{
					Messenger::error("Slave %i in pool '%s' failed to broadcast item index in BroadcastRefList.\n", procPool.poolRank(), procPool.name());
					return;
				}
				if (!procPool.broadcast(&data, 1))
				{
					Messenger::error("Slave %i in pool '%s' failed to broadcast item data in BroadcastRefList.\n", procPool.poolRank(), procPool.name());
					return;
				}
				item = itemSource.item(index);
				if (item == NULL)
				{
					Messenger::error("BroadcastRefList couldn't find index %i in source list.\n", index);
					return;
				}
				items.add(item, data);
			}
		}
		// Success!
		result = true;
	};
};

/*
 * Macro Definitions
 */

/*
 * MPIRunMaster(x)
 * Effectively performs the following code:
 * if (ProcessPool::isWorldMaster()) then
 * {
 *	// Test supplied code/condition 'x', and broadcast result to all processes
 *	if (x) then
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
#define MPIRunMaster(x)\
(ProcessPool::isWorldMaster() ?\
   (x ? (MPI_Bcast(&ProcessPool::SUCCEEDED,1,MPI_INTEGER,0,MPI_COMM_WORLD),true)\
      : (MPI_Bcast(&ProcessPool::FAILED,1,MPI_INTEGER,0,MPI_COMM_WORLD),false))\
: (MPI_Bcast(&ProcessPool::RESULT,1,MPI_INTEGER,0,MPI_COMM_WORLD),ProcessPool::RESULT ? true : false))
#else
#define MPIRunMaster(x) x
#endif

#endif
