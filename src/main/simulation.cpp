/*
	*** dUQ - Simulation
	*** src/main/simulation.cpp
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

// Set number of test points to use when calculating Box normalisation arrays
void DUQ::setBoxNormalisationPoints(int nPoints)
{
	boxNormalisationPoints_ = nPoints;
}

// Bin width to use when calculating RMSE between Sample F(Q) and reference data
void DUQ::setRMSEDeltaQ(double dq)
{
	rmseDeltaQ_ = dq;
}

// Set random seed
void DUQ::setSeed(int seed)
{
	seed_ = seed;
}

// Return random seed
int DUQ::seed()
{
	return seed_;
}

// Set window function to use for all Fourier transforms
void DUQ::setWindowFunction(Data2D::WindowFunction wf)
{
	windowFunction_ = wf;
}

// Setup all simulation data, checking it as we go
bool DUQ::setupSimulation()
{
	Messenger::print("\n");
	Messenger::print("*** SYSTEM SETUP BEGINS ***\n");

	/* Check each defined Species */
	Messenger::print("\n");
	Messenger::print("*** Checking Species definitions...\n");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		Messenger::print("--- Species '%s'...\n", sp->name());
		if (!sp->checkSetup(atomTypes_)) return false;
	}

	Messenger::print("\n");
	Messenger::print("*** Setting up Configurations...\n");
	int index = 0;
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next, ++index)
	{
		Messenger::print("*** Configuration %2i: '%s'\n", index, cfg->name());

		if (!cfg->setup(worldPool_, atomTypes_, pairPotentialRange_, boxNormalisationPoints_)) return false;
	}

	/* Pair Potentials */
	/* We expect a PairPotential to have been defined for every combination of AtomType used in the system */
	Messenger::print("\n");
	Messenger::print("Checking PairPotential definitions....TODO!!!!\n");
// 	for (RefListItem<AtomType,int>* ri = typeCount.first(); ri != NULL; ri = ri->next)
// 	{
// 		for (RefListItem<AtomType,int>* rj = ri; rj != NULL; rj = rj->next)
// 		{
// 			PairPotential* pot = hasPairPotential(ri->item, rj->item);
// 			if (pot == NULL)
// 			{
// 				Messenger::error("A PairPotential between AtomTypes '%s' and '%s' is required, but has not been defined.\n", ri->item->name(), rj->item->name());
// 				++nErrors;
// 			}
// 			else if ((ri->data == 0) || (rj->data == 0))
// 			{
// 				Messenger::error("A PairPotential between AtomTypes '%s' and '%s' has been defined, but is not required.\n", ri->item->name(), rj->item->name());
// 				++nErrors;
// 			}
// 		}
// 	}

	/* Prepare Samples */
	Messenger::print("\n");
	Messenger::print("*** Preparing Samples...\n");
	if (!setupSamples())
	{
		Messenger::error("Failed to setup Samples.\n");
		return false;
	}

	/* Finalise AtomTypes */
	// Assign indices to atom types
	Messenger::print("--> Assigning indices to master AtomTypes...\n");
	int id = 0;
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next) at->setIndex(id++);

	// Set global AtomType indices in all Configurations
	Messenger::print("--> Setting global AtomType indices in Configurations...\n");
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next) cfg->setGlobalAtomTypeIndices(atomTypes_);

	// Create PairPotential matrix
	Messenger::print("--> Creating PairPotential matrix (%ix%i)...\n", atomTypes_.nItems(), atomTypes_.nItems());
	if (!potentialMap_.initialise(atomTypes_, pairPotentials_, pairPotentialRange_)) return false;

	return true;
}

// Run Simulation
bool DUQ::go()
{
	/*
	 * This is the main program loop. We perform the following steps:
	 *
	 *  1)	Run Modules in all Configurations according to the defined ParallelStrategy
	 *  2)	Run Calculations in all Samples
	 *  3)	Run any remaining global Modules
	 */

	/*
	 * Start of Main Loop
	 */
	int iteration = 1;
	bool keepGoing = true;

	do
	{
		Messenger::print("\n");
		Messenger::print("==================================\n");
		Messenger::print("  MAIN LOOP ITERATION %10i\n", iteration);
		Messenger::print("==================================\n");
		Messenger::print("\n");

		// Loop over Configurations, running their modules in the sequence they are defined
		// If a process is not involved in the Configurations ProcessPool, it can move on
		for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
		{
			// Check involvement of this process
			if (!cfg->processPool().involvesMe()) continue;

			// Loop over Modules defined in the Configuration
			for (Module* module = cfg->modules(); module != NULL; module = module->next)
			{
// 				
			}
			// TEST - Calculate energy of current system, partial data, write, and quit
			totalEnergyTest(cfg);
			totalEnergy(worldPool_, cfg);
			intramolecularEnergy(worldPool_, cfg);
		}

		// Sync up all processes
		worldPool_.wait(ProcessPool::Pool);

		// TEST - Exit after one iteration
		keepGoing = false;
	}
	while (keepGoing);

	return true;
}
