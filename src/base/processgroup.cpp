/*
	*** Process Group
	*** src/base/processgroup.cpp
	Copyright T. Youngs 2012-2020

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

#include "base/processgroup.h"
#include "base/messenger.h"

// Constructor
ProcessGroup::ProcessGroup() : ListItem<ProcessGroup>()
{
}

/*
 * Group Data
 */

// Add process to group
void ProcessGroup::addProcess(int poolRank, int worldRank)
{
	poolRanks_.add(poolRank);
	worldRanks_.add(worldRank);
}

// Return total number of processes in group
int ProcessGroup::nProcesses()
{
	return poolRanks_.nItems();
}

// Return world ranks of group processes
Array<int>& ProcessGroup::worldRanks()
{
	return worldRanks_;
}

// Return nth world rank of group processes
int ProcessGroup::worldRank(int n) const
{
	return worldRanks_.constAt(n);
}

// Return pool ranks of group processes
Array<int>& ProcessGroup::poolRanks()
{
	return poolRanks_;
}

// Return nth pool rank of group processes
int ProcessGroup::poolRank(int n) const
{
	return poolRanks_.constAt(n);
}
