/*
	*** Process Group
	*** src/base/processgroup.h
	Copyright T. Youngs 2012-2018

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_PROCESSGROUP_H
#define DISSOLVE_PROCESSGROUP_H

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
	// Constructor
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
	int nProcesses();
	// Return world ranks of group processes
	Array<int>& worldRanks();
	// Return nth world rank of group processes
	int worldRank(int n);
	// Return pool ranks of group processes
	Array<int>& poolRanks();
	// Return nth pool rank of group processes
	int poolRank(int n);
};

#endif
