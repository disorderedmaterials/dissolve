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
	Messenger::print("\n");
	Messenger::print("*** Setting up MPI pools...\n");

	// Get relative atom counts between each configuration
	Array<int> configSizes;
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next) configSizes.add(cfg->nAtoms());
// 	configSizes /= configSizes.min();

	// Setup pools based on selected strategy
	if (parallelStrategy_ == DUQ::SequentialConfigStrategy)
	{
		// Simple, sequential strategy - all processes assigned to all Configurations
		Array<int> processes;
		for (int n=0; n<ProcessPool::nWorldProcesses(); ++n) processes.add(n);

		for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next) if (!cfg->setupProcessPool(processes)) return false;
	}

	return true;
}

// Broadcast system setup data
bool DUQ::broadcastSetup()
{
#ifdef PARALLEL
	bool result;
	int n, count;
	Messenger::print("\n");
	Messenger::print("*** Broadcasting System Setup ***\n");

	// AtomTypes
	Messenger::printVerbose("[MPI] Broadcasting AtomTypes...\n");
	count = atomTypes_.nItems();
	if (!worldPool_.broadcast(&count, 1)) return false;
	Messenger::printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (worldPool_.isSlave()) addAtomType(0);
		atomTypes_[n]->broadcast(worldPool_);
	}

	// Species
	Messenger::printVerbose("[MPI] Broadcasting Species...\n");
	count = species_.nItems();
	if (!worldPool_.broadcast(&count, 1)) return false;
	Messenger::printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (worldPool_.isSlave()) addSpecies();
		species_[n]->broadcast(worldPool_, atomTypes_);
	}

	// Samples
	Messenger::printVerbose("Broadcasting Samples...\n");
	count = samples_.nItems();
	if (!worldPool_.broadcast(&count, 1)) return false;
	Messenger::printVerbose("Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (worldPool_.isSlave()) addSample();
		samples_[n]->broadcast(worldPool_, species_);
	}

	// Configurations
	Messenger::printVerbose("[MPI] Broadcasting Configurations...\n");
	count = configurations_.nItems();
	if (!worldPool_.broadcast(&count, 1)) return false;
	Messenger::printVerbose("Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (worldPool_.isSlave()) addConfiguration();
		configurations_[n]->broadcast(worldPool_, species_, pairPotentialRange_, allModules_);
	}

	// PairPotentials
	Messenger::printVerbose("[MPI] Broadcasting PairPotentials...\n");
	count = pairPotentials_.nItems();
	if (!worldPool_.broadcast(&count, 1)) return false;
	Messenger::printVerbose("Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (worldPool_.isSlave()) pairPotentials_.add();
		pairPotentials_[n]->broadcast(worldPool_, atomTypes_);
	}
	if (!worldPool_.broadcast(&pairPotentialRange_, 1)) return false;
	if (!worldPool_.broadcast(&pairPotentialRangeSquared_, 1)) return false;
	if (!worldPool_.broadcast(&pairPotentialTruncationWidth_, 1)) return false;
	if (!worldPool_.broadcast(&pairPotentialDelta_, 1)) return false;

	// Simulation Variables
	Messenger::printVerbose("Broadcasting Simulation Variables...\n");
	if (!worldPool_.broadcast(&boxNormalisationPoints_, 1)) return false;
	if (!worldPool_.broadcast(&seed_, 1)) return false;
#endif
	return true;
}
