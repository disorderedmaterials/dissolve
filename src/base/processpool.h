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

#include "base/constants.h"
#include "base/dnchar.h"
#include "base/timer.h"
#include "templates/vector3.h"
#include "templates/reflist.h"
#include "templates/array.h"
// Include <mpi.h> only if we are compiling in parallel
#ifdef PARALLEL
#include <mpi.h>
#endif

/*
 * ProcessPool
 * Basic class to define process-global variables useful to control parallel execution,
 * and provide macros to simplify coding and aid readability.
 */
class ProcessPool
{	
	public:
	// Constructor
	ProcessPool();
	// Copy Constructor
	ProcessPool(const ProcessPool& source);
	// Assignment Operator
	void operator=(const ProcessPool& source);
	// Communications Group (for subroutines)
	enum CommGroup
	{
		Solo,		/* No group (process is flying solo) (communicator == NONE) */
		Group,		/* Process groups (communicator == localCommunicator_) */
		Leaders,	/* A group containing all process group leaders in the local pool (communicator == leaderCommunicator_) */
		Pool		/* A group containing all processes in the local pool (communicator == poolCommunicator_) */
	};
#ifdef PARALLEL
	// Return communicator for group specified
	MPI_Comm communicator(ProcessPool::CommGroup group);
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
	// Initialise parallel communications, setting world ranks / nProcesses
	static bool initialiseMPI(int* argn, char ***argv);
	// End parallel communications
	static bool finalise();
	// Return number of world processes
	static int nWorldProcesses();
	// Return world rank of this process
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
	 * Local Process Pool
	 */
	private:
	// Name of this pool
	Dnchar name_;
	// List of world ranks in this pool
	Array<int> worldRanks_;
	// Local rank of this process in the pool
	int poolRank_;
	// List of process groups
	List< Array<int> > processGroups_;
	// World ranks of process group leaders
	Array<int> groupLeaders_;
	// Index of local group in which this process exists
	int groupIndex_;
	// Rank of this process in its local group
	int groupRank_;
#ifdef PARALLEL
	// Group of all processes in this pool
	MPI_Group poolGroup_;
	// Communicator for all processes in this pool
	MPI_Comm poolCommunicator_;
	// Group group of this process
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
	// Return rank of this process in the pool
	int poolRank();
	// Return whether this process is the master for this pool
	bool isMaster();
	// Return whether this process is a local slave in this pool
	bool isSlave();
	// Return whether this pool involves this process
	bool involvesMe();
	// Setup strategy for Cells, based on local process pool size
	bool setupCellStrategy(const Vec3<int>& divisions, const Vec3<int>& cellExtents, const List< ListVec3<int> >& neighbours);
	// Return number of process groups
	int nProcessGroups() const;
	// Return number of processes in nth group
	int nProcessesInGroup(int groupId);
	// Return process array of nth group
	int* processesInGroup(int groupId);
	// Return group index in which this process exists
	int groupIndex();
	// Return size of local group in which this process exists
	int groupSize();
	// Return rank of this process in its local group
	int groupRank();
	// Return whether this process is a group leader
	bool groupLeader();
#ifdef PARALLEL
	// Local group communicator
	MPI_Comm localCommunicator();
#endif


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
	int interleavedLoopStart(ProcessPool::CommGroup group);
	// Return stride for general interleaved loop
	int interleavedLoopStride(ProcessPool::CommGroup group);


	/*
	 * Send/Receive Functions
	 */
	public:
	// Wait for all processes
	bool wait(ProcessPool::CommGroup group);
	// Send single integer value to target process
	bool send(int value, int targetProcess);
	// Receive single integer from source process
	bool receive(int& value, int sourceProcess);
	// Send single double value to target process
	bool send(double value, int targetProcess);
	// Receive single double value from source process
	bool receive(double& value, int sourceProcess);
	// Send single bool value to target process
	bool send(bool value, int targetProcess);
	// Receive single bool value from source process
	bool receive(bool& value, int sourceProcess);
	// Send integer array data to target process
	bool send(int* source, int nData, int targetProcess);
	// Receive integer array data from target process
	bool receive(int* source, int nData, int sourceProcess);
	// Send double array data to target process
	bool send(double* source, int nData, int targetProcess);
	// Receive double array data from target process
	bool receive(double* source, int nData, int sourceProcess);


	/*
	 * Broadcast Functions
	 */
	public:
	// Broadcast Dnchar
	bool broadcast(Dnchar& source, int rootProcess = 0, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Broadcast char data
	bool broadcast(char* source, int rootProcess = 0, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Broadcast Vec3<double>
	bool broadcast(Vec3<double>& source, int rootProcess = 0, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Broadcast integer(s)
	bool broadcast(int* source, int count, int rootProcess = 0, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Broadcast double(s)
	bool broadcast(double* source, int count, int rootProcess = 0, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Broadcast float(s)
	bool broadcast(float* source, int count, int rootProcess = 0, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Broadcast bool(s)
	bool broadcast(bool* source, int count, int rootProcess = 0, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Broadcast Array<int>
	bool broadcast(Array<int>& array, int rootProcess = 0, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Broadcast Array<double>
	bool broadcast(Array<double>& array, int rootProcess = 0, ProcessPool::CommGroup group = ProcessPool::Pool);


	/*
	 * Special Array Functions
	 */
	public:
	// Reduce (sum) double data to root process
	bool sum(double* source, int count, int rootProcess = 0, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Reduce (sum) int data to root process
	bool sum(int* source, int count, int rootProcess = 0, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Reduce (sum) double data to all processes
	bool allSum(double* source, int count, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Reduce (sum) int data to all processes
	bool allSum(int* source, int count, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Assemble integer array on target process
	bool assemble(int* array, int nData, int* rootDest, int rootMaxData, int rootProcess = 0);
	// Assemble double array on target process
	bool assemble(double* array, int nData, double* rootDest, int rootMaxData, int rootProcess = 0);
	// Assemble double array on target process
	bool assemble(Array<double>& array, int nData, Array<double>& rootDest, int rootMaxData, int rootProcess = 0);


	/*
	 * Decisions
	 */
	public:
	// Broadcast logical decision to proceed to all processes (Master only)
	void proceed();
	// Broadcast logical decision to stop to all processes (Master only)
	void stop();
	// Receive logical decision from master (Slaves only)
	bool decision();
	// Return if one or more processes have failed (based on supplied boo)
	bool ok(bool isOK);


	/*
	 * Buffered Random Numbers
	 */
	private:
	// Random number buffer
	double randomBuffer_[RANDBUFFERSIZE];
	// Index of next buffered number
	int randomBufferIndex_;
	// Communicator for random number buffer
	ProcessPool::CommGroup randomBufferCommGroup_;

	private:
	// Refill random number buffer
	void refillRandomBuffer();

	public:
	// Initialise random number buffer for processes
	void initialiseRandomBuffer(ProcessPool::CommGroup group);
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
 * BroadcastList
 * Constructor-only template class which iterates over a supplied list, broadcasting the object from master
 * to slave processes. The List must contain items which subclass MPIListItem, in order to provide the 'broadcast()' virtual.
 */
template <class T> class BroadcastList
{
	public:
	// Constructor
	BroadcastList(ProcessPool& procPool, List<T>& items, bool& result)
	{
		result = false;
		int count;
		if (procPool.isMaster())
		{
			// Broadcast number of items in list...
			count = items.nItems();
			if (!procPool.broadcast(&count, 1)) return;
			for (MPIListItem<T>* item = items.first(); item != NULL; item = item->next) if (!item->broadcast(procPool)) return;
		}
		else
		{
			// Get number of list items to expect
			if (!procPool.broadcast(&count, 1)) return;
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

/*
 * BroadcastRefList
 * Constructor-only template class which iterates over a supplied RefList, recreating the reference list of the master
 * on the slave processes.
 */
template <class T, class D> class BroadcastRefList
{
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
