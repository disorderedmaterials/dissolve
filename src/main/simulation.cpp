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
#include "base/sysfunc.h"

// Find first occurrence of named Module in pre-processing stages
Module* DUQ::findPreProcessingStage(const char* name)
{
	for (RefListItem<Module,bool>* ri = preProcessingStages_.first(); ri != NULL; ri = ri->next)
	{
		Module* module = ri->item;

		if (DUQSys::sameString(name, module->name())) return module;
	}

	return NULL;
}

// Find first occurrence of named Module in post-processing stages
Module* DUQ::findPostProcessingStage(const char* name)
{
	for (RefListItem<Module,bool>* ri = postProcessingStages_.first(); ri != NULL; ri = ri->next)
	{
		Module* module = ri->item;

		if (DUQSys::sameString(name, module->name())) return module;
	}

	return NULL;
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
		Messenger::print("===============================================\n");
		Messenger::print("  MAIN LOOP ITERATION %10i / %-10s\n", iteration, maxIterations_ == -1 ? "(no limit)" : DUQSys::itoa(maxIterations_));
		Messenger::print("===============================================\n");
		Messenger::print("\n");

		// Perform pre-processing tasks (using worldPool_)
		for (RefListItem<Module,bool>* ri = preProcessingStages_.first(); ri != NULL; ri = ri->next)
		{
			// Grab Module pointer
			Module* module = ri->item;

			// Execute the pre-processing stage
			if (!module->preProcess(*this, worldPool_)) return false;
		}

		// Sync up all processes
		worldPool_.wait(ProcessPool::Pool);

		// Loop over Configurations, running their modules in the sequence they are defined
		// If a process is not involved in the Configurations ProcessPool, it can move on
		bool result = true;
		for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
		{
			// Check for failure of one or more processes / processing tasks
			if (!worldPool_.ok(result)) 

			// Check involvement of this process
			if (!cfg->processPool().involvesMe()) continue;

			// Loop over Modules defined in the Configuration
			for (RefListItem<Module,bool>* ri = cfg->modules(); ri != NULL; ri = ri->next)
			{
				// Grab Module pointer
				Module* module = ri->item;

				result = module->process(*this, cfg->processPool());

				if (!result) break;
			}
			// TEST - Calculate energy of current system, partial data, write, and quit
			//totalEnergyTest(cfg);
			//totalEnergy(worldPool_, cfg);
			//intramolecularEnergy(worldPool_, cfg);
		}

		// Sync up all processes
		worldPool_.wait(ProcessPool::Pool);

		// Perform post-processing tasks (using worldPool_)
		for (RefListItem<Module,bool>* ri = postProcessingStages_.first(); ri != NULL; ri = ri->next)
		{
			// Grab Module pointer
			Module* module = ri->item;

			// Execute the post-processing stage
			if (!module->postProcess(*this, worldPool_)) return false;
		}

		// Sync up all processes
		worldPool_.wait(ProcessPool::Pool);

		// TODO GLOBAL MODULES HERE - Fitting etc.

		Messenger::print("\n");
		Messenger::print("========================================\n");
		Messenger::print("  END OF MAIN LOOP ITERATION %10i\n", iteration);
		Messenger::print("========================================\n");
		Messenger::print("\n");
	}
	while (iteration < maxIterations_);

	return true;
}

