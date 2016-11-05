/*
	*** Process Pool
	*** src/base/processpool.cpp
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

#include "base/processpool.h"
#include "base/messenger.h"
#include "base/constants.h"
#include <string.h>

// Static Members
int ProcessPool::nWorldProcesses_ = 1;
int ProcessPool::worldRank_ = 0;
	
// External Declarations
int ProcessPool::FAILED = 0;
int ProcessPool::SUCCEEDED = 1;
int ProcessPool::RESULT;

// Constructor
ProcessPool::ProcessPool()
{
	poolRank_ = -1;
	groupIndex_ = -1;
	groupRank_ = -1;
}

#ifdef PARALLEL
// Return communicator specified
MPI_Comm ProcessPool::communicator(ProcessPool::CommGroup group)
{
	if (group == ProcessPool::Group) return groupCommunicator_;
	else if (group == ProcessPool::Leaders) return leaderCommunicator_;
	else if (group == ProcessPool::Pool) return poolCommunicator_;
	Messenger::print("BAD_USAGE - Communicator %i is not valid in ProcessPool::communicator().\n", group);
	return MPI_COMM_WORLD;
}
#endif

// Copy Constructor
ProcessPool::ProcessPool(const ProcessPool& source)
{
	(*this) = source;
}

// Assignment Operator
void ProcessPool::operator=(const ProcessPool& source)
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
#ifdef PARALLEL
	groupGroup_ = source.groupGroup_;
	groupCommunicator_ = source.groupCommunicator_;
	leaderGroup_ = source.leaderGroup_;
	leaderCommunicator_ = source.leaderCommunicator_;
	poolGroup_ = source.poolGroup_;
	poolCommunicator_ = source.poolCommunicator_;
#endif

	// Local process limits
	linearFirstAtom_ = source.linearFirstAtom_;
	linearLastAtom_ = source.linearLastAtom_;
	linearFirstGrain_ = source.linearFirstGrain_;
	linearLastGrain_ = source.linearLastGrain_;
	diagonalFirstAtom_ = source.diagonalFirstAtom_;
	diagonalLastAtom_ = source.diagonalLastAtom_;

	// Random number buffer
	// ???
}

/*
 * Global Information
 */

// Initialise parallel communications, setting world ranks / nProcesses
bool ProcessPool::initialiseMPI(int* argn, char*** argv)
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
		Messenger::print("Number of processes = %i\n", nWorldProcesses_);

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
bool ProcessPool::finalise()
{
#ifdef PARALLEL
	MPI_Finalize();
#endif
	return true;
}

// Return number of world processes
int ProcessPool::nWorldProcesses()
{
	return nWorldProcesses_;
}

// Return world rank of this process
int ProcessPool::worldRank()
{
	return worldRank_;
}

// Return if this is the world master process
bool ProcessPool::isWorldMaster()
{
	return (worldRank_ == 0);
}

/*
 * Timing
 */

// Reset accumulated Comm time
void ProcessPool::resetAccumulatedTime()
{
	accumTime_.zero();
}

// Return accumulated time string
const char* ProcessPool::accumulatedTimeString()
{
	return accumTime_.timeString();
}

// Return total time string
const char* ProcessPool::totalTimeString()
{
	return totalTime_.timeString();
}

/*
 * Local Process Pool
 */

// Setup pool with world ranks specified
bool ProcessPool::setup(const char* name, Array<int> worldRanks)
{
	name_ = name;

	// Set rank list
	worldRanks_ = worldRanks;

	// See if our rank is in the list
	for (int n=0; n<worldRanks_.nItems(); ++n)
	{
		if (worldRank_ == worldRanks_[n])
		{
			poolRank_ = n;
			Messenger::printVerbose("Process with world rank %i added to pool '%s' with local rank %i.\n", worldRanks_[n], name_.get(), n);
			break;
		}
	}

#ifdef PARALLEL
	// Create pool group and communicator
	MPI_Group origGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &origGroup);
	if (MPI_Group_incl(origGroup, worldRanks_.nItems(), worldRanks_.array(), &poolGroup_) != MPI_SUCCESS) return false;
	if (MPI_Comm_create(MPI_COMM_WORLD, poolGroup_, &poolCommunicator_) != MPI_SUCCESS) return false;
#endif

	return true;
}

// Return name of pool
const char* ProcessPool::name()
{
	return name_.get();
}

// Return total number of processes in pool
int ProcessPool::nProcesses()
{
	return worldRanks_.nItems();
}

// Return local rank of this process in the pool
int ProcessPool::poolRank()
{
	return poolRank_;
}

// Return root (first) world rank of this pool
int ProcessPool::rootWorldRank()
{
	return worldRanks_.first();
}

// Return whether this process is the master for this pool
bool ProcessPool::isMaster()
{
	return (poolRank_ == 0);
}

// Return whether this process is a local slave in this pool
bool ProcessPool::isSlave()
{
	return (poolRank_ != 0);
}

// Return whether this pool involves this process
bool ProcessPool::involvesMe()
{
	for (int n=0; n<worldRanks_.nItems(); ++n) if (worldRanks_.value(n) == worldRank_) return true;
	return false;
}

// Setup strategy for Cells, based on local process pool size
bool ProcessPool::setupCellStrategy(const Vec3<int>& divisions, const Vec3<int>& cellExtents, const List< ListVec3<int> >& neighbours)
{
#ifdef PARALLEL
	// Construct a temporary array of 'Cells'
	int*** cells = new int**[divisions.x];
	for (int x = 0; x<divisions.x; ++x)
	{
		cells[x] = new int*[divisions.y];
		for (int y = 0; y<divisions.y; ++y)
		{
			cells[x][y] = new int[divisions.z];
			for (int z = 0; z < divisions.z; ++z) cells[x][y][z] = 0;
		}
	}

	// Test assignment - how many processes can we get onto one XY slice, and how many in total at once?
	int nGroups = 0;
	int xx, yy, zz;
	for (int x = 0; x<divisions.x; ++x)
	{
		for (int y = 0; y<divisions.y; ++y)
		{
			for (int z = 0; z < divisions.z; ++z)
			{
				if (cells[x][y][z] != 0) continue;

				// Cell not yet assigned, so set to -1 for now and check we can mark off neighbours
				cells[x][y][z] = -1;
				for (int pass = 0; pass < 2; ++pass)
				{
					if (cells[x][y][z] != -1) break;
					// Check availability on first pass, set flags on second
					for (ListVec3<int>* item = neighbours.first(); item != NULL; item = item->next)
					{
						// Get wrapped Cell grid reference
						xx = (x+item->x)%divisions.x;
						yy = (y+item->y)%divisions.y;
						zz = (z+item->z)%divisions.z;
						if (xx < 0) xx += divisions.x;
						if (yy < 0) yy += divisions.y;
						if (zz < 0) zz += divisions.z;
						// Check for availability / Lock Cell
						if (pass == 0)
						{
							if (cells[xx][yy][zz] == -1)
							{
								cells[x][y][z] = 0;
								break;
							}
						}
						else ++cells[xx][yy][zz];
					}
				}
				// If the central cell has been unset, we couldn't lock it so continue the search
				if (cells[x][y][z] != -1) continue;

				// Otherwise, we found a Cell and locked it, so increase group count
				++nGroups;
			}
		}
	}
	Messenger::print("For the current Cell division and extent values, %i simultaneous Cell-modifying processes are possible.\n", nGroups);
	// Maximum number of groups possible is the number of processes available, but we want all groups to contain the same number
	// of processes...
	while (worldRanks_.nItems()%nGroups != 0) --nGroups;
	
	Messenger::print("Processes will be divided into %i groups.\n", nGroups);

	// Create process groups
	int baseAlloc = worldRanks_.nItems() / nGroups;
	int remainder = worldRanks_.nItems() % nGroups;
	int rank, firstRank, lastRank;
	Array<int>* group;
	Dnchar s;
	for (int n=0; n<nGroups; ++n)
	{
		// Create process group and add a (currently null) entry to the groupLeaders_ array
		group = processGroups_.add();
		groupLeaders_.add(-1);
		Messenger::print("Created process group %i\n", n);

		// Create array of ranks to put in new group
		firstRank = baseAlloc * n + (n < remainder ? n : remainder);
		lastRank = firstRank + baseAlloc - (n >= remainder ? 1 : 0);
		s.clear();
		for (rank=firstRank; rank<=lastRank; ++rank)
		{
			group->add(rank);
			if (poolRank_ == rank) groupIndex_ = n;
			s.strcatf(" %i", rank);
		}
		Messenger::print("--> Group will contain %i processes :%s\n", group->nItems(), s.get());
	}

	// Create local groups and communicators
	MPI_Group origGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &origGroup);
	if (MPI_Group_incl(origGroup, groupSize(), processesInGroup(groupIndex_), &groupGroup_) != MPI_SUCCESS) return false;
	if (MPI_Comm_create(MPI_COMM_WORLD, groupGroup_, &groupCommunicator_) != MPI_SUCCESS) return false;
	MPI_Group_rank(groupGroup_, &groupRank_);
	Messenger::printVerbose("Process with pool rank %i (world rank %i) has local group %i, group rank %i, and is a process group %s %i\n", poolRank_, worldRank_, groupIndex_, groupRank_, groupLeader() ? "leader" : "slave");

	// Master now assembles list of group leaders
	bool leader;
	if (isMaster())
	{
		// Loop over process groups
		for (int group=0; group<processGroups_.nItems(); ++group)
		{
			// Query each process in the group to see if it is the leader...
			for (int n=0; n<nProcessesInGroup(group); ++n)
			{
				// Is this us?
				if (processesInGroup(group)[n] == poolRank_) leader = groupLeader();
				else if (!receive(leader, processesInGroup(group)[n])) return false;
				
// 				printf("Group %i, process el %i : leader = %i\n", group, n, leader);
				
				// Is this process the leader?
				if (leader)
				{
					// Check for existing value
					if (groupLeaders_[group] == -1) groupLeaders_[group] = processesInGroup(group)[n];
					else
					{
						Messenger::print("MPI Error: More than one process group leader for group %i.\n", group);
						return false;
					}
				}
			}
		}
	}
	else if (!send(groupLeader(), 0)) return false;

	// Broadcast group leader list
	if (!broadcast(groupLeaders_.array(), processGroups_.nItems())) return false;
	Messenger::print("Group leader processes are :\n");
	for (int group=0; group<processGroups_.nItems(); ++group) Messenger::print("--> Group %3i : process rank %i\n", group, groupLeaders_[group]);

	// Create group leader communicator
	MPI_Comm_group(MPI_COMM_WORLD, &origGroup);
	if (MPI_Group_incl(origGroup, processGroups_.nItems(), groupLeaders_.array(), &leaderGroup_) != MPI_SUCCESS) return false;
	if (MPI_Comm_create(MPI_COMM_WORLD, leaderGroup_, &leaderCommunicator_) != MPI_SUCCESS) return false;
#else
	// No MPI, but must still setup a dummy process group
	Array<int>* procList = processGroups_.add();
	procList->add(0);
	groupIndex_ = 0;
	groupRank_ = 0;
#endif
	return true;
}

// Return number of process groups
int ProcessPool::nProcessGroups() const
{
	return processGroups_.nItems();
}

// Return number of processes in nth group
int ProcessPool::nProcessesInGroup(int groupId)
{
#ifdef CHECKS
	if ((groupId < 0) || (groupId >= processGroups_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Specified groupId (%i) is out of range in ProcessPool::nProcesses() (nProcessGroups = %i).\n", groupId, processGroups_.nItems());
		return 0;
	}
#endif
	return processGroups_[groupId]->nItems();
}

// Return process array of nth group
int* ProcessPool::processesInGroup(int groupId)
{
#ifdef CHECKS
	if ((groupId < 0) || (groupId >= processGroups_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Specified groupId (%i) is out of range in ProcessPool::processes() (nProcessGroups = %i).\n", groupId, processGroups_.nItems());
		return 0;
	}
#endif
	return processGroups_[groupId]->array();
}

// Return group index in which this process exists
int ProcessPool::groupIndex()
{
	return groupIndex_;
}
// Return size of local group in which this process exists
int ProcessPool::groupSize()
{
#ifdef CHECKS
	if ((groupIndex_ < 0) || (groupIndex_ >= processGroups_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Local group index for this process (%i) is out of range in ProcessPool::localGroupSize() (nProcessGroups = %i).\n", groupIndex_, processGroups_.nItems());
		return 0;
	}
#endif
	return processGroups_[groupIndex_]->nItems();
}
// Return rank of this process in its local group
int ProcessPool::groupRank()
{
	return groupRank_;
}

// Return whether this process is a group leader
bool ProcessPool::groupLeader()
{
	return (groupRank_ == 0);
}

// Return process info string
const char* ProcessPool::processInfo()
{
	static Dnchar info;
	// World Rank / Pool Rank, Group Rank @ Group Index
	info.sprintf("%iw/%ip,%ig@%ii", worldRank_, poolRank_, groupRank_, groupIndex_);
	return info.get();
}

/*
 * Process Limits
 */

// Setup limits based on total nAtoms and nGrains
bool ProcessPool::calculateLimits(int nAtoms, int nGrains)
{
	long int baseAlloc, remainder;
	// Linear Atoms - Do straight division by number of processes to get basic allocation
	baseAlloc = nAtoms / worldRanks_.nItems();
	remainder = nAtoms % worldRanks_.nItems();
	linearFirstAtom_ = baseAlloc * poolRank_ + (poolRank_ < remainder ?  poolRank_: remainder);
	linearLastAtom_ = linearFirstAtom_ + baseAlloc - (poolRank_ >= remainder ? 1 : 0);
	Messenger::print("--> Nominally assigned Atoms %i to %i (%i total) to process with rank %i.\n", linearFirstAtom_, linearLastAtom_, 1+linearLastAtom_-linearFirstAtom_, poolRank_);

	// Linear Grains - Do straight division by number of processes to get basic allocation
	baseAlloc = nGrains / worldRanks_.nItems();
	remainder = nGrains % worldRanks_.nItems();
	linearFirstGrain_ = baseAlloc * poolRank_ + (poolRank_ < remainder ?  poolRank_: remainder);
	linearLastGrain_ = linearFirstGrain_ + baseAlloc - (poolRank_ >= remainder ? 1 : 0);
	Messenger::print("--> Nominally assigned Grains %i to %i (%i total) to process with rank %i.\n", linearFirstGrain_, linearLastGrain_, 1+linearLastGrain_-linearFirstGrain_, poolRank_);
	
	// Diagonal Atoms - For calculation of upper-diagonal half of any two-body interaction matrix
	double rnproc = 1.0 / worldRanks_.nItems(), area = 1.0;
	int startAtom = 0, finishAtom;

	// Loop over processes
	for (int process = 0; process<worldRanks_.nItems(); ++process)
	{
		// If this is the last process, make sure we avoid doing sqrt of zero or delta-neg value
		if (process == (worldRanks_.nItems()-1)) finishAtom = nAtoms - 1;
		else finishAtom = (1.0 - sqrt(area - rnproc)) * nAtoms - 1;
		area -= rnproc;

		// Store limits for this process (if it is us)
		if (process == poolRank_)
		{
			diagonalFirstAtom_ = startAtom;
			diagonalLastAtom_ = finishAtom;
		}

		Messenger::print("--> Assigned diagonal Atom calculation limits of %i -> %i for process with rank %i.\n", startAtom, finishAtom, process);

		// Update startAtom
		startAtom = finishAtom+1;
	}
	
	return true;
}

// Return linear first Atom index
int ProcessPool::linearFirstAtom()
{
	return linearFirstAtom_;
}

// Return linear last Atom index
int ProcessPool::linearLastAtom()
{
	return linearLastAtom_;
}

// Return linear first Grain index
int ProcessPool::linearFirstGrain()
{
	return linearFirstGrain_;
}

// Return linear last Grain index
int ProcessPool::linearLastGrain()
{
	return linearLastGrain_;
}

// Return diagonal first Atom index
int ProcessPool::diagonalFirstAtom()
{
	return diagonalFirstAtom_;
}

// Return diagonal last Atom index
int ProcessPool::diagonalLastAtom()
{
	return diagonalLastAtom_;
}

// Return starting index for general loop
int ProcessPool::interleavedLoopStart(ProcessPool::LoopContext loopContext)
{
	if (loopContext == ProcessPool::OverPoolProcesses) return poolRank_;
	else if (loopContext == ProcessPool::OverGroupProcesses) return groupRank_;
	else if (loopContext == ProcessPool::OverGroups) return groupIndex_;
	else return 0;
}

// Return stride for general loop
int ProcessPool::interleavedLoopStride(ProcessPool::LoopContext loopContext)
{
	if (loopContext == ProcessPool::OverPoolProcesses) return worldRanks_.nItems();
	else if (loopContext == ProcessPool::OverGroupProcesses) return processGroups_.nItems();
	else if (loopContext == ProcessPool::OverGroups) return nProcessesInGroup(groupIndex_);
	else return 1;
}

/*
 * Send/Receive Functions
 */

// Wait for all processes
bool ProcessPool::wait(ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	if (MPI_Barrier(communicator(group)) != MPI_SUCCESS) return false;
#endif
	return true;
}

// Send single integer value to target process
bool ProcessPool::send(int value, int targetProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Send(&value, 1, MPI_INTEGER, targetProcess, 0, MPI_COMM_WORLD) != MPI_SUCCESS) return false;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Receive single integer value from source process
bool ProcessPool::receive(int& value, int sourceProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	MPI_Status status;
	if (MPI_Recv(&value, 1, MPI_INTEGER, sourceProcess, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS) return false;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Send single double value to target process
bool ProcessPool::send(double value, int targetProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Send(&value, 1, MPI_DOUBLE, targetProcess, 0, MPI_COMM_WORLD) != MPI_SUCCESS) return false;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Receive single double value from source process
bool ProcessPool::receive(double& value, int sourceProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	MPI_Status status;
	if (MPI_Recv(&value, 1, MPI_DOUBLE, sourceProcess, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS) return false;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Send single bool value to target process
bool ProcessPool::send(bool value, int targetProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Send(&value, 1, MPI_INTEGER, targetProcess, 0, MPI_COMM_WORLD) != MPI_SUCCESS) return false;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Receive single bool value from source process
bool ProcessPool::receive(bool& value, int sourceProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	MPI_Status status;
	if (MPI_Recv(&value, 1, MPI_INTEGER, sourceProcess, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS) return false;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Send integer array data to target process
bool ProcessPool::send(int* source, int nData, int targetProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Send(source, nData, MPI_INTEGER, targetProcess, 0, MPI_COMM_WORLD) != MPI_SUCCESS) return false;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Receive integer array data from target process
bool ProcessPool::receive(int* source, int nData, int sourceProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	MPI_Status status;
	if (MPI_Recv(source, nData, MPI_INTEGER, sourceProcess, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS) return false;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Send double array data to target process
bool ProcessPool::send(double* source, int nData, int targetProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Send(source, nData, MPI_DOUBLE, targetProcess, 0, MPI_COMM_WORLD) != MPI_SUCCESS) return false;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Receive double array data from target process
bool ProcessPool::receive(double* source, int nData, int sourceProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	MPI_Status status;
	if (MPI_Recv(source, nData, MPI_DOUBLE, sourceProcess, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS) return false;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

/*
 * Broadcast Functions
 */

// Broadcast Dnchar to all Processes
bool ProcessPool::broadcast(Dnchar& source, int rootProcess, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	static char buffer[4096];
	
	// Get length of string, and make a local copy to avoid the const-ness of Dnchar.get().
	if (poolRank_ == rootProcess) strcpy(buffer, source.get());

	// Broadcast data
	if (!broadcast(buffer, worldRanks_[rootProcess], group)) return false;
	
	if (isSlave()) source = buffer;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Broadcast char data to all Processes
bool ProcessPool::broadcast(char* source, int rootProcess, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	int length;
	if (poolRank_ == rootProcess)
	{
		// Broadcast string length first...
		length = strlen(source) + 1;
		if (MPI_Bcast(&length, 1, MPI_INTEGER, worldRanks_[rootProcess], communicator(group)) != MPI_SUCCESS)
		{
			Messenger::print("Failed to broadcast char length data from root process %i (world rank %i).\n", rootProcess, worldRanks_[rootProcess]);
			return false;
		}
		
		// Now broadcast character data. We need to make a local copy of the string, to avoid the const-ness of Dnchar.get().
		if (MPI_Bcast(source, length, MPI_CHARACTER, worldRanks_[rootProcess], communicator(group)) != MPI_SUCCESS)
		{
			Messenger::print("Failed to broadcast char data from root process %i (world rank %i) (world rank %i).\n", rootProcess, worldRanks_[rootProcess]);
			return false;
		}
	}
	else
	{
		// Slaves receive the data into the buffer, and then set the source string.
		// Length first...
		if (MPI_Bcast(&length, 1, MPI_INTEGER, worldRanks_[rootProcess], communicator(group)) != MPI_SUCCESS)
		{
			Messenger::print("Slave %i (world rank %i) failed to receive char length data from root process %i (world rank %i) (world rank %i).\n", poolRank_, worldRanks_[poolRank_], rootProcess, worldRanks_[rootProcess]);
			return false;
		}
		
		if (MPI_Bcast(source, length, MPI_CHARACTER, worldRanks_[rootProcess], communicator(group)) != MPI_SUCCESS)
		{
			Messenger::print("Slave %i (world rank %i) failed to receive char data from root process %i (world rank %i) (world rank %i).\n", poolRank_, worldRanks_[poolRank_], rootProcess, worldRanks_[rootProcess]);
			return false;
		}
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Broadcast Vec3<double> to all Processes
bool ProcessPool::broadcast(Vec3<double>& source, int rootProcess, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	double buffer[3];
	if (poolRank_ == rootProcess)
	{
		// Construct an array from the data...
		buffer[0] = source.x;
		buffer[1] = source.y;
		buffer[2] = source.z;
		if (MPI_Bcast(buffer, 3, MPI_DOUBLE, worldRanks_[rootProcess], communicator(group)) != MPI_SUCCESS)
		{
			Messenger::print("Failed to broadcast Vec3<double> data from root process %i (world rank %i).\n", rootProcess, worldRanks_[rootProcess]);
			return false;
		}
	}
	else
	{
		// Slaves receive the data into the buffer, and then set the source variable.
		if (MPI_Bcast(buffer, 3, MPI_DOUBLE, worldRanks_[rootProcess], communicator(group)) != MPI_SUCCESS)
		{
			Messenger::print("Slave %i (world rank %i) failed to receive Vec3<double> data from root process %i (world rank %i).\n", poolRank_, worldRanks_[poolRank_], rootProcess, worldRanks_[rootProcess]);
			return false;
		}
		source.x = buffer[0];
		source.y = buffer[1];
		source.z = buffer[2];
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Broadcast integer(s) to all Processes
bool ProcessPool::broadcast(int* source, int count, int rootProcess, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Bcast(source, count, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
	{
		Messenger::print("Failed to broadcast int data from root process %i (world rank %i).\n", rootProcess, worldRanks_[rootProcess]);
		return false;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Broadcast double(s) to all Processes
bool ProcessPool::broadcast(double* source, int count, int rootProcess, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Bcast(source, count, MPI_DOUBLE, rootProcess, communicator(group)) != MPI_SUCCESS)
	{
		Messenger::print("Failed to broadcast int data from root process %i (world rank %i).\n", rootProcess, worldRanks_[rootProcess]);
		return false;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Broadcast float(s) to all Processes
bool ProcessPool::broadcast(float* source, int count, int rootProcess, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Bcast(source, count, MPI_FLOAT, rootProcess, communicator(group)) != MPI_SUCCESS)
	{
		Messenger::print("Failed to broadcast int data from root process %i (world rank %i).\n", rootProcess, worldRanks_[rootProcess]);
		return false;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Broadcast bool(s) to all Processes
bool ProcessPool::broadcast(bool* source, int count, int rootProcess, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Bcast(source, count, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
	{
		Messenger::print("Failed to broadcast int data from root process %i (world rank %i).\n", rootProcess, worldRanks_[rootProcess]);
		return false;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Broadcast Array<int>
bool ProcessPool::broadcast(Array<int>& array, int rootProcess, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();

	int length;
	if (poolRank_ == rootProcess)
	{
		// Broadcast string length first...
		length = array.nItems();
		if (MPI_Bcast(&length, 1, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
		{
			Messenger::print("Failed to broadcast Array<int> size from root process %i (world rank %i).\n", rootProcess, worldRanks_[rootProcess]);
			return false;
		}

		// Now broadcast Array data
		if (length > 0)
		{
			if (MPI_Bcast(array.array(), length, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
			{
				Messenger::print("Failed to broadcast Array<int> data from root process %i (world rank %i).\n", rootProcess, worldRanks_[rootProcess]);
				return false;
			}
		}
	}
	else
	{
		// Slaves receive the data into the buffer, and then set the source string.
		// Length first...
		if (MPI_Bcast(&length, 1, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
		{
			Messenger::print("Slave %i (world rank %i) failed to receive Array<int> size from rootProcess %i.\n", poolRank_, worldRanks_[poolRank_], rootProcess);
			return false;
		}

		if (length > 0)
		{
			// Create array of specified size
			array.initialise(length);

			if (MPI_Bcast(array.array(), length, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array<int> data from root process %i (world rank %i).\n", poolRank_, worldRanks_[poolRank_], rootProcess, worldRanks_[rootProcess]);
				return false;
			}
		}
		else array.clear();
	}

	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Broadcast Array<double>
bool ProcessPool::broadcast(Array<double>& array, int rootProcess, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();

	int length;
	if (poolRank_ == rootProcess)
	{
		// Broadcast string length first...
		length = array.nItems();
		if (MPI_Bcast(&length, 1, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
		{
			Messenger::print("Failed to broadcast Array<double> size from root process %i (world rank %i).\n", rootProcess, worldRanks_[rootProcess]);
			return false;
		}

		// Now broadcast Arrah data
		if (length > 0)
		{
			if (MPI_Bcast(array.array(), length, MPI_DOUBLE, rootProcess, communicator(group)) != MPI_SUCCESS)
			{
				Messenger::print("Failed to broadcast Array<double> data from root process %i (world rank %i).\n", rootProcess, worldRanks_[rootProcess]);
				return false;
			}
		}
	}
	else
	{
		// Slaves receive the data into the buffer, and then set the source string.
		// Length first...
		if (MPI_Bcast(&length, 1, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
		{
			Messenger::print("Slave %i (world rank %i) failed to receive Array<double> size from rootProcess %i.\n", poolRank_, worldRanks_[poolRank_], rootProcess);
			return false;
		}

		if (length > 0)
		{
			// Create array of specified size
			array.initialise(length);

			if (MPI_Bcast(array.array(), length, MPI_DOUBLE, rootProcess, communicator(group)) != MPI_SUCCESS)
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array<double> data from root process %i (world rank %i).\n", poolRank_, worldRanks_[poolRank_], rootProcess, worldRanks_[rootProcess]);
				return false;
			}
		}
		else array.clear();
	}

	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

/*
 * Special Array Functions
 */

// Reduce (sum) double data to root process
bool ProcessPool::sum(double* source, int count, int rootProcess, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	// If we are the target process then we need to construct a temporary buffer to store the received data in.
	if ((group == ProcessPool::Leaders) && (!groupLeader())) return true;
	if (poolRank_ == rootProcess)
	{
		double buffer[count];
		if (MPI_Reduce(source, buffer, count, MPI_DOUBLE, MPI_SUM, rootProcess, communicator(group)) != MPI_SUCCESS) return false;
		// Put reduced data back into original buffer
		for (int n=0; n<count; ++n) source[n] = buffer[n];
	}
	else
	{
		// Not the target process, so just send data
		if (MPI_Reduce(source, NULL, count, MPI_DOUBLE, MPI_SUM, rootProcess, MPI_COMM_WORLD) != MPI_SUCCESS) return false;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Reduce (sum) int data to root process
bool ProcessPool::sum(int* source, int count, int rootProcess, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	// If we are the target process then we need to construct a temporary buffer to store the received data in.
	if ((group == ProcessPool::Leaders) && (!groupLeader())) return true;
	if (poolRank_ == rootProcess)
	{
		int buffer[count];
		if (MPI_Reduce(source, buffer, count, MPI_INTEGER, MPI_SUM, rootProcess, communicator(group)) != MPI_SUCCESS) return false;
		// Put reduced data back into original buffer
		for (int n=0; n<count; ++n) source[n] = buffer[n];
	}
	else
	{
		// Not the target process, so just send data
		if (MPI_Reduce(source, NULL, count, MPI_INTEGER, MPI_SUM, rootProcess, MPI_COMM_WORLD) != MPI_SUCCESS) return false;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Reduce (sum) double data to all processes
bool ProcessPool::allSum(double* source, int count, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	double buffer[count];
	if ((group == ProcessPool::Leaders) && (!groupLeader())) return true;
	if (MPI_Allreduce(source, &buffer, count, MPI_DOUBLE, MPI_SUM, communicator(group)) != MPI_SUCCESS) return false;
	// Put reduced data back into original buffer
	for (int n=0; n<count; ++n) source[n] = buffer[n];
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Reduce (sum) int data to all processes
bool ProcessPool::allSum(int* source, int count, ProcessPool::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	int buffer[count];
	if ((group == ProcessPool::Leaders) && (!groupLeader())) return true;
	if (MPI_Allreduce(source, &buffer, count, MPI_INTEGER, MPI_SUM, communicator(group)) != MPI_SUCCESS) return false;
	// Put reduced data back into original buffer
	for (int n=0; n<count; ++n) source[n] = buffer[n];
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Assemble integer array on target process
bool ProcessPool::assemble(int* array, int nData, int* rootDest, int rootMaxData, int rootProcess)
{
	/*
	 * Given that the integer 'array' exists on all processes, and each process has stored nData at the
	 * beginning of this array, this function will assemble the data into a complete, continuous array (up to
	 * maxData elements in the new array rootDest) on the rootProcess.
	 */
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (poolRank_ == rootProcess)
	{
		int n;
		// The rootProcess' data must be copied into the local array
		for (n=0; n<nData; ++n) rootDest[n] = array[n];

		// Now get data from other processes, appending each chunk to the rootDest array
		int slaveNData;
		for (int n=0; n<worldRanks_.nItems(); ++n)
		{
			if (poolRank_ == n) continue;

			// Get length of data to receive
			if (!receive(slaveNData, n)) return false;

			// Check buffer length
			if ((nData + slaveNData) > rootMaxData)
			{
				printf("MPI ERROR: Tried to assemble more data on process '%i' than the array allows for (maxData = %i).\n", rootProcess, rootMaxData);
				return false;
			}
			// Receive data
			if (!receive(&rootDest[nData], slaveNData, n)) return false;
			nData += slaveNData;
		}
	}
	else
	{
		// Slaves send array size and then the data to the master...
		if (!send(nData, rootProcess)) return false;
		if (!send(array, nData, rootProcess)) return false;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Assemble double array on target process
bool ProcessPool::assemble(double* array, int nLocalData, double* rootDest, int rootMaxData, int rootProcess)
{
	/*
	 * Given that the double 'array' exists on all processes, and each process has stored nData at the
	 * beginning of this array, this function will assemble the data into a complete, continuous array (up to
	 * maxData elements in the new array rootDest) on the rootProcess.
	 */
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (poolRank_ == rootProcess)
	{
		// The rootProcess' data must be copied into the local array
		for (int n=0; n<nLocalData; ++n) rootDest[n] = array[n];

		// Now get data from other processes, appending each chunk to the rootDest array
		int slaveNData;
		for (int n=0; n<worldRanks_.nItems(); ++n)
		{
			if (poolRank_ == n) continue;
			
			// Get length of data to receive
			if (!receive(slaveNData, n)) return false;
			
			// Check buffer length
			if ((nLocalData + slaveNData) > rootMaxData)
			{
				printf("MPI ERROR: Tried to assemble more data on process '%i' than the array allows for (maxData = %i).\n", rootProcess, rootMaxData);
				return false;
			}
			// Receive data
			if (!receive(&rootDest[nLocalData], slaveNData, n)) return false;
			nLocalData += slaveNData;
		}
	}
	else
	{
		// Slaves send array size and then the data to the master...
		if (!send(nLocalData, rootProcess)) return false;
		if (!send(array, nLocalData, rootProcess)) return false;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return true;
}

// Assemble double array on target process
bool ProcessPool::assemble(Array<double>& array, int nData, Array<double>& rootDest, int rootMaxData, int rootProcess)
{
	if (poolRank_ == rootProcess)
	{
		if (rootDest.size() < rootMaxData)
		{
			Messenger::error("Destination Array<double> in ProcessPool::assemble() is not large enough.");
			stop();
			return false;
		}
		else proceed();
	}
	else if (!decision()) return false;

	// Call double* version of routine...
	return assemble(array.array(), nData, rootDest.array(), rootMaxData, rootProcess);
}

/*
 * Decisions
 */

// Broadcast logical decision to proceed to all processes (Master only)
void ProcessPool::proceed()
{
	bool decision = true;
#ifdef PARALLEL
	if (!isMaster())
	{
		Messenger::print("BAD_USAGE - Slave tried to make a decision.\n");
		return;
	}
	if (!broadcast(&decision, 1)) Messenger::print("Error telling slaves to proceed.\n");
#endif
}

// Broadcast logical decision to stop to all processes (Master only)
void ProcessPool::stop()
{
	bool decision = false;
#ifdef PARALLEL
	if (!isMaster())
	{
		Messenger::print("BAD_USAGE - Slave tried to make a decision.\n");
		return;
	}
	if (!broadcast(&decision, 1)) Messenger::print("Error telling slaves to stop.\n");
#endif
}

// Receive logical decision from master (Slaves only)
bool ProcessPool::decision()
{
#ifdef PARALLEL
	if (!isSlave())
	{
		Messenger::print("BAD_USAGE - Master tried to receive a decision.\n");
		return false;
	}
	bool data;
	if (!broadcast(&data, 1)) Messenger::print("Error receiving decision.\n");
	return data;
#endif
	return true;
}

// Return if one or more processes have failed (based on supplied bool)
bool ProcessPool::ok(bool isOK)
{
#ifdef PARALLEL
	// First, sum all bool values of the processes in the pool
	int summedResult = (isOK ? 1 : 0);
	if (!allSum(&summedResult, 1)) return false;
	return (summedResult == nProcesses());
#endif
	return isOK;
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
	if (randomBufferCommGroup_ == ProcessPool::Pool)
	{
		// Master creates buffer and sends to slaves
		Messenger::printVerbose("Random Buffer - Pool parallel, so master (%s) will create and send array.\n", isMaster() ? "me" : "not me");
		if (isMaster())
		{
			for (int n=0; n<RANDBUFFERSIZE; ++n) randomBuffer_[n] = DUQMath::random();
			broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
		}
		else broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
	}
	else if (randomBufferCommGroup_ == ProcessPool::Group)
	{
		// Group leader creates buffer and sends to slaves
		Messenger::printVerbose("Random Buffer - Group parallel, so process leader (%s) will create and send array.\n", groupLeader() ? "me" : "not me");
		if (groupLeader())
		{
			for (int n=0; n<RANDBUFFERSIZE; ++n) randomBuffer_[n] = DUQMath::random();
			broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
		}
		else broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
	}
	else
	{
		// Create own random buffer
		Messenger::printVerbose("Random Buffer - Solo parallel, so will create own array.\n");
		for (int n=0; n<RANDBUFFERSIZE; ++n) randomBuffer_[n] = DUQMath::random();
	}
#endif
}

// Initialise random number buffer
void ProcessPool::initialiseRandomBuffer(ProcessPool::CommGroup group)
{
	randomBufferCommGroup_ = group;
	refillRandomBuffer();
}

// Get next buffered random number
double ProcessPool::random()
{
#ifdef PARALLEL
	// Have we exhausted the buffer?
	if (randomBufferIndex_ == RANDBUFFERSIZE) refillRandomBuffer();
	return randomBuffer_[randomBufferIndex_++];
#else
	return DUQMath::random();
#endif
}

// Get next buffered random number (-1 to +1 inclusive)
double ProcessPool::randomPlusMinusOne()
{
#ifdef PARALLEL
	// Have we exhausted the buffer?
	if (randomBufferIndex_ == RANDBUFFERSIZE) refillRandomBuffer();
	return (randomBuffer_[randomBufferIndex_++]-0.5)*2.0;
#else
	return DUQMath::randomPlusMinusOne();
#endif
}
