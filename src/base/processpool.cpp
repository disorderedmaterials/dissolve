/*
	*** Process Pool
	*** src/base/processpool.cpp
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
	maxProcessGroups_ = 0;
	groupsModifiable_ = true;
}

// Copy Constructor
ProcessPool::ProcessPool(const ProcessPool& source)
{
	(*this) = source;
	poolRank_ = -99;
	groupIndex_ = -99;
	groupRank_ = -99;
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


#ifdef PARALLEL
// Return communicator specified
MPI_Comm ProcessPool::communicator(ProcessPool::CommunicatorType commType)
{
	if (commType == ProcessPool::Group) return groupCommunicator_;
	else if (commType == ProcessPool::Leaders) return leaderCommunicator_;
	else if (commType == ProcessPool::Pool) return poolCommunicator_;
	Messenger::print("BAD_USAGE - CommunicatorType %i is not valid in ProcessPool::communicator().\n", commType);

	return MPI_COMM_WORLD;
}
#endif

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
	timer_.zero();
}

// Return accumulated time string
const char* ProcessPool::accumulatedTimeString()
{
	return timer_.totalTimeString();
}

/*
 * Process Identification
 */

// Return local rank of this process in the pool
int ProcessPool::poolRank()
{
	return poolRank_;
}

// Return whether this process is the master for the specified communicator
bool ProcessPool::isMaster(ProcessPool::CommunicatorType commType)
{
	if (commType == ProcessPool::Pool) return (poolRank_ == 0);
	else if (commType == ProcessPool::Group) return (groupLeaders_[groupIndex_] == poolRank_);
	else if (commType == ProcessPool::Leaders) return (groupLeaders_[0] == poolRank_);

	return false;
}

// Return whether this process is a local slave for the specified communicatyr
bool ProcessPool::isSlave(ProcessPool::CommunicatorType commType)
{
	if (commType == ProcessPool::Pool) return (poolRank_ != 0);
	else if (commType == ProcessPool::Group) return (groupLeaders_[groupIndex_] != poolRank_);
	else if (commType == ProcessPool::Leaders) return (groupLeaders_[0] != poolRank_);

	return true;
}

// Return whether this process is the pool index specified
bool ProcessPool::isMe(int poolIndex)
{
	return (poolRank_ == poolIndex);
}

// Return whether this pool involves this process
bool ProcessPool::involvesMe()
{
	for (int n=0; n<worldRanks_.nItems(); ++n) if (worldRanks_.value(n) == worldRank_) return true;
	return false;
}

// Return group index in which this process exists
int ProcessPool::groupIndex()
{
	return groupIndex_;
}

// Return local group in which this process exists
ProcessGroup* ProcessPool::myGroup()
{
#ifdef CHECKS
	if ((groupIndex_ < 0) || (groupIndex_ >= processGroups_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Local group index for this process (%i) is out of range in ProcessPool::localGroupSize() (nProcessGroups = %i).\n", groupIndex_, processGroups_.nItems());
		return NULL;
	}
#endif
	return processGroups_[groupIndex_];
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
	static CharString info;
	// World Rank / Pool Rank, Group Rank @ Group Index
	info.sprintf("W%i/P%i,G%i@GI%i", worldRank_, poolRank_, groupRank_, groupIndex_);
	return info.get();
}

/*
 * Pool Contents
 */

// Setup pool with world ranks specified
bool ProcessPool::setup(const char* name, Array<int> worldRanks)
{
	name_ = name;

	Messenger::print("--> Setting up process pool '%s'...\n", name_.get());

	// Set rank list
	worldRanks_ = worldRanks;

	// See if our rank is in the list
	for (int n=0; n<worldRanks_.nItems(); ++n)
	{
		if (worldRank_ == worldRanks_[n])
		{
			poolRank_ = n;
			Messenger::print("--> Process with world rank %i added to pool '%s' with local rank %i.\n", worldRanks_[n], name_.get(), n);
			break;
		}
	}

	Messenger::print("--> There are %i processes in pool '%s'.\n", worldRanks_.nItems(), name_.get());

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

// Return root (first) world rank of this pool
int ProcessPool::rootWorldRank()
{
	return worldRanks_.first();
}

// Determine how many simultaneous processes (groups) we can have at once, based on the Cell divisions
void ProcessPool::determineMaxProcessGroups(const Vec3<int>& divisions, const Vec3<int>& cellExtents, const List< ListVec3<int> >& neighbours)
{
#ifdef PARALLEL
	// Check that this pool actually involves us - if not we can leave now
	if (!involvesMe())
	{
		Messenger::print("--> ... Process with world rank %i is not involved in the process pool '%s', so we will ignore its setup.\n", worldRank_, name_.get());
		return true;
	}

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
	maxProcessGroups_ = 0;
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
				++maxProcessGroups_;
			}
		}
	}
	Messenger::print("--> For the current Cell division and extent values, a maximum of %i simultaneous Cell-modifying processes are possible.\n", maxProcessGroups_);
	// Maximum number of groups possible is the number of processes available, but we want all groups to contain the same number
	// of processes...
	while (worldRanks_.nItems()%maxProcessGroups_ != 0) --maxProcessGroups_;
	
	Messenger::print("--> Processes will be divided into %i groups.\n", maxProcessGroups_);
#endif
}

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
	int baseAlloc = worldRanks_.nItems() / nGroups;
	int remainder = worldRanks_.nItems() % nGroups;
	ProcessGroup* group;
	CharString rankString;
	for (int n=0; n<nGroups; ++n)
	{
		// Create nth process group and add a (currently null) entry to the groupLeaders_ array
		group = processGroups_.add();
		groupLeaders_.add(-1);
		Messenger::printVerbose("--> Created process group %i\n", n);

		// Create array of ranks to put in new group
		int firstRank = baseAlloc * n + (n < remainder ? n : remainder);
		int lastRank = firstRank + baseAlloc - (n >= remainder ? 1 : 0);
		rankString.clear();
		for (int localRank=firstRank; localRank<=lastRank; ++localRank)
		{
			int wr = worldRanks_[localRank];
			group->addProcess(localRank, wr);
			rankString.strcatf(" %i", wr);

			// If this process is the current worldRank_ we are considering, set its group membership
			if (wr == worldRank_) groupIndex_ = n;
		}
		Messenger::printVerbose("--> Group will contain %i processes (world ranks:%s).\n", group->nProcesses(), rankString.get());
	}

	// Create local group and communicator - each process will only create and be involved in one group communicator (groupGroup_)
	MPI_Group origGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &origGroup);
	if (MPI_Group_incl(origGroup, myGroup()->nProcesses(), myGroup()->worldRanks().array(), &groupGroup_) != MPI_SUCCESS) return false;
	if (MPI_Comm_create(MPI_COMM_WORLD, groupGroup_, &groupCommunicator_) != MPI_SUCCESS) return false;
	MPI_Group_rank(groupGroup_, &groupRank_);
	Messenger::printVerbose("--> ... Process with pool rank %i (world rank %i) has local group %i, group rank %i, and is a process group %s\n", poolRank_, worldRank_, groupIndex_, groupRank_, groupLeader() ? "leader" : "slave");

	// Master now assembles list of group leaders
	bool leader;
	if (isMaster())
	{
		Messenger::printVerbose("Process world rank %i is the master of pool '%s', and will assemble group leaders...\n", worldRank_, name_.get());
		// Loop over process groups
		for (int group=0; group<processGroups_.nItems(); ++group)
		{
			// Query each process in the group to see if it is the leader...
			for (int n=0; n<nProcessesInGroup(group); ++n)
			{
				int prank = processGroups_[group]->poolRank(n);

				// Is this us, the master process?
				if (prank == poolRank_) leader = groupLeader();
				else if (!receive(leader, prank)) return false;
				
				// Is this process the leader?
				if (leader)
				{
					// Check for existing value
					if (groupLeaders_[group] == -1) groupLeaders_[group] = prank;
					else
					{
						Messenger::error("MPI Error: More than one process group leader for group %i.\n", group);
						return false;
					}

					Messenger::printVerbose("Process with pool rank %i (world rank %i) added to group leaders list for pool '%s'.\n", prank, worldRanks_[prank], name_.get());
				}
			}
		}
	}
	else if (!send(groupLeader(), 0)) return false;

	// Broadcast group leader list
	if (!broadcast(groupLeaders_)) return false;
	Messenger::printVerbose("--> Group leader processes are :\n");
	for (int group=0; group<processGroups_.nItems(); ++group) Messenger::printVerbose("-->    Group %3i : process rank %i\n", group, groupLeaders_[group]);

	// Create group leader communicator
	// Must first convert local pool ranks of the group leaders into world ranks, before passing this to MPI_Group_incl
	Array<int> groupLeadersW;
	for (int n=0; n<groupLeaders_.nItems(); ++n) groupLeadersW.add(worldRanks_[groupLeaders_[n]]);
	MPI_Comm_group(MPI_COMM_WORLD, &origGroup);
	if (MPI_Group_incl(origGroup, groupLeadersW.nItems(), groupLeadersW.array(), &leaderGroup_) != MPI_SUCCESS) return false;
	if (MPI_Comm_create(MPI_COMM_WORLD, leaderGroup_, &leaderCommunicator_) != MPI_SUCCESS) return false;
#else
	// No MPI, but must still setup a dummy process group
	ProcessGroup* group = processGroups_.add();
	group->addProcess(0, 0);
	groupIndex_ = 0;
	groupRank_ = 0;
#endif

	return true;
}

// Assign processes to groups taken from supplied ProcessPool
bool ProcessPool::assignProcessesToGroups(ProcessPool& groupsSource)
{
	/*
	 * Since we have the ability to run Modules with any ProcessPool and at any point, we must occasionally
	 * re-assign the processes in the pool (typically the DUQ::worldPool_) to a different set of groups in
	 * order to utilise all available processing power (e.g. when a Module is run as, or is performing, a
	 * post-processing step.
	 */

	// If we have been supplied with ourself as the reference ProcessPool, we can exit gracefully now
	if (this == &groupsSource) return true;

	// First check that we are allowed to modify the groups within this pool
	if (!groupsModifiable_)
	{
		Messenger::error("Tried to modify the group contents of a ProcessPool in which it has explicitly been fixed.\n");
		return false;
	}

#ifdef PARALLEL
	// All processes in this pool first abandon their current groupGroup_ and leaderGroup_ communicators and groups
	MPI_Group_free(groupGroup_);
	MPI_Comm_free(groupCommunicator_);
	MPI_Group_free(leaderGroup_);
	MPI_Comm_free(leaderCommunicator_);
#endif

	// Copy over the number of allowable groups from the source ProcessPool
	maxProcessGroups_ = groupsSource.maxProcessGroups_;
	if (!assignProcessesToGroups())
	{
		Messenger::error("Failed to re-assign processes to groups in this ProcessPool.\n");
		return false;
	}

	return true;
}

// Return number of process groups
int ProcessPool::nProcessGroups() const
{
	return processGroups_.nItems();
}

// Return nth process group
ProcessGroup* ProcessPool::processGroup(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= processGroups_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Specified groupId (%i) is out of range in ProcessPool::processGroup() (nProcessGroups = %i).\n", n, processGroups_.nItems());
		return 0;
	}
#endif
	return processGroups_[n];
}

// Return number of processes in specified group
int ProcessPool::nProcessesInGroup(int groupId)
{
#ifdef CHECKS
	if ((groupId < 0) || (groupId >= processGroups_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Specified groupId (%i) is out of range in ProcessPool::nProcessesInGroup() (nProcessGroups = %i).\n", groupId, processGroups_.nItems());
		return 0;
	}
#endif
	return processGroups_[groupId]->nProcesses();
}

// Return array of pool ranks in specified group
int* ProcessPool::poolRanksInGroup(int groupId)
{
#ifdef CHECKS
	if ((groupId < 0) || (groupId >= processGroups_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Specified groupId (%i) is out of range in ProcessPool::worldRanksInGroup() (nProcessGroups = %i).\n", groupId, processGroups_.nItems());
		return 0;
	}
#endif
	return processGroups_[groupId]->poolRanks().array();
}

// Return whether group data is modifiable
bool ProcessPool::groupsModifiable()
{
	return groupsModifiable_;
}

// Prevent group data from being modified
void ProcessPool::setGroupsFixed()
{
	groupsModifiable_ = false;
}

/*
 * Process Limits
 */

// Setup limits based on total nAtoms and nGrains
bool ProcessPool::calculateLimits(int nAtoms, int nGrains)
{
	// Don't do anything if this pool doesn't involve us
#ifdef PARALLEL
	if (!involvesMe()) return true;
#endif
	long int baseAlloc, remainder;
	// Linear Atoms - Do straight division by number of processes to get basic allocation
	baseAlloc = nAtoms / worldRanks_.nItems();
	remainder = nAtoms % worldRanks_.nItems();
	linearFirstAtom_ = baseAlloc * poolRank_ + (poolRank_ < remainder ?  poolRank_: remainder);
	linearLastAtom_ = linearFirstAtom_ + baseAlloc - (poolRank_ >= remainder ? 1 : 0);
	Messenger::print("--> Nominally assigned Atoms %i to %i (%i total) to process with pool rank %i (world rank %i).\n", linearFirstAtom_, linearLastAtom_, 1+linearLastAtom_-linearFirstAtom_, poolRank_, worldRank_);

	// Linear Grains - Do straight division by number of processes to get basic allocation
	baseAlloc = nGrains / worldRanks_.nItems();
	remainder = nGrains % worldRanks_.nItems();
	linearFirstGrain_ = baseAlloc * poolRank_ + (poolRank_ < remainder ?  poolRank_: remainder);
	linearLastGrain_ = linearFirstGrain_ + baseAlloc - (poolRank_ >= remainder ? 1 : 0);
	Messenger::print("--> Nominally assigned Grains %i to %i (%i total) to process with pool rank %i (world rank %i).\n", linearFirstGrain_, linearLastGrain_, 1+linearLastGrain_-linearFirstGrain_, poolRank_, worldRank_);
	
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

		Messenger::print("--> Assigned diagonal Atom calculation limits of %i -> %i for process with pool rank %i.\n", startAtom, finishAtom, process);

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

	return 0;
}

// Return stride for general loop
int ProcessPool::interleavedLoopStride(ProcessPool::LoopContext loopContext)
{
	if (loopContext == ProcessPool::OverPoolProcesses) return worldRanks_.nItems();
	else if (loopContext == ProcessPool::OverGroupProcesses) return nProcessesInGroup(groupIndex_);
	else if (loopContext == ProcessPool::OverGroups) return processGroups_.nItems();

	return 1;
}

/*
 * Send/Receive Functions
 */

// Wait for all processes
bool ProcessPool::wait(ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	if (MPI_Barrier(communicator(commType)) != MPI_SUCCESS) return false;
#endif
	return true;
}

// Send single integer value to target process
bool ProcessPool::send(int value, int targetWorldRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	if (MPI_Send(&value, 1, MPI_INTEGER, targetWorldRank, 0, communicator(commType)) != MPI_SUCCESS) return false;
	timer_.accumulate();
#endif
	return true;
}

// Receive single integer value from source process
bool ProcessPool::receive(int& value, int sourceWorldRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	MPI_Status status;
	if (MPI_Recv(&value, 1, MPI_INTEGER, sourceWorldRank, 0, communicator(commType), &status) != MPI_SUCCESS) return false;
	timer_.accumulate();
#endif
	return true;
}

// Send single double value to target process
bool ProcessPool::send(double value, int targetWorldRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	if (MPI_Send(&value, 1, MPI_DOUBLE, targetWorldRank, 0, communicator(commType)) != MPI_SUCCESS) return false;
	timer_.accumulate();
#endif
	return true;
}

// Receive single double value from source process
bool ProcessPool::receive(double& value, int sourceWorldRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	MPI_Status status;
	if (MPI_Recv(&value, 1, MPI_DOUBLE, sourceWorldRank, 0, communicator(commType), &status) != MPI_SUCCESS) return false;
	timer_.accumulate();
#endif
	return true;
}

// Send single bool value to target process
bool ProcessPool::send(bool value, int targetWorldRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	int data = value;
	if (MPI_Send(&data, 1, MPI_INTEGER, targetWorldRank, 0, communicator(commType)) != MPI_SUCCESS) return false;
	timer_.accumulate();
#endif
	return true;
}

// Receive single bool value from source process
bool ProcessPool::receive(bool& value, int sourceWorldRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	MPI_Status status;
	int result;
	if (MPI_Recv(&result, 1, MPI_INTEGER, sourceWorldRank, 0, communicator(commType), &status) != MPI_SUCCESS) return false;
	value = result;
	timer_.accumulate();
#endif
	return true;
}

// Send integer array data to target process
bool ProcessPool::send(int* source, int nData, int targetWorldRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	if (MPI_Send(source, nData, MPI_INTEGER, targetWorldRank, 0, communicator(commType)) != MPI_SUCCESS) return false;
	timer_.accumulate();
#endif
	return true;
}

// Receive integer array data from target process
bool ProcessPool::receive(int* source, int nData, int sourceWorldRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	MPI_Status status;
	if (MPI_Recv(source, nData, MPI_INTEGER, sourceWorldRank, 0, communicator(commType), &status) != MPI_SUCCESS) return false;
	timer_.accumulate();
#endif
	return true;
}

// Send double array data to target process
bool ProcessPool::send(double* source, int nData, int targetWorldRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	if (MPI_Send(source, nData, MPI_DOUBLE, targetWorldRank, 0, communicator(commType)) != MPI_SUCCESS) return false;
	timer_.accumulate();
#endif
	return true;
}

// Receive double array data from target process
bool ProcessPool::receive(double* source, int nData, int sourceWorldRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	MPI_Status status;
	if (MPI_Recv(source, nData, MPI_DOUBLE, sourceWorldRank, 0, communicator(commType), &status) != MPI_SUCCESS) return false;
	timer_.accumulate();
#endif
	return true;
}

/*
 * Broadcast Functions
 */

// Broadcast CharString to all Processes
bool ProcessPool::broadcast(CharString& source, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	static char buffer[4096];
	
	// Get length of string, and make a local copy to avoid the const-ness of CharString.get().
	if (poolRank_ == rootRank) strcpy(buffer, source.get());

	// Broadcast data
	if (!broadcast(buffer, rootRank, commType)) return false;
	
	if (isSlave()) source = buffer;
	timer_.accumulate();
#endif
	return true;
}

// Broadcast char data to all Processes
bool ProcessPool::broadcast(char* source, int rootRank, ProcessPool::CommunicatorType commType)
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
			Messenger::print("Failed to broadcast char length data from root rank %i.\n", rootRank);
			return false;
		}
		
		// Now broadcast character data
		if (MPI_Bcast(source, length, MPI_CHARACTER, rootRank, communicator(commType)) != MPI_SUCCESS)
		{
			Messenger::print("Failed to broadcast char data from root rank %i.\n", rootRank);
			return false;
		}
	}
	else
	{
		// Slaves receive the data into the buffer, and then set the source string.
		// Length first...
		if (MPI_Bcast(&length, 1, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
		{
			Messenger::print("Slave %i (world rank %i) failed to receive char length data from root rank %i.\n", poolRank_, worldRank_, rootRank);
			return false;
		}
		
		if (MPI_Bcast(source, length, MPI_CHARACTER, rootRank, communicator(commType)) != MPI_SUCCESS)
		{
			Messenger::print("Slave %i (world rank %i) failed to receive char data from root rank %i.\n", poolRank_, worldRank_, rootRank);
			return false;
		}
	}
	timer_.accumulate();
#endif
	return true;
}

// Broadcast Vec3<int> to all Processes
bool ProcessPool::broadcast(Vec3<int>& source, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	double buffer[3];
	if (poolRank_ == rootRank)
	{
		// Construct an array from the data...
		buffer[0] = source.x;
		buffer[1] = source.y;
		buffer[2] = source.z;
		if (MPI_Bcast(buffer, 3, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
		{
			Messenger::print("Failed to broadcast Vec3<int> data from root rank %i.\n", rootRank);
			return false;
		}
	}
	else
	{
		// Slaves receive the data into the buffer, and then set the source variable.
		if (MPI_Bcast(buffer, 3, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
		{
			Messenger::print("Slave %i (world rank %i) failed to receive Vec3<int> data from root rank %i (world rank %i).\n", poolRank_, worldRank_, rootRank);
			return false;
		}
		source.x = buffer[0];
		source.y = buffer[1];
		source.z = buffer[2];
	}
	timer_.accumulate();
#endif
	return true;
}

// Broadcast Vec3<double> to all Processes
bool ProcessPool::broadcast(Vec3<double>& source, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	double buffer[3];
	if (poolRank_ == rootRank)
	{
		// Construct an array from the data...
		buffer[0] = source.x;
		buffer[1] = source.y;
		buffer[2] = source.z;
		if (MPI_Bcast(buffer, 3, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
		{
			Messenger::print("Failed to broadcast Vec3<double> data from root rank %i.\n", rootRank);
			return false;
		}
	}
	else
	{
		// Slaves receive the data into the buffer, and then set the source variable.
		if (MPI_Bcast(buffer, 3, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
		{
			Messenger::print("Slave %i (world rank %i) failed to receive Vec3<double> data from root rank %i (world rank %i).\n", poolRank_, worldRank_, rootRank);
			return false;
		}
		source.x = buffer[0];
		source.y = buffer[1];
		source.z = buffer[2];
	}
	timer_.accumulate();
#endif
	return true;
}

// Broadcast single integer
bool ProcessPool::broadcast(int& source, int rootRank, ProcessPool::CommunicatorType commType)
{
	return broadcast(&source, 1, rootRank, commType);
}

// Broadcast integer(s) to all Processes
bool ProcessPool::broadcast(int* source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	if (MPI_Bcast(source, count, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
	{
		Messenger::print("Failed to broadcast int data from root rank %i.\n", rootRank);
		return false;
	}
	timer_.accumulate();
#endif
	return true;
}

// Broadcast long integer to all Processes
bool ProcessPool::broadcast(long int& source, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	if (MPI_Bcast(&source, 1, MPI_LONG, rootRank, communicator(commType)) != MPI_SUCCESS)
	{
		Messenger::print("Failed to broadcast long int data from root rank %i.\n", rootRank);
		return false;
	}
	timer_.accumulate();
#endif
	return true;
}

// Broadcast single double
bool ProcessPool::broadcast(double& source, int rootRank, ProcessPool::CommunicatorType commType)
{
	return broadcast(&source, 1, rootRank, commType);
}

// Broadcast double(s) to all Processes
bool ProcessPool::broadcast(double* source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	if (MPI_Bcast(source, count, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
	{
		Messenger::print("Failed to broadcast int data from root rank %i.\n", rootRank);
		return false;
	}
	timer_.accumulate();
#endif
	return true;
}

// Broadcast float(s) to all Processes
bool ProcessPool::broadcast(float* source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	if (MPI_Bcast(source, count, MPI_FLOAT, rootRank, communicator(commType)) != MPI_SUCCESS)
	{
		Messenger::print("Failed to broadcast float data from root rank %i.\n", rootRank);
		return false;
	}
	timer_.accumulate();
#endif

	return true;
}

// Broadcast bool to all Processes
bool ProcessPool::broadcast(bool& source, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	int result = source;
	if (MPI_Bcast(&result, 1, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
	{
		Messenger::print("Failed to broadcast int data from root rank %i.\n", rootRank);
		return false;
	}
	source = result;
	timer_.accumulate();
#endif
	return true;
}

// Broadcast Array<int>
bool ProcessPool::broadcast(Array<int>& array, int rootRank, ProcessPool::CommunicatorType commType)
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
			Messenger::print("Failed to broadcast Array<int> size from root rank %i (world rank %i).\n", rootRank, worldRanks_[rootRank]);
			return false;
		}

		// Now broadcast Array data
		if (length > 0)
		{
			if (MPI_Bcast(array.array(), length, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Failed to broadcast Array<int> data from root rank %i (world rank %i).\n", rootRank, worldRanks_[rootRank]);
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
			Messenger::print("Slave %i (world rank %i) failed to receive Array<int> size from root rank %i.\n", poolRank_, worldRank_, rootRank);
			return false;
		}

		if (length > 0)
		{
			// Create array of specified size
			array.initialise(length);

			if (MPI_Bcast(array.array(), length, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array<int> data from root rank %i.\n", poolRank_, worldRank_, rootRank);
				return false;
			}
		}
		else array.clear();
	}

	timer_.accumulate();
#endif
	return true;
}

// Broadcast Array<double>
bool ProcessPool::broadcast(Array<double>& array, int rootRank, ProcessPool::CommunicatorType commType)
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
			Messenger::print("Failed to broadcast Array<double> size from root rank %i.\n", rootRank);
			return false;
		}

		// Now broadcast Array data
		if (length > 0)
		{
			if (MPI_Bcast(array.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Failed to broadcast Array<double> data from root rank %i.\n", rootRank);
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
			Messenger::print("Slave %i (world rank %i) failed to receive Array<double> size from root rank %i.\n", poolRank_, worldRank_, rootRank);
			return false;
		}

		if (length > 0)
		{
			// Create array of specified size
			array.initialise(length);

			if (MPI_Bcast(array.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array<double> data from root rank %i.\n", poolRank_, worldRank_, rootRank);
				return false;
			}
		}
		else array.clear();
	}

	timer_.accumulate();
#endif
	return true;
}

// Broadcast Array< Vec3<int> >
bool ProcessPool::broadcast(Array< Vec3<int> >& array, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();

	// Working arrays (static)
	static Array<int> tempx, tempy, tempz;

	int length;
	if (poolRank_ == rootRank)
	{
		// Broadcast array length first...
		length = array.nItems();
		if (MPI_Bcast(&length, 1, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
		{
			Messenger::print("Failed to broadcast Array< Vec3<int> > size from root rank %i.\n", rootRank);
			return false;
		}

		// Now broadcast Array data
		if (length > 0)
		{
			// Construct local array data to broadcast
			tempx.initialise(length);
			tempy.initialise(length);
			tempz.initialise(length);
			for (int n=0; n<length; ++n)
			{
				tempx[n] = array[n].x;
				tempy[n] = array[n].y;
				tempz[n] = array[n].z;
			}

			if (MPI_Bcast(tempx.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Failed to broadcast Array< Vec3<int> > (x) data from root rank %i.\n", rootRank);
				return false;
			}
			if (MPI_Bcast(tempy.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Failed to broadcast Array< Vec3<int> > (y) data from root rank %i.\n", rootRank);
				return false;
			}
			if (MPI_Bcast(tempz.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Failed to broadcast Array< Vec3<int> > (z) data from root rank %i.\n", rootRank);
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
			Messenger::print("Slave %i (world rank %i) failed to receive Array< Vec3<int> > size from root rank %i.\n", poolRank_, worldRank_, rootRank);
			return false;
		}

		if (length > 0)
		{
			// Create array of specified size, and temporary storage arrays
			array.initialise(length);
			tempx.initialise(length);
			tempy.initialise(length);
			tempz.initialise(length);

			if (MPI_Bcast(tempx.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array< Vec3<int> > (x) data from root rank %i.\n", poolRank_, worldRank_, rootRank);
				return false;
			}
			if (MPI_Bcast(tempy.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array< Vec3<int> > (y) data from root rank %i.\n", poolRank_, worldRank_, rootRank);
				return false;
			}
			if (MPI_Bcast(tempz.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array< Vec3<int> > (z) data from root rank %i.\n", poolRank_, worldRank_, rootRank);
				return false;
			}

			// Store received data in array
			for (int n=0; n<length; ++n)
			{
				array[n].x = tempx[n];
				array[n].y = tempy[n];
				array[n].z = tempz[n];
			}
		}
		else array.clear();
	}

	timer_.accumulate();
#endif
	return true;
}

// Broadcast Array< Vec3<double> >
bool ProcessPool::broadcast(Array< Vec3<double> >& array, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();

	// Working arrays (static)
	static Array<double> tempx, tempy, tempz;

	int length;
	if (poolRank_ == rootRank)
	{
		// Broadcast array length first...
		length = array.nItems();
		if (MPI_Bcast(&length, 1, MPI_INTEGER, rootRank, communicator(commType)) != MPI_SUCCESS)
		{
			Messenger::print("Failed to broadcast Array< Vec3<double> > size from root rank %i.\n", rootRank);
			return false;
		}

		// Now broadcast Array data
		if (length > 0)
		{
			// Construct local array data to broadcast
			tempx.initialise(length);
			tempy.initialise(length);
			tempz.initialise(length);
			for (int n=0; n<length; ++n)
			{
				tempx[n] = array[n].x;
				tempy[n] = array[n].y;
				tempz[n] = array[n].z;
			}

			if (MPI_Bcast(tempx.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Failed to broadcast Array< Vec3<double> > (x) data from root rank %i.\n", rootRank);
				return false;
			}
			if (MPI_Bcast(tempy.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Failed to broadcast Array< Vec3<double> > (y) data from root rank %i.\n", rootRank);
				return false;
			}
			if (MPI_Bcast(tempz.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Failed to broadcast Array< Vec3<double> > (z) data from root rank %i.\n", rootRank);
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
			Messenger::print("Slave %i (world rank %i) failed to receive Array< Vec3<double> > size from root rank %i.\n", poolRank_, worldRank_, rootRank);
			return false;
		}

		if (length > 0)
		{
			// Create array of specified size, and temporary storage arrays
			array.initialise(length);
			tempx.initialise(length);
			tempy.initialise(length);
			tempz.initialise(length);

			if (MPI_Bcast(tempx.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array< Vec3<double> > (x) data from root rank %i.\n", poolRank_, worldRank_, rootRank);
				return false;
			}
			if (MPI_Bcast(tempy.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array< Vec3<double> > (y) data from root rank %i.\n", poolRank_, worldRank_, rootRank);
				return false;
			}
			if (MPI_Bcast(tempz.array(), length, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array< Vec3<double> > (z) data from root rank %i.\n", poolRank_, worldRank_, rootRank);
				return false;
			}

			// Store received data in array
			for (int n=0; n<length; ++n)
			{
				array[n].x = tempx[n];
				array[n].y = tempy[n];
				array[n].z = tempz[n];
			}
		}
		else array.clear();
	}

	timer_.accumulate();
#endif
	return true;
}

// Broadcast Array2D<double>
bool ProcessPool::broadcast(Array2D<double>& array, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();

	int nRows, nColumns;
	bool half;
	if (poolRank_ == rootRank)
	{
		// Broadcast array size first...
		nRows = array.nRows();
		if (!broadcast(nRows, rootRank))
		{
			Messenger::print("Failed to broadcast Array2D<double> nRows from root rank %i.\n", rootRank);
			return false;
		}
		nColumns = array.nColumns();
		if (!broadcast(nColumns, rootRank))
		{
			Messenger::print("Failed to broadcast Array2D<double> nColmnns from root rank %i.\n", rootRank);
			return false;
		}
		half = array.halved();
		if (!broadcast(half, rootRank))
		{
			Messenger::print("Failed to broadcast Array2D<double> half-diagonal status from root rank %i.\n", rootRank);
			return false;
		}
		
		// Now broadcast Array data
		if ((nRows*nColumns) > 0)
		{
			if (MPI_Bcast(array.linearArray(), nRows*nColumns, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Failed to broadcast Array2D<double> data from root rank %i.\n", rootRank);
				return false;
			}
		}
	}
	else
	{
		// Slaves receive the size, and then create and receive the array
		if (!broadcast(nRows, rootRank))
		{
			Messenger::print("Slave %i (world rank %i) failed to receive Array2D<double> nRows from root rank %i.\n", poolRank_, worldRank_, rootRank);
			return false;
		}
		if (!broadcast(nColumns, rootRank))
		{
			Messenger::print("Slave %i (world rank %i) failed to receive Array2D<double> nRows from root rank %i.\n", poolRank_, worldRank_, rootRank);
			return false;
		}
		if (!broadcast(half, rootRank))
		{
			Messenger::print("Slave %i (world rank %i) failed to receive Array2D<double> halved status from root rank %i.\n", poolRank_, worldRank_, rootRank);
			return false;
		}

		// Resize and receive array
		array.initialise(nRows, nColumns, half);
		if ((nRows*nColumns) > 0)
		{
			if (MPI_Bcast(array.linearArray(), nRows*nColumns, MPI_DOUBLE, rootRank, communicator(commType)) != MPI_SUCCESS)
			{
				Messenger::print("Slave %i (world rank %i) failed to receive ArrayD<double> data from root rank %i.\n", poolRank_, worldRank_, rootRank);
				return false;
			}
		}
		else array.clear();
	}

	timer_.accumulate();
#endif
	return true;
}

/*
 * Special Array Functions
 */

// Reduce (sum) double data to root rank
bool ProcessPool::sum(double* source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	// If we are the target process then we need to construct a temporary buffer to store the received data in.
	if ((commType == ProcessPool::Leaders) && (!groupLeader())) return true;
	if (poolRank_ == rootRank)
	{
		double buffer[count];
		if (MPI_Reduce(source, buffer, count, MPI_DOUBLE, MPI_SUM, rootRank, communicator(commType)) != MPI_SUCCESS) return false;
		// Put reduced data back into original buffer
		for (int n=0; n<count; ++n) source[n] = buffer[n];
	}
	else
	{
		// Not the target process, so just send data
		if (MPI_Reduce(source, NULL, count, MPI_DOUBLE, MPI_SUM, rootRank, communicator(commType)) != MPI_SUCCESS) return false;
	}
	timer_.accumulate();
#endif
	return true;
}

// Reduce (sum) int data to root rank
bool ProcessPool::sum(int* source, int count, int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	// If we are the target process then we need to construct a temporary buffer to store the received data in.
	if ((commType == ProcessPool::Leaders) && (!groupLeader())) return true;
	if (poolRank_ == rootRank)
	{
		int buffer[count];
		if (MPI_Reduce(source, buffer, count, MPI_INTEGER, MPI_SUM, rootRank, communicator(commType)) != MPI_SUCCESS) return false;
		// Put reduced data back into original buffer
		for (int n=0; n<count; ++n) source[n] = buffer[n];
	}
	else
	{
		// Not the target process, so just send data
		if (MPI_Reduce(source, NULL, count, MPI_INTEGER, MPI_SUM, rootRank, communicator(commType)) != MPI_SUCCESS) return false;
	}
	timer_.accumulate();
#endif
	return true;
}

// Reduce (sum) double data to all processes
bool ProcessPool::allSum(double* source, int count, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	double buffer[count];
	if ((commType == ProcessPool::Leaders) && (!groupLeader())) return true;
	if (MPI_Allreduce(source, &buffer, count, MPI_DOUBLE, MPI_SUM, communicator(commType)) != MPI_SUCCESS) return false;
	// Put reduced data back into original buffer
	for (int n=0; n<count; ++n) source[n] = buffer[n];
	timer_.accumulate();
#endif
	return true;
}

// Reduce (sum) int data to all processes
bool ProcessPool::allSum(int* source, int count, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	timer_.start();
	int buffer[count];
	if ((commType == ProcessPool::Leaders) && (!groupLeader())) return true;
	if (MPI_Allreduce(source, &buffer, count, MPI_INTEGER, MPI_SUM, communicator(commType)) != MPI_SUCCESS) return false;
	// Put reduced data back into original buffer
	for (int n=0; n<count; ++n) source[n] = buffer[n];
	timer_.accumulate();
#endif
	return true;
}

// Assemble integer array for entire pool on target process
bool ProcessPool::assemble(int* array, int nData, int* rootDest, int rootMaxData, int rootRank, ProcessPool::CommunicatorType commType)
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
		for (n=0; n<nData; ++n) rootDest[n] = array[n];

		// Now get data from other processes, appending each chunk to the rootDest array
		int slaveNData;
		for (int n=0; n<worldRanks_.nItems(); ++n)
		{
			if (poolRank_ == n) continue;

			// Get length of data to receive
			if (!receive(slaveNData, n, commType)) return false;

			// Check buffer length
			if ((nData + slaveNData) > rootMaxData)
			{
				printf("MPI ERROR: Tried to assemble more data on process '%i' than the array allows for (maxData = %i).\n", rootRank, rootMaxData);
				return false;
			}
			// Receive data
			if (!receive(&rootDest[nData], slaveNData, n, commType)) return false;
			nData += slaveNData;
		}
	}
	else
	{
		// Slaves send array size and then the data to the master...
		if (!send(nData, rootRank, commType)) return false;
		if (!send(array, nData, rootRank, commType)) return false;
	}
	timer_.accumulate();
#endif
	return true;
}

// Assemble double array for entire pool on target process
bool ProcessPool::assemble(double* array, int nLocalData, double* rootDest, int rootMaxData, int rootRank, ProcessPool::CommunicatorType commType)
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
		for (int n=0; n<nLocalData; ++n) rootDest[n] = array[n];

		// Now get data from other processes, appending each chunk to the rootDest array
		int slaveNData;
		for (int n=0; n<worldRanks_.nItems(); ++n)
		{
			if (poolRank_ == n) continue;
			
			// Get length of data to receive
			if (!receive(slaveNData, n, commType)) return false;
			
			// Check buffer length
			if ((nLocalData + slaveNData) > rootMaxData)
			{
				printf("MPI ERROR: Tried to assemble more data on process '%i' than the array allows for (maxData = %i).\n", rootRank, rootMaxData);
				return false;
			}
			// Receive data
			if (!receive(&rootDest[nLocalData], slaveNData, n, commType)) return false;
			nLocalData += slaveNData;
		}
	}
	else
	{
		// Slaves send array size and then the data to the master...
		if (!send(nLocalData, rootRank, commType)) return false;
		if (!send(array, nLocalData, rootRank, commType)) return false;
	}
	timer_.accumulate();
#endif
	return true;
}

// Assemble Array<double> for entire pool on target process
bool ProcessPool::assemble(Array<double>& array, int nData, Array<double>& rootDest, int rootMaxData, int rootRank, ProcessPool::CommunicatorType commType)
{
	if (poolRank_ == rootRank)
	{
		if (rootDest.size() < rootMaxData)
		{
			Messenger::error("Destination Array<double> in ProcessPool::assemble() is not large enough.");
			stop(rootRank, commType);
			return false;
		}
		else proceed(rootRank, commType);
	}
	else if (!decision(rootRank, commType)) return false;

	// Call double* version of routine...
	return assemble(array.array(), nData, rootDest.array(), rootMaxData, rootRank, commType);
}

/*
 * Decisions
 */

// Broadcast logical decision to proceed to all processes (Master only)
void ProcessPool::proceed(int rootRank, ProcessPool::CommunicatorType commType)
{
	bool decision = true;
#ifdef PARALLEL
	if (!broadcast(decision, rootRank, commType)) Messenger::print("Error telling processes to proceed.\n");
#endif
}

// Broadcast logical decision to stop to all processes (Master only)
void ProcessPool::stop(int rootRank, ProcessPool::CommunicatorType commType)
{
	bool decision = false;
#ifdef PARALLEL
	if (!broadcast(decision, rootRank, commType)) Messenger::print("Error telling processes to stop.\n");
#endif
}

// Receive logical decision from master (Slaves only)
bool ProcessPool::decision(int rootRank, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	bool data;
	if (!broadcast(data, rootRank, commType)) Messenger::print("Error receiving decision from root rank %i.\n", rootRank);
	return data;
#endif
	return true;
}

// Test the supplied condition over all processes, returning true only if they all report truth
bool ProcessPool::allTrue(bool value, ProcessPool::CommunicatorType commType)
{
#ifdef PARALLEL
	// First, sum all bool values of the processes in the pool
	int summedResult = (value ? 1 : 0);
	if (!allSum(&summedResult, 1, commType)) return false;
	if (commType == ProcessPool::Leaders) return (summedResult == groupLeaders_.nItems());
	else return (summedResult == nProcesses());
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
void ProcessPool::initialiseRandomBuffer(ProcessPool::CommunicatorType commType)
{
	randomBufferCommGroup_ = commType;
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
