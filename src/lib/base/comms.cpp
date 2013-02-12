/*
	*** Parallel Communications
	*** src/lib/base/comms.cpp
	Copyright T. Youngs 2012-2013

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

#include "base/comms.h"
#include "base/messenger.h"
#include "base/constants.h"
#include <string.h>

// External Declarations
dUQComm Comm;
int dUQComm::FAILED = 0;
int dUQComm::SUCCEEDED = 1;
int dUQComm::RESULT;

/*!
 * \brief Constructor
 * \details Constructor for dUQComm.
 */
dUQComm::dUQComm()
{
	nProcesses_ = 1;
	rank_ = 0;
	localGroupIndex_ = -1;
	localGroupRank_ = -1;
}

#ifdef PARALLEL
/*!
 * \brief Return communicator specified
 * \details Return the MPI Communicator requested
 */
MPI_Comm dUQComm::communicator(dUQComm::CommGroup group)
{
	if (group == dUQComm::Group) return localCommunicator_;
	else if (group == dUQComm::Leaders) return leaderCommunicator_;
	else if (group == dUQComm::World) return MPI_COMM_WORLD;
	msg.print("BAD_USAGE - Communicator %i is not valid in dUQComm::communicator().\n", group);
	return MPI_COMM_WORLD;
}
#endif


/*
// World Comms
*/

/*!
 * \brief Initialise parallel communications
 */
bool dUQComm::initialise(int* argn, char*** argv)
{
	// Initialise MPI
#ifdef PARALLEL
	msg.printVerbose("Initialising MPI...\n");
	if (MPI_Init(argn, argv) == MPI_SUCCESS)
	{
		msg.print("Initialised MPI.\n");
		if (MPI_Comm_size(MPI_COMM_WORLD, &nProcesses_))
		{
			msg.error("Failed to get world size.\n");
			return FALSE;
		}
		msg.print("Number of processes = %i\n", nProcesses_);

		if (MPI_Comm_rank(MPI_COMM_WORLD, &rank_))
		{
			msg.error("Failed to get process rank.\n");
			return FALSE;
		}
	}
	else
	{
		msg.error("Failed to initialise MPI.\n");
		return FALSE;
	}
#endif
	return TRUE;
}

/*!
 * \brief End parallel communications
 */
bool dUQComm::finalise()
{
#ifdef PARALLEL
	MPI_Finalize();
#endif
	return TRUE;
}

/*!
 * \brief Return number of processes
 */
int dUQComm::nProcesses()
{
	return nProcesses_;
}

/*!
 * \brief Return rank of this process
 */
int dUQComm::rank()
{
	return rank_;
}

/*!
 * \brief Return whether this process is the master
 */
bool dUQComm::master()
{
	return (rank_ == 0);
}

/*!
 * \brief Return whether this process is a slave
 */
bool dUQComm::slave()
{
	return (rank_ != 0);
}

/*!
 * \brief Reset accumulated Comm time
 */
void dUQComm::resetAccumulatedTime()
{
	accumTime_.zero();
}

/*!
 * \brief Return accumulated time string
 */
const char* dUQComm::accumulatedTimeString()
{
	return accumTime_.timeString();
}

/*!
 * \brief Return total time string
 */
const char* dUQComm::totalTimeString()
{
	return totalTime_.timeString();
}

/*
// Parallel Strategy
*/

/*!
 * \brief Setup strategy
 */
bool dUQComm::setupStrategy(const Vec3<int>& divisions, const Vec3<int>& cellExtents, const List< ListVec3<int> >& neighbours)
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
	msg.print("For the current Cell division and extent values, %i simultaneous Cell-modifying processes are possible.\n", nGroups);
	// Maximum number of groups possible is the number of processes available, but we want all groups to contain the same number
	// of processes...
	while (nProcesses_%nGroups != 0) --nGroups;
	
	msg.print("Processes will be divided into %i groups.\n", nGroups);

	// Create process groups
	int baseAlloc = nProcesses_ / nGroups;
	int remainder = nProcesses_ % nGroups;
	int rank, firstRank, lastRank;
	Array<int>* group;
	Dnchar s;
	for (int n=0; n<nGroups; ++n)
	{
		// Create process group and add a (currently null) entry to the groupLeaders_ array
		group = processGroups_.add();
		groupLeaders_.add(-1);
		msg.print("Created process group %i\n", n);

		// Create array of ranks to put in new group
		firstRank = baseAlloc * n + (n < remainder ? n : remainder);
		lastRank = firstRank + baseAlloc - (n >= remainder ? 1 : 0);
		s.clear();
		for (rank = firstRank; rank <=lastRank; ++rank)
		{
			group->add(rank);
			if (rank_ == rank) localGroupIndex_ = n;
			s.strcatf(" %i", rank);
		}
		msg.print("--> Group will contain %i processes :%s\n", group->nItems(), s.get());
	}

	// Create MPI groups and communicators
	MPI_Group origGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &origGroup);
	if (MPI_Group_incl(origGroup, localGroupSize(), processes(localGroupIndex_), &localGroup_) != MPI_SUCCESS) return FALSE;
	if (MPI_Comm_create(MPI_COMM_WORLD, localGroup_, &localCommunicator_) != MPI_SUCCESS) return FALSE;
	MPI_Group_rank(localGroup_, &localGroupRank_);
	msg.printVerbose("Process with world rank %i has local group (%i) rank %i, pgl? = %i\n", rank_, localGroupIndex_, localGroupRank_, processGroupLeader());

	// Master now assembles list of group leaders
	bool leader;
	if (Comm.master())
	{
		// Loop over process groups
		for (int group=0; group<processGroups_.nItems(); ++group)
		{
			// Query each process in the group to see if it is the leader...
			for (int n=0; n<nProcesses(group); ++n)
			{
				// Is this us?
				if (processes(group)[n] == rank_) leader = processGroupLeader();
				else if (!Comm.receive(leader, processes(group)[n])) return FALSE;
				
// 				printf("Group %i, process el %i : leader = %i\n", group, n, leader);
				
				// Is this process the leader?
				if (leader)
				{
					// Check for existing value
					if (groupLeaders_[group] == -1) groupLeaders_[group] = processes(group)[n];
					else
					{
						msg.print("MPI Error: More than one process group leader for group %i.\n", group);
						return FALSE;
					}
				}
			}
		}
	}
	else if (!Comm.send(processGroupLeader(), 0)) return FALSE;
	
	// Broadcast group leader list
	if (!Comm.broadcast(groupLeaders_.array(), processGroups_.nItems())) return FALSE;
	msg.print("Group leader processes are :\n");
	for (int group=0; group<processGroups_.nItems(); ++group) msg.print("--> Group %3i : process rank %i\n", group, groupLeaders_[group]);

	// Create group leader communicator
	MPI_Comm_group(MPI_COMM_WORLD, &origGroup);
	if (MPI_Group_incl(origGroup, processGroups_.nItems(), groupLeaders_.array(), &leaderGroup_) != MPI_SUCCESS) return FALSE;
	if (MPI_Comm_create(MPI_COMM_WORLD, leaderGroup_, &leaderCommunicator_) != MPI_SUCCESS) return FALSE;
#else
	// No MPI, but must still setup a dummy process group
	Array<int>* procList = processGroups_.add();
	procList->add(0);
	localGroupIndex_ = 0;
	localGroupRank_ = 0;
#endif
	return TRUE;
}

/*!
 * \brief Return number of process groups
 */
int dUQComm::nProcessGroups() const
{
	return processGroups_.nItems();
}

/*!
 * \brief Return number of processes in nth group
 */
int dUQComm::nProcesses(int groupId)
{
#ifdef CHECKS
	if ((groupId < 0) || (groupId >= processGroups_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Specified groupId (%i) is out of range in dUQComm::nProcesses() (nProcessGroups = %i).\n", groupId, processGroups_.nItems());
		return 0;
	}
#endif
	return processGroups_[groupId]->nItems();
}

/*!
 * \brief Return process array of nth group
 */
int* dUQComm::processes( int groupId )
{
#ifdef CHECKS
	if ((groupId < 0) || (groupId >= processGroups_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Specified groupId (%i) is out of range in dUQComm::processes() (nProcessGroups = %i).\n", groupId, processGroups_.nItems());
		return 0;
	}
#endif
	return processGroups_[groupId]->array();
}

/*!
 * \brief Return group index for this process
 */
int dUQComm::localGroupIndex()
{
	return localGroupIndex_;
}

/*!
 * \brief Return group size for this process
 */
int dUQComm::localGroupSize()
{
#ifdef CHECKS
	if ((localGroupIndex_ < 0) || (localGroupIndex_ >= processGroups_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Local group index for this process (%i) is out of range in dUQComm::localGroupSize() (nProcessGroups = %i).\n", localGroupIndex_, processGroups_.nItems());
		return 0;
	}
#endif
	return processGroups_[localGroupIndex_]->nItems();
}

/*!
 * \brief Return group rank of this process
 */
int dUQComm::localGroupRank()
{
	return localGroupRank_;
}

/*!
 * \brief Return whether this process is a group leader
 */
bool dUQComm::processGroupLeader()
{
	return (localGroupRank_ == 0);
}

/*
// Process Limits
*/

/*!
 * \brief Setup limits base on total nAtoms and nGrains
 */
bool dUQComm::calculateLimits(int nAtoms, int nGrains)
{
	long int baseAlloc, remainder;
	// Linear Atoms - Do straight division by number of processes to get basic allocation
	baseAlloc = nAtoms / nProcesses_;
	remainder = nAtoms % nProcesses_;
	linearFirstAtom_ = baseAlloc * rank_ + (rank_ < remainder ?  rank_: remainder);
	linearLastAtom_ = linearFirstAtom_ + baseAlloc - (rank_ >= remainder ? 1 : 0);
	msg.print("--> Nominally assigned Atoms %i to %i (%i total) to process with rank %i.\n", linearFirstAtom_, linearLastAtom_, 1+linearLastAtom_-linearFirstAtom_, rank_);

	// Linear Grains - Do straight division by number of processes to get basic allocation
	baseAlloc = nGrains / nProcesses_;
	remainder = nGrains % nProcesses_;
	linearFirstGrain_ = baseAlloc * rank_ + (rank_ < remainder ?  rank_: remainder);
	linearLastGrain_ = linearFirstGrain_ + baseAlloc - (rank_ >= remainder ? 1 : 0);
	msg.print("--> Nominally assigned Grains %i to %i (%i total) to process with rank %i.\n", linearFirstGrain_, linearLastGrain_, 1+linearLastGrain_-linearFirstGrain_, rank_);
	
	// Diagonal Atoms - For calculation of upper-diagonal half of any two-body interaction matrix
	if (master())
	{
		double rnproc = 1.0 / nProcesses_, area = 1.0;
		int startAtom = 0, finishAtom;

		// Loop over processes
		for (int process = 0; process<nProcesses_; ++process)
		{
			// If this is the last process, make sure we avoid doing sqrt of zero or delta-neg value
			if (process == (nProcesses_-1)) finishAtom = nAtoms - 1;
			else finishAtom = (1.0 - sqrt(area - rnproc)) * nAtoms - 1;
			area -= rnproc;

			// Store / send data
			if (process == 0)
			{
				diagonalFirstAtom_ = startAtom;
				diagonalLastAtom_ = finishAtom;
			}
			else
			{
#ifdef PARALLEL
				if (MPI_Send(&startAtom, 1, MPI_INTEGER, process, 0, MPI_COMM_WORLD) != MPI_SUCCESS) return FALSE;
				if (MPI_Send(&finishAtom, 1, MPI_INTEGER, process, 1, MPI_COMM_WORLD) != MPI_SUCCESS) return FALSE;
#endif
			}

			msg.print("--> Assigned diagonal Atom calculation limits of %i -> %i for process with rank %i.\n", startAtom, finishAtom, process);

			// Update startAtom
			startAtom = finishAtom+1;
		}
	}
	else
	{
		// Slaves just wait to receive their limits
#ifdef PARALLEL
		MPI_Status mpiStatus;
		if (MPI_Recv(&diagonalFirstAtom_, 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, &mpiStatus) != MPI_SUCCESS) return FALSE;
		if (MPI_Recv(&diagonalLastAtom_, 1, MPI_INTEGER, 0, 1, MPI_COMM_WORLD, &mpiStatus) != MPI_SUCCESS) return FALSE;
#endif
	}
	
	return TRUE;
}

/*!
 * \brief Return linear first Atom index
 */
int dUQComm::linearFirstAtom()
{
	return linearFirstAtom_;
}

/*!
 * \brief Return linear last Atom index
 */
int dUQComm::linearLastAtom()
{
	return linearLastAtom_;
}

/*!
 * \brief Return linear first Grain index
 */
int dUQComm::linearFirstGrain()
{
	return linearFirstGrain_;
}

/*!
 * \brief Return linear last Grain index
 */
int dUQComm::linearLastGrain()
{
	return linearLastGrain_;
}

/*!
 * \brief Return diagonal first Atom index
 */
int dUQComm::diagonalFirstAtom()
{
	return diagonalFirstAtom_;
}

/*!
 * \brief Return diagonal last Atom index
 */
int dUQComm::diagonalLastAtom()
{
	return diagonalLastAtom_;
}

/*!
 * \brief Return starting index for general loop
 */
int dUQComm::interleavedLoopStart(dUQComm::CommGroup group)
{
	if (group == dUQComm::Group) return localGroupRank_;
	else if (group == dUQComm::World) return rank_;
	return 0;
}

/*!
 * \brief Return stride for general loop
 */
int dUQComm::interleavedLoopStride(dUQComm::CommGroup group)
{
	if (group == dUQComm::Group) return localGroupSize();
	else if (group == dUQComm::World) return nProcesses_;
	return 1;
}

/*
// Send/Receive Functions
*/

/*!
 * \brief Wait for all processes
 */
bool dUQComm::wait(dUQComm::CommGroup group)
{
#ifdef PARALLEL
	if (MPI_Barrier(communicator(group)) != MPI_SUCCESS) return FALSE;
#endif
	return TRUE;
}

/*!
 * \brief Send single integer value to target process
 */
bool dUQComm::send(int value, int targetProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Send(&value, 1, MPI_INTEGER, targetProcess, 0, MPI_COMM_WORLD) != MPI_SUCCESS) return FALSE;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Receive single integer value from source process
 */
bool dUQComm::receive(int& value, int sourceProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	MPI_Status status;
	if (MPI_Recv(&value, 1, MPI_INTEGER, sourceProcess, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS) return FALSE;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Send single double value to target process
 */
bool dUQComm::send(double value, int targetProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Send(&value, 1, MPI_DOUBLE, targetProcess, 0, MPI_COMM_WORLD) != MPI_SUCCESS) return FALSE;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Receive single double value from source process
 */
bool dUQComm::receive(double& value, int sourceProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	MPI_Status status;
	if (MPI_Recv(&value, 1, MPI_DOUBLE, sourceProcess, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS) return FALSE;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Send single bool value to target process
 */
bool dUQComm::send(bool value, int targetProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Send(&value, 1, MPI_INTEGER, targetProcess, 0, MPI_COMM_WORLD) != MPI_SUCCESS) return FALSE;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Receive single bool value from source process
 */
bool dUQComm::receive(bool& value, int sourceProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	MPI_Status status;
	if (MPI_Recv(&value, 1, MPI_INTEGER, sourceProcess, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS) return FALSE;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Send integer array data to target process
 */
bool dUQComm::send(int* source, int nData, int targetProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Send(source, nData, MPI_INTEGER, targetProcess, 0, MPI_COMM_WORLD) != MPI_SUCCESS) return FALSE;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Receive integer array data from target process
 */
bool dUQComm::receive(int* source, int nData, int sourceProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	MPI_Status status;
	if (MPI_Recv(source, nData, MPI_INTEGER, sourceProcess, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS) return FALSE;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Send double array data to target process
 */
bool dUQComm::send(double* source, int nData, int targetProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Send(source, nData, MPI_DOUBLE, targetProcess, 0, MPI_COMM_WORLD) != MPI_SUCCESS) return FALSE;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Receive double array data from target process
 */
bool dUQComm::receive(double* source, int nData, int sourceProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	MPI_Status status;
	if (MPI_Recv(source, nData, MPI_DOUBLE, sourceProcess, 0, MPI_COMM_WORLD, &status) != MPI_SUCCESS) return FALSE;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*
// Broadcast Functions
*/

/*!
 * \brief Broadcast Dnchar to all Processes
 */
bool dUQComm::broadcast(Dnchar& source, int rootProcess, dUQComm::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	static char buffer[4096];
	
	// Get length of string, and make a local copy to avoid the const-ness of Dnchar.get().
	if (rank_ == rootProcess) strcpy(buffer, source.get());

	// Broadcast data
	if (!broadcast(buffer, rootProcess, group)) return FALSE;
	
	if (slave()) source = buffer;
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Broadcast char data to all Processes
 */
bool dUQComm::broadcast(char* source, int rootProcess, dUQComm::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	int length;
	if (rank_ == rootProcess)
	{
		// Broadcast string length first...
		length = strlen(source) + 1;
		if (MPI_Bcast(&length, 1, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
		{
			msg.print("Failed to broadcast char length data from rootProcess %i.\n", rootProcess);
			return FALSE;
		}
		
		// Now broadcast character data. We need to make a local copy of the string, to avoid the const-ness of Dnchar.get().
		if (MPI_Bcast(source, length, MPI_CHARACTER, rootProcess, communicator(group)) != MPI_SUCCESS)
		{
			msg.print("Failed to broadcast char data from rootProcess %i.\n", rootProcess);
			return FALSE;
		}
	}
	else
	{
		// Slaves receive the data into the buffer, and then set the source string.
		// Length first...
		if (MPI_Bcast(&length, 1, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
		{
			msg.print("Slave %i failed to receive char length data from rootProcess %i.\n", rank_, rootProcess);
			return FALSE;
		}
		
		if (MPI_Bcast(source, length, MPI_CHARACTER, rootProcess, communicator(group)) != MPI_SUCCESS)
		{
			msg.print("Slave %i failed to receive char data from rootProcess %i.\n", rank_, rootProcess);
			return FALSE;
		}
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Broadcast Vec3<double> to all Processes
 */
bool dUQComm::broadcast(Vec3<double>& source, int rootProcess, dUQComm::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	double buffer[3];
	if (rank_ == rootProcess)
	{
		// Construct an array from the data...
		buffer[0] = source.x;
		buffer[1] = source.y;
		buffer[2] = source.z;
		if (MPI_Bcast(buffer, 3, MPI_DOUBLE, rootProcess, communicator(group)) != MPI_SUCCESS)
		{
			msg.print("Failed to broadcast Vec3<double> data from rootProcess %i.\n", rootProcess);
			return FALSE;
		}
	}
	else
	{
		// Slaves receive the data into the buffer, and then set the source variable.
		if (MPI_Bcast(buffer, 3, MPI_DOUBLE, rootProcess, communicator(group)) != MPI_SUCCESS)
		{
			msg.print("Slave %i failed to receive Vec3<double> data from rootProcess %i.\n", rank_, rootProcess);
			return FALSE;
		}
		source.x = buffer[0];
		source.y = buffer[1];
		source.z = buffer[2];
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Broadcast integer(s) to all Processes
 */
bool dUQComm::broadcast(int* source, int count, int rootProcess, dUQComm::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Bcast(source, count, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
	{
		msg.print("Failed to broadcast int data from rootProcess %i.\n", rootProcess);
		return FALSE;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Broadcast double(s) to all Processes
 */
bool dUQComm::broadcast(double* source, int count, int rootProcess, dUQComm::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Bcast(source, count, MPI_DOUBLE, rootProcess, communicator(group)) != MPI_SUCCESS)
	{
		msg.print("Failed to broadcast int data from rootProcess %i.\n", rootProcess);
		return FALSE;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Broadcast float(s) to all Processes
 */
bool dUQComm::broadcast(float* source, int count, int rootProcess, dUQComm::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Bcast(source, count, MPI_FLOAT, rootProcess, communicator(group)) != MPI_SUCCESS)
	{
		msg.print("Failed to broadcast int data from rootProcess %i.\n", rootProcess);
		return FALSE;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Broadcast bool(s) to all Processes
 */
bool dUQComm::broadcast(bool* source, int count, int rootProcess, dUQComm::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (MPI_Bcast(source, count, MPI_INTEGER, rootProcess, communicator(group)) != MPI_SUCCESS)
	{
		msg.print("Failed to broadcast int data from rootProcess %i.\n", rootProcess);
		return FALSE;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*
// Special Array Functions
*/

/*!
 * \brief Reduce (sum) double data to root process
 */
bool dUQComm::sum(double* source, int count, int rootProcess, dUQComm::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	// If we are the target process then we need to construct a temporary buffer to store the received data in.
	if ((group == dUQComm::Leaders) && (!processGroupLeader())) return TRUE;
	if (rank_ == rootProcess)
	{
		double buffer[count];
		if (MPI_Reduce(source, buffer, count, MPI_DOUBLE, MPI_SUM, rootProcess, communicator(group)) != MPI_SUCCESS) return FALSE;
		// Put reduced data back into original buffer
		for (int n=0; n<count; ++n) source[n] = buffer[n];
	}
	else
	{
		// Not the target process, so just send data
		if (MPI_Reduce(source, NULL, count, MPI_DOUBLE, MPI_SUM, rootProcess, MPI_COMM_WORLD) != MPI_SUCCESS) return FALSE;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Reduce (sum) int data to root process
 */
bool dUQComm::sum(int* source, int count, int rootProcess, dUQComm::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	// If we are the target process then we need to construct a temporary buffer to store the received data in.
	if ((group == dUQComm::Leaders) && (!processGroupLeader())) return TRUE;
	if (rank_ == rootProcess)
	{
		int buffer[count];
		if (MPI_Reduce(source, buffer, count, MPI_INTEGER, MPI_SUM, rootProcess, communicator(group)) != MPI_SUCCESS) return FALSE;
		// Put reduced data back into original buffer
		for (int n=0; n<count; ++n) source[n] = buffer[n];
	}
	else
	{
		// Not the target process, so just send data
		if (MPI_Reduce(source, NULL, count, MPI_INTEGER, MPI_SUM, rootProcess, MPI_COMM_WORLD) != MPI_SUCCESS) return FALSE;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Reduce (sum) double data to all processes
 */
bool dUQComm::allSum(double* source, int count, dUQComm::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	double buffer[count];
	if ((group == dUQComm::Leaders) && (!processGroupLeader())) return TRUE;
	if (MPI_Allreduce(source, &buffer, count, MPI_DOUBLE, MPI_SUM, communicator(group)) != MPI_SUCCESS) return FALSE;
	// Put reduced data back into original buffer
	for (int n=0; n<count; ++n) source[n] = buffer[n];
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Reduce (sum) int data to all processes
 */
bool dUQComm::allSum(int* source, int count, dUQComm::CommGroup group)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	int buffer[count];
	if ((group == dUQComm::Leaders) && (!processGroupLeader())) return TRUE;
	if (MPI_Allreduce(source, &buffer, count, MPI_INTEGER, MPI_SUM, communicator(group)) != MPI_SUCCESS) return FALSE;
	// Put reduced data back into original buffer
	for (int n=0; n<count; ++n) source[n] = buffer[n];
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Assemble integer array on target process
 * \details Given that the integer 'array' exists on all processes, and each process has stored nData at the
 * beginning of this array, this function will assemble the data into a complete, continuous array (up to
 * maxData elements in the new array rootDest) on the rootProcess.
 */
bool dUQComm::assemble(int* array, int nData, int* rootDest, int rootMaxData, int rootProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (rank_ == rootProcess)
	{
		int n;
		// The rootProcess' data must be copied into the local array
		for (n=0; n<nData; ++n) rootDest[n] = array[n];

		// Now get data from other processes, appending each chunk to the rootDest array
		int slaveNData;
		for (int n=0; n<nProcesses_; ++n)
		{
			if (rank_ == n) continue;

			// Get length of data to receive
			if (!receive(slaveNData, n)) return FALSE;

			// Check buffer length
			if ((nData + slaveNData) > rootMaxData)
			{
				printf("MPI ERROR: Tried to assemble more data on process '%i' than the array allows for (maxData = %i).\n", rootProcess, rootMaxData);
				return FALSE;
			}
			// Receive data
			if (!receive(&rootDest[nData], slaveNData, n)) return FALSE;
			nData += slaveNData;
		}
	}
	else
	{
		// Slaves send array size and then the data to the master...
		if (!send(nData, rootProcess)) return FALSE;
		if (!send(array, nData, rootProcess)) return FALSE;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*!
 * \brief Assemble double array on target process
 * \details Given that the double 'array' exists on all processes, and each process has stored nData at the
 * beginning of this array, this function will assemble the data into a complete, continuous array (up to
 * maxData elements in the new array rootDest) on the rootProcess.
 */
bool dUQComm::assemble(double* array, int nLocalData, double* rootDest, int rootMaxData, int rootProcess)
{
#ifdef PARALLEL
	totalTime_.start();
	accumTime_.start();
	if (rank_ == rootProcess)
	{
		int n;
		// The rootProcess' data must be copied into the local array
		for (n=0; n<nLocalData; ++n) rootDest[n] = array[n];

		// Now get data from other processes, appending each chunk to the rootDest array
		int slaveNData;
		for (int n=0; n<nProcesses_; ++n)
		{
			if (rank_ == n) continue;
			
			// Get length of data to receive
			if (!receive(slaveNData, n)) return FALSE;
			
			// Check buffer length
			if ((nLocalData + slaveNData) > rootMaxData)
			{
				printf("MPI ERROR: Tried to assemble more data on process '%i' than the array allows for (maxData = %i).\n", rootProcess, rootMaxData);
				return FALSE;
			}
			// Receive data
			if (!receive(&rootDest[nLocalData], slaveNData, n)) return FALSE;
			nLocalData += slaveNData;
		}
	}
	else
	{
		// Slaves send array size and then the data to the master...
		if (!send(nLocalData, rootProcess)) return FALSE;
		if (!send(array, nLocalData, rootProcess)) return FALSE;
	}
	totalTime_.accumulate();
	accumTime_.accumulate();
#endif
	return TRUE;
}

/*
* Decisions
*/

/*!
 * \brief Broadcast logical decision to all processes (Master only)
 */
void dUQComm::decide(bool decision)
{
#ifdef PARALLEL
	if (!Comm.master())
	{
		msg.print("BAD_USAGE - Slave tried to make a decision,\n");
		return;
	}
	if (!Comm.broadcast(&decision, 1)) msg.print("Error broadcasting decision (%i).\n", decision);
#endif
}

/*!
 * \brief Receive logical decision from master (Slaves only)
 */
bool dUQComm::decision()
{
#ifdef PARALLEL
	if (!Comm.slave())
	{
		msg.print("BAD_USAGE - Master tried to receive a decision.\n");
		return FALSE;
	}
	bool data;
	if (!Comm.broadcast(&data, 1)) msg.print("Error receiving decision.\n");
	return data;
#endif
}

/*
// Buffered Random Numbers
*/

/*!
 * \brief Refill random number buffer
 */
void dUQComm::refillRandomBuffer()
{
#ifdef PARALLEL
	// Reset index
	randomBufferIndex_ = 0;
	
	// Generate new random numbers in array
	if (randomBufferCommGroup_ == dUQComm::World)
	{
		// Master creates buffer and sends to slaves
		msg.printVerbose("Random Buffer - World parallel, so master (%s) will create and send array.\n", master() ? "me" : "not me");
		if (master())
		{
			for (int n=0; n<RANDBUFFERSIZE; ++n) randomBuffer_[n] = dUQMath::random();
			Comm.broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
		}
		else Comm.broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
	}
	else if (randomBufferCommGroup_ == dUQComm::Group)
	{
		// Group leader creates buffer and sends to slaves
		msg.printVerbose("Random Buffer - Group parallel, so process leader (%s) will create and send array.\n", processGroupLeader() ? "me" : "not me");
		if (processGroupLeader())
		{
			for (int n=0; n<RANDBUFFERSIZE; ++n) randomBuffer_[n] = dUQMath::random();
			Comm.broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
		}
		else Comm.broadcast(randomBuffer_, RANDBUFFERSIZE, 0, randomBufferCommGroup_);
	}
	else
	{
		// Create own random buffer
		msg.printVerbose("Random Buffer - Solo parallel, so will create own array.\n");
		for (int n=0; n<RANDBUFFERSIZE; ++n) randomBuffer_[n] = dUQMath::random();
	}
#endif
}

/*!
 * \brief Initialise random number buffer
 * \details Reinitialise the random buffer, setting the communicator to use and refilling the array with new randomness.
 */
void dUQComm::initialiseRandomBuffer(dUQComm::CommGroup group)
{
	randomBufferCommGroup_ = group;
	refillRandomBuffer();
}

/*!
 * \brief Get next buffered random number
 */
double dUQComm::random()
{
#ifdef PARALLEL
	// Have we exhausted the buffer?
	if (randomBufferIndex_ == RANDBUFFERSIZE) refillRandomBuffer();
	return randomBuffer_[randomBufferIndex_++];
#else
	return dUQMath::random();
#endif
}

/*!
 * \brief Get next buffered random number (-1 to +1 inclusive)
 */
double dUQComm::randomPlusMinusOne()
{
#ifdef PARALLEL
	// Have we exhausted the buffer?
	if (randomBufferIndex_ == RANDBUFFERSIZE) refillRandomBuffer();
	return (randomBuffer_[randomBufferIndex_++]-0.5)*2.0;
#else
	return dUQMath::randomPlusMinusOne();
#endif
}
