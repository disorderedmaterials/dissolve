/*
	*** dUQ - Communications
	*** src/main/comms.cpp
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

#include "main/duq.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "base/processpool.h"
#include "base/sysfunc.h"

// Parallel Strategy Keywords
const char* ParallelStrategyKeywords[] = { "Sequential", "Even" };

// Convert string to ParallelStrategy
DUQ::ParallelStrategy DUQ::parallelStrategy(const char* s)
{
	for (int n=0; n<DUQ::nParallelStrategies; ++n) if (DUQSys::sameString(ParallelStrategyKeywords[n], s)) return (DUQ::ParallelStrategy) n;
	return DUQ::nParallelStrategies;
}

// Set parallel strategy for Configuration work
void DUQ::setParallelStrategy(DUQ::ParallelStrategy ps)
{
	parallelStrategy_ = ps;
}

// Return parallel strategy for Configuration work
DUQ::ParallelStrategy DUQ::parallelStrategy()
{
	return parallelStrategy_;
}

// Return a world process pool
ProcessPool& DUQ::worldPool()
{
	static bool firstRun = true;

	// If this is the first time we've been called, construct the pool
	if (firstRun)
	{
		// Assemble list of (world) process ranks for the pool
		Array<int> ranks;
		for (int n=0; n<ProcessPool::nWorldProcesses(); ++n) ranks.add(n);
		worldPool_.setup("World", ranks);
		firstRun = false;
	}

	return worldPool_;
}

// Setup communications
bool DUQ::setupMPIPools()
{
	Messenger::print("*** Setting up MPI pools...\n");

	// If there is only one process, make sure we revert to SequentialConfigStrategy
	if ((ProcessPool::nWorldProcesses() == 1) && (parallelStrategy_ != DUQ::SequentialConfigStrategy))
	{
		Messenger::warn("Parallel strategies make no sense with only one processor, so reverting to sequential strategy default.\n");
		parallelStrategy_ = DUQ::SequentialConfigStrategy;
	}

	// Get relative atom counts between each configuration
	Array<int> configSizes;
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next) configSizes.add(cfg->nAtoms());
// 	configSizes /= configSizes.min();

	// Default pool - all world ranks
	Array<int> allProcesses;
	for (int n=0; n<ProcessPool::nWorldProcesses(); ++n) allProcesses.add(n);

	// Setup pool based on selected strategy
	int cfgIndex = 0;
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		Messenger::print("--> Configuration '%s':\n", cfg->name());

		if (parallelStrategy_ == DUQ::SequentialConfigStrategy)
		{
			// Simple, sequential strategy - all processes assigned to all Configurations
			if (!cfg->setupProcessPool(allProcesses)) return false;
		}
		else if (parallelStrategy_ == DUQ::EvenStrategy)
		{
			// All processes divided equally amongst Configurations - do we have enough?
			if (ProcessPool::nWorldProcesses() < configurations_.nItems())
			{
				Messenger::error("Number of processes (%i) is less than the number of Configurations (%i) so Even strategy can't be employed.\n", ProcessPool::nWorldProcesses(), configurations_.nItems());
				return false;
			}
			else if (ProcessPool::nWorldProcesses()%configurations_.nItems() != 0)
			{
				Messenger::error("Number of processes (%i) does not divide equally amongst the number of Configurations (%i) so Even strategy can't be employed.\n", ProcessPool::nWorldProcesses(), configurations_.nItems());
				return false;
			}

			// Create new pool
			int procsPerConfig = ProcessPool::nWorldProcesses() / configurations_.nItems();
			Array<int> poolProcesses;
			for (int n=0; n<procsPerConfig; ++n) poolProcesses.add(procsPerConfig*cfgIndex + n);
			if (!cfg->setupProcessPool(poolProcesses)) return false;
		}

		// Increase Configuration index
		++cfgIndex;
	}

	return true;
}
