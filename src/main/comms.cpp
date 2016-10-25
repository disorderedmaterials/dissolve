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

/*
 * \brief Broadcast system setup data
 * \details Send all data regarding system setup (Species, Samples, composition etc.) out to slave processes.
 */
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

	// System Composition
	Messenger::printVerbose("[MPI] Broadcasting Composition...\n");
	if (!Comm.broadcast(&multiplier_, 1)) return false;
	if (!Comm.broadcast(&density_, 1)) return false;
	if (!Comm.broadcast(&densityIsAtomic_, 1)) return false;
	if (!Comm.broadcast(relativeBoxLengths_)) return false;
	if (!Comm.broadcast(boxAngles_)) return false;
	if (!Comm.broadcast(&nonPeriodic_, 1)) return false;
	if (!Comm.broadcast(&randomConfiguration_, 1)) return false;

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

	// PairPotentials
	Messenger::printVerbose("Broadcasting PairPotentials...\n");
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

	// Setup Variables
	Messenger::printVerbose("Broadcasting Setup Variables...\n");
	if (!Comm.broadcast(&boxNormalisationPoints_, 1)) return false;
	if (!Comm.broadcast(boxNormalisationFileName_)) return false;
	if (!boxNormalisation_.broadcast()) return false;
	if (!Comm.broadcast(&qDependentFWHM_, 1)) return false;
	if (!Comm.broadcast(&qIndependentFWHM_, 1)) return false;
	if (!Comm.broadcast(&requestedRDFRange_, 1)) return false;
	if (!Comm.broadcast(&rdfRange_, 1)) return false;
	if (!Comm.broadcast(&rdfBinWidth_, 1)) return false;
	if (!Comm.broadcast(&seed_, 1)) return false;
	if (!Comm.broadcast(&temperature_, 1)) return false;

	// Perturbation variables
	if (!Comm.broadcast(&simplexNMoves_, 1)) return false;
	if (!Comm.broadcast(&simplexNCycles_, 1)) return false;
	if (!Comm.broadcast(&simplexTemperature_, 1)) return false;
	if (!Comm.broadcast(&simplexTolerance_, 1)) return false;

	// Partials
	if (!Comm.broadcast((int*)&rdfMethod_, 1)) return false;

	// Simulation Steps
	Messenger::printVerbose("Broadcasting Simulation Steps...\n");
	// -- Equilibration Stage
	count = equilibrationSteps_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) equilibrationSteps_.add();
		equilibrationSteps_[n]->broadcast(commands_);
	}
	// -- Shake Stage
	count = shakeSteps_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) shakeSteps_.add();
		shakeSteps_[n]->broadcast(commands_);
	}
	// -- Strategy Stage
	count = strategySteps_.nItems();
	if (!Comm.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (Comm.slave()) strategySteps_.add();
		strategySteps_[n]->broadcast(commands_);
	}
#endif
	return true;
}
