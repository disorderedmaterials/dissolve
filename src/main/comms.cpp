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
#include "base/comms.h"

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
	if (!Comm.broadcast(&count, 1)) return false;
	Messenger::printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addAtomType(0);
		atomTypes_[n]->broadcast();
	}

	// Species
	Messenger::printVerbose("[MPI] Broadcasting Species...\n");
	count = species_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	Messenger::printVerbose("[MPI] Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addSpecies();
		species_[n]->broadcast(atomTypes_);
	}

	// Reference Data
	// TODO

	// Samples
	Messenger::printVerbose("Broadcasting Samples...\n");
	count = samples_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	Messenger::printVerbose("Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addSample();
		samples_[n]->broadcast(species_);
	}

	// Configurations
	Messenger::printVerbose("[MPI] Broadcasting Configurations...\n");
	count = configurations_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	Messenger::printVerbose("Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) addConfiguration();
		configurations_[n]->broadcast(species_, pairPotentialRange_, allModules_);
	}

	// PairPotentials
	Messenger::printVerbose("[MPI] Broadcasting PairPotentials...\n");
	count = pairPotentials_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	Messenger::printVerbose("Expecting %i items...\n", count);
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) pairPotentials_.add();
		pairPotentials_[n]->broadcast(atomTypes_);
	}
	if (!Comm.broadcast(&pairPotentialRange_, 1)) return false;
	if (!Comm.broadcast(&pairPotentialRangeSquared_, 1)) return false;
	if (!Comm.broadcast(&pairPotentialTruncationWidth_, 1)) return false;
	if (!Comm.broadcast(&pairPotentialDelta_, 1)) return false;

	// Simulation Variables
	Messenger::printVerbose("Broadcasting Simulation Variables...\n");
	if (!Comm.broadcast(&boxNormalisationPoints_, 1)) return false;
	if (!Comm.broadcast(&seed_, 1)) return false;
#endif
	return true;
}
